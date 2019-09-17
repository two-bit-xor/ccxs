#include <math.h>

#include "bitfinex-depth.h"
#include "../io/strings.h"
#include "../io/time.h"

static int bitfinex_channel_id = 0;
static int bitfinex_count = 0;

OrderBookLevel2 *
bitfinex_parse_depth_update(const char *json_string) {
    clock_t t_0 = clock();

    cJSON *root_node = cJSON_Parse(json_string);
    if (root_node == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            lwsl_err("before: \"%s\"\n", error_ptr);
        }
        return NULL;
    }

    const cJSON *type = cJSON_GetObjectItemCaseSensitive(root_node, "event");
    if (type != NULL && cJSON_IsString(type) && (type->valuestring != NULL)) {
        if (strcmp("info", type->valuestring) == 0) {
            const cJSON *version = cJSON_GetObjectItemCaseSensitive(root_node, "version");
            const cJSON *status = cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItemCaseSensitive(root_node, "platform"), "status");
            lwsl_user("%s Welcome: version=%d status=%d\n", __func__, version->valueint, status->valueint);
            cJSON_Delete(root_node);
            return NULL;
        }
        if (strcmp("subscribed", type->valuestring) == 0) {
            const cJSON *channel_name = cJSON_GetObjectItemCaseSensitive(root_node, "channel");
            const cJSON *channel_id = cJSON_GetObjectItemCaseSensitive(root_node, "chanId");
            lwsl_user("%s subscribed: name=\"%s\" id=%d\n", __func__, channel_name->valuestring, (bitfinex_channel_id = channel_id->valueint));
            cJSON_Delete(root_node);
            return NULL;
        }
        if (strcmp("error", type->valuestring) == 0) {
            const cJSON *message = cJSON_GetObjectItemCaseSensitive(root_node, "msg");
            const cJSON *code = cJSON_GetObjectItemCaseSensitive(root_node, "code");
            lwsl_err("%s subscription failure: msg=\"%s\" code=%d\n", __func__, message->valuestring, code->valueint);
            cJSON_Delete(root_node);
            return NULL;
        }
    }

    if (!cJSON_IsArray(root_node)) {
        cJSON_Delete(root_node);
        return NULL;
    }
    cJSON *channel_id = cJSON_GetArrayItem(root_node, 0);
    if (bitfinex_channel_id != channel_id->valueint) {
        lwsl_user("%s wrong channel id=%d\n", __func__, channel_id->valueint);
        cJSON_Delete(root_node);
        return NULL;
    }
    cJSON *first_element = cJSON_GetArrayItem(root_node, 1);
    if (first_element != NULL && cJSON_IsString(first_element) && (first_element->valuestring != NULL)) {
        if (strcmp("hb", first_element->valuestring) == 0) {
            lwsl_user("%s hb received\n", __func__);
            cJSON_Delete(root_node);
            return NULL;
        }
    }
    OrderBookLevel2 *order_book = malloc(sizeof(OrderBookLevel2));
    order_book->exchange = strndup("bitfinex", 20);
    order_book->market_name = strndup("BTC-USD", 20);
    order_book->time = find_time();

    cJSON *first_item = cJSON_GetArrayItem(first_element, 0);
    if (cJSON_IsArray(first_item)) {
        const cJSON *order_node = NULL;
        // Then this is a snapshot event.
        order_book->bids_length = 0;
        order_book->asks_length = 0;
        cJSON_ArrayForEach(order_node, first_element) {
            cJSON *price = cJSON_GetArrayItem(order_node, 0);
            cJSON *count = cJSON_GetArrayItem(order_node, 1);
            cJSON *amount = cJSON_GetArrayItem(order_node, 2);

            if (amount->valuedouble > 0) {
                Order book = {.price=price->valuedouble, .amount=count->valueint == 0 ? 0.0 : fabs(amount->valuedouble)};
                order_book->bids[order_book->bids_length++] = book;
            } else {
                Order book = {.price=price->valuedouble, .amount=count->valueint == 0 ? 0.0 : fabs(amount->valuedouble)};
                order_book->asks[order_book->asks_length++] = book;
            }
        }
        order_book->bids[order_book->bids_length] = EMPTY_ORDER;
        order_book->asks[order_book->asks_length] = EMPTY_ORDER;
    } else {
        // This is an update event.
        order_book->bids_length = 0;
        order_book->asks_length = 0;

        cJSON *price = cJSON_GetArrayItem(first_element, 0);
        cJSON *count = cJSON_GetArrayItem(first_element, 1);
        cJSON *amount = cJSON_GetArrayItem(first_element, 2);

        if (amount->valuedouble > 0) {
            Order book = {.price=price->valuedouble, .amount=count->valueint == 0 ? 0.0 : fabs(amount->valuedouble)};
            order_book->bids[order_book->bids_length++] = book;
        } else {
            Order book = {.price=price->valuedouble, .amount=count->valueint == 0 ? 0.0 : fabs(amount->valuedouble)};
            order_book->asks[order_book->asks_length++] = book;
        }
        order_book->bids[order_book->bids_length] = EMPTY_ORDER;
        order_book->asks[order_book->asks_length] = EMPTY_ORDER;
    }

    double t_1 = ((double) clock() - t_0) / CLOCKS_PER_SEC;
    bitfinex_count++;

    char string_json[500];
    memset(string_json, 0, 500);
    sprintf(string_json, "time:%f, id: %d, exchange:%s, market:%s, bid:%f, ask:%f, delay:%f",
            order_book->time,
            bitfinex_count,
            order_book->exchange,
            order_book->market_name && strlen(order_book->market_name) > 0 ? order_book->market_name : "?",
            order_book->bids_length > 0 ? order_book->bids[0].price : -1,
            order_book->asks_length > 0 ? order_book->asks[0].price : -1,
            t_1
    );
    lwsl_user("Done in %f seconds - bids=%d, asks=%d, json={%s}\n", t_1, order_book->bids_length, order_book->asks_length, string_json);
    return order_book;
}
