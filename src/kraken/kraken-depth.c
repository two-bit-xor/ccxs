#include <math.h>

#include "kraken-depth.h"
#include "../io/strings.h"
#include "../io/time.h"
#include "../io/c-json.h"

static long kraken_channel_id = 0;

int
kraken_parse_order_node(const cJSON *order_book_node, Order *orders) {
    const cJSON *order_node = NULL;

    int i = 0;
    cJSON_ArrayForEach(order_node, order_book_node) {
        cJSON *price = cJSON_GetArrayItem(order_node, 0);
        cJSON *volume = cJSON_GetArrayItem(order_node, 1);

        if (i < 100 && is_valid_string(price) && is_valid_string(volume)) {
            Order book = {.price=string_to_double(price->valuestring), .amount=string_to_double(volume->valuestring)};
            orders[i++] = book;
        }
    }
    orders[i] = EMPTY_ORDER;
    return i;
}

OrderBookLevel2 *
kraken_parse_depth_update(const char *json_string) {
    clock_t t_0 = clock();

    cJSON *root_node = cJSON_Parse(json_string);
    if (root_node == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            lwsl_err("before: \"%s\"\n", error_ptr);
        }
        return NULL;
    }

    if (cJSON_IsObject(root_node)) {
        const cJSON *type = cJSON_GetObjectItemCaseSensitive(root_node, "event");
        if (type != NULL && cJSON_IsString(type) && type->valuestring != NULL) {
            if (strcmp("heartbeat", type->valuestring) == 0) {
                lwsl_user("%s heartbeat\n", __func__);
                cJSON_Delete(root_node);
                return NULL;
            }
            if (strcmp("systemStatus", type->valuestring) == 0) {
                const cJSON *status = cJSON_GetObjectItemCaseSensitive(root_node, "status");
                lwsl_user("%s status=\"%s\"\n", __func__, status->valuestring);
                cJSON_Delete(root_node);
                return NULL;
            }
            if (strcmp("subscriptionStatus", type->valuestring) == 0) {
                const cJSON *status = cJSON_GetObjectItemCaseSensitive(root_node, "status");
                const cJSON *pair = cJSON_GetObjectItemCaseSensitive(root_node, "pair");
                const cJSON *channel = cJSON_GetObjectItemCaseSensitive(root_node, "channelID");
                kraken_channel_id = channel->valueint;
                lwsl_user("%s subscription: status=\"%s\", pair=\"%s\", channel=\"%ld\"\n",
                         __func__, status->valuestring, pair->valuestring, kraken_channel_id);
                cJSON_Delete(root_node);
                return NULL;
            }
        }
    }
    if (!cJSON_IsArray(root_node)) {
        cJSON_Delete(root_node);
        return NULL;
    }

    int array_size = cJSON_GetArraySize(root_node);
    int item_index = 0;
    cJSON *channel = cJSON_GetArrayItem(root_node, item_index++);
    if (array_size < 4) {
        cJSON_Delete(root_node);
        return NULL;
    }
    if (channel == NULL || !cJSON_IsNumber(channel) || channel->valueint != kraken_channel_id) {
        cJSON_Delete(root_node);
        return NULL;
    }

    OrderBookLevel2 *order_book = malloc(sizeof(OrderBookLevel2));
    order_book->exchange = strndup("kraken", 20);
    order_book->time = find_time();
    order_book->asks_length = 0;
    order_book->bids_length = 0;

    cJSON *payload_0 = cJSON_GetArrayItem(root_node, item_index++);
    if (payload_0 != NULL && cJSON_IsObject(payload_0)) {
        cJSON *asks_snapshot = cJSON_GetObjectItemCaseSensitive(payload_0, "as");
        if (asks_snapshot != NULL) {
            order_book->asks_length = kraken_parse_order_node(asks_snapshot, order_book->asks);
            cJSON *bids_snapshot = cJSON_GetObjectItemCaseSensitive(payload_0, "bs");
            if (bids_snapshot != NULL) {
                order_book->bids_length = kraken_parse_order_node(bids_snapshot, order_book->bids);
            }
            item_index++;
        } else {
            cJSON *asks_update = cJSON_GetObjectItemCaseSensitive(payload_0, "a");
            if (asks_update != NULL) {
                order_book->asks_length = kraken_parse_order_node(asks_update, order_book->asks);
                cJSON *payload_1 = cJSON_GetArrayItem(root_node, item_index++);
                cJSON *bids_update = cJSON_GetObjectItemCaseSensitive(payload_1, "b");
                if (bids_update != NULL) {
                    order_book->bids_length = kraken_parse_order_node(bids_update, order_book->bids);
                    item_index++;
                }
            }
        }
    }
    cJSON *market_node = cJSON_GetArrayItem(root_node, item_index);
    if (market_node != NULL && cJSON_IsString(market_node)) {
        order_book->market_name = strndup(market_node->valuestring, 20);
    }

    double t_1 = ((double) clock() - t_0) / CLOCKS_PER_SEC;

    char string_json[500];
    memset(string_json, 0, 500);
    sprintf(string_json, "time:%f, timestamp: %s, exchange:%s, market:%s, bid:%f, ask:%f, delay:%f",
            order_book->time,
            "placeholder",
            order_book->exchange,
            order_book->market_name && strlen(order_book->market_name) > 0 ? order_book->market_name : "?",
            order_book->bids_length > 0 ? order_book->bids[0].price : -1,
            order_book->asks_length > 0 ? order_book->asks[0].price : -1,
            t_1
    );

    cJSON_Delete(root_node);
    lwsl_user("Done in %f seconds - bids=%d, asks=%d, json={%s}\n", t_1, order_book->bids_length, order_book->asks_length, string_json);
    return order_book;
}
