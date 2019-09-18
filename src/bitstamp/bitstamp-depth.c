#include <math.h>

#include "bitstamp-depth.h"
#include "../io/strings.h"
#include "../io/time.h"
#include "../io/c-json.h"

static char *bitstamp_last_update = 0;

int
bitstamp_parse_order_node(const cJSON *order_book_node, Order *orders) {
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
bitstamp_parse_depth_update(const char *json_string) {
    clock_t t_0 = clock();

    cJSON *root_node = cJSON_Parse(json_string);
    if (root_node == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            lwsl_err("before: \"%s\"\n", error_ptr);
        }
        return NULL;
    }

    const cJSON *channel_name = cJSON_GetObjectItemCaseSensitive(root_node, "channel");
    const cJSON *type = cJSON_GetObjectItemCaseSensitive(root_node, "event");
    if (type != NULL && cJSON_IsString(type) && type->valuestring != NULL) {
        if (strcmp("bts:subscription_succeeded", type->valuestring) == 0) {
            lwsl_user("%s subscribed: name=\"%s\"\n", __func__, channel_name->valuestring);
            cJSON_Delete(root_node);
            return NULL;
        }
        if (strcmp("error", type->valuestring) == 0) {
            lwsl_err("%s error: msg=\"%s\"\n", __func__, json_string);
            cJSON_Delete(root_node);
            return NULL;
        }
    }

    const cJSON *data_node = cJSON_GetObjectItemCaseSensitive(root_node, "data");
    if (data_node == NULL) {
        cJSON_Delete(root_node);
        return NULL;
    }
    const cJSON *exchange_timestamp = cJSON_GetObjectItemCaseSensitive(data_node, "microtimestamp");
    if (exchange_timestamp != NULL && type->valuestring != NULL) {
        bitstamp_last_update = exchange_timestamp->valuestring;
    }

    OrderBookLevel2 *order_book = malloc(sizeof(OrderBookLevel2));
    order_book->exchange = strndup("bitstamp", 20);
    order_book->market_name = strndup("BTC-USD", 20);
    order_book->time = find_time();

    // This is an update event.
    order_book->bids_length = bitstamp_parse_order_node(cJSON_GetObjectItemCaseSensitive(data_node, "bids"), order_book->bids);
    order_book->asks_length = bitstamp_parse_order_node(cJSON_GetObjectItemCaseSensitive(data_node, "asks"), order_book->asks);


    double t_1 = ((double) clock() - t_0) / CLOCKS_PER_SEC;

    char string_json[500];
    memset(string_json, 0, 500);
    sprintf(string_json, "time:%f, timestamp: %s, exchange:%s, market:%s, bid:%f, ask:%f, delay:%f",
            order_book->time,
            bitstamp_last_update,
            order_book->exchange,
            order_book->market_name && strlen(order_book->market_name) > 0 ? order_book->market_name : "?",
            order_book->bids_length > 0 ? order_book->bids[0].price : -1,
            order_book->asks_length > 0 ? order_book->asks[0].price : -1,
            t_1
    );
    lwsl_user("Done in %f seconds - bids=%d, asks=%d, json={%s}\n", t_1, order_book->bids_length, order_book->asks_length, string_json);
    return order_book;
}
