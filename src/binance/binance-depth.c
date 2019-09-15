#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include "cjson/cJSON.h"
#include "zf_log.h"
#include "binance-depth.h"

bool is_valid_string(const cJSON *node);

double json_to_double(const cJSON *value);

int binance_parse_order_node(cJSON *root_node, char *order_side, Order *orders) {
    const cJSON *order_node = NULL;
    const cJSON *order_book_node = cJSON_GetObjectItemCaseSensitive(root_node, order_side);

    int i = 0;
    cJSON_ArrayForEach(order_node, order_book_node) {
        cJSON *price = cJSON_GetArrayItem(order_node, 0);
        cJSON *volume = cJSON_GetArrayItem(order_node, 1);

        if (is_valid_string(price) && is_valid_string(volume)) {
            Order book = {.price=json_to_double(price), .amount=json_to_double(volume)};
            orders[i++] = book;
        }
    }
    orders[i] = EMPTY_ORDER;
    return i;
}

double json_to_double(const cJSON *value) {
    char *endptr;
    double d = strtod(value->valuestring, &endptr);
    if (d == 0) {
        /* If a conversion error occurred, display a message and exit */
        if (errno == EINVAL) {
            printf("Conversion error occurred: %d.\n", errno);
        }

        if (errno == ERANGE) {
            printf("The value provided was out of range.\n");
        }
    }
    return d;
}

bool is_valid_string(const cJSON *node) { return cJSON_IsString(node) && (node->valuestring != NULL); }

int end(cJSON *node_to_free, int status) {
    cJSON_Delete(node_to_free);
    return status;
}

int binance_parse_depth_update(struct per_vhost_data__minimal *vhd, const char *const json_string) {
    clock_t t_0 = clock();
    int status = 0;
    cJSON *root_node = cJSON_Parse(json_string);
    if (root_node == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            ZF_LOGE("Error before: \"%s\"", error_ptr);
        }
        return end(root_node, 0);
    }

    const cJSON *type = cJSON_GetObjectItemCaseSensitive(root_node, "e");
    if (cJSON_IsString(type) && (type->valuestring != NULL)) {
        if (strcmp("depthUpdate", type->valuestring) != 0) {
            return end(root_node, 0);
        }
    }

    OrderBookLevel2 *order_book = malloc(sizeof(OrderBookLevel2));
    const cJSON *time = cJSON_GetObjectItemCaseSensitive(root_node, "E");
    if (cJSON_IsNumber(time)) {
        order_book->time = type->valuedouble;
    }

    const cJSON *market = cJSON_GetObjectItemCaseSensitive(root_node, "s");
    if (cJSON_IsString(market) && (market->valuestring != NULL)) {
        order_book->market = market->valuedouble;
    }

    int bids = binance_parse_order_node(root_node, "b", order_book->bids);
    int asks = binance_parse_order_node(root_node, "a", order_book->asks);


    double t_1 = ((double) clock() - t_0) / CLOCKS_PER_SEC; // in seconds


    char string_json[500];
    memset(string_json, 0, sizeof string_json);
    sprintf(string_json, "time:%f, exchange:%s, market:%s, bid:%f, ask:%f, delay:%f",
            order_book->time,
            order_book->exchange,
            strlen(order_book->market_name) > 0 ? order_book->market_name : "?",
            bids > 0 ? order_book->bids[0].price : -1,
            asks > 0 ? order_book->asks[0].price : -1,
            t_1
    );
    ZF_LOGI("Done in %f seconds bids=%d, asks=%d, json%s\n", t_1, bids, asks, string_json);
    free(order_book);
    return end(root_node, status);
}