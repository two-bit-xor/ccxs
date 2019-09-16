#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include <libwebsockets.h>

#include "cjson/cJSON.h"
#include "binance-depth.h"

static int last_update_id = 0;

bool
is_valid_string(const cJSON *node);

double
json_to_double(const cJSON *value);

int
binance_parse_order_node(cJSON *root_node, char *order_side, Order *orders) {
    const cJSON *order_node = NULL;
    const cJSON *order_book_node = cJSON_GetObjectItemCaseSensitive(root_node, order_side);

    int i = 0;
    cJSON_ArrayForEach(order_node, order_book_node) {
        cJSON *price = cJSON_GetArrayItem(order_node, 0);
        cJSON *volume = cJSON_GetArrayItem(order_node, 1);

        if (i < 100 && is_valid_string(price) && is_valid_string(volume)) {
            Order book = {.price=json_to_double(price), .amount=json_to_double(volume)};
            orders[i++] = book;
        }
    }
    orders[i] = EMPTY_ORDER;
    return i;
}

double
json_to_double(const cJSON *value) {
    char *endptr;
    double d = strtold(value->valuestring, &endptr);
    if (d == 0) {
        /* If a conversion error occurred, display a message and exit */
        if (errno == EINVAL) {
            lwsl_err("Conversion error occurred: %d.\n", errno);
        }

        if (errno == ERANGE) {
            lwsl_err("The value provided was out of range.\n");
        }
    }
    return d;
}

bool
is_valid_string(const cJSON *node) { return cJSON_IsString(node) && (node->valuestring != NULL); }

int
end(cJSON *node_to_free, int status) {
    cJSON_Delete(node_to_free);
    return status;
}

OrderBookLevel2 *
binance_parse_depth_update(const char *const json_string) {
    clock_t t_0 = clock();

    cJSON *root_node = cJSON_Parse(json_string);
    if (root_node == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            lwsl_err("before: \"%s\"", error_ptr);
        }
        return NULL;
    }

    const cJSON *type = cJSON_GetObjectItemCaseSensitive(root_node, "e");
    if (type != NULL && cJSON_IsString(type) && (type->valuestring != NULL)) {
        if (strcmp("depthUpdate", type->valuestring) != 0) {
            cJSON_Delete(root_node);
            return NULL;
        }
    }

    OrderBookLevel2 *order_book = malloc(sizeof(OrderBookLevel2));
    order_book->exchange = strndup("binance", 20);
    order_book->market_name = strndup("BTCUSD", 20);

    const cJSON *last_id = cJSON_GetObjectItemCaseSensitive(root_node, "lastUpdateId");
    if (cJSON_IsNumber(last_id)) {
        last_update_id = last_id->valueint;
    }

    order_book->bids_length = binance_parse_order_node(root_node, "bids", order_book->bids);
    order_book->asks_length = binance_parse_order_node(root_node, "asks", order_book->asks);

    double t_1 = ((double) clock() - t_0) / CLOCKS_PER_SEC; // in seconds

    char string_json[500];
    memset(string_json, 0, 500);
    sprintf(string_json, "time:%f, id: %d, exchange:%s, market:%s, bid:%f, ask:%f, delay:%f",
            order_book->time,
            last_update_id,
            order_book->exchange,
            order_book->market_name && strlen(order_book->market_name) > 0 ? order_book->market_name : "?",
            order_book->bids_length > 0 ? order_book->bids[0].price : -1,
            order_book->asks_length > 0 ? order_book->asks[0].price : -1,
            t_1
    );
    lwsl_user("Done in %f seconds - bids=%d, asks=%d, json={%s}\n", t_1, order_book->bids_length, order_book->asks_length, string_json);
    return order_book;
}
