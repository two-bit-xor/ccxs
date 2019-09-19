#include <math.h>

#include "kraken-depth.h"
#include "../io/strings.h"
#include "../io/time.h"
#include "../io/c-json.h"

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

    OrderBookLevel2 *order_book = malloc(sizeof(OrderBookLevel2));
    order_book->exchange = strndup("bitstamp", 20);
    order_book->market_name = strndup("BTC-USD", 20);
    order_book->time = find_time();


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
