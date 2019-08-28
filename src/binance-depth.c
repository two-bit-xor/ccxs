#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "cjson/cJSON.h"
#include "zf_log.h"
#include "binance-depth.h"

bool is_valid_string(const cJSON *node);

int binance_parse_order_node(cJSON *root_node, char *order_side, Order *orders) {
    const cJSON *order_node = NULL;
    const cJSON *order_book_node = cJSON_GetObjectItemCaseSensitive(root_node, order_side);

    int i = 0;
    cJSON_ArrayForEach(order_node, order_book_node) {
        cJSON *price = cJSON_GetArrayItem(order_node, 0);
        cJSON *volume = cJSON_GetArrayItem(order_node, 1);

        if (is_valid_string(price) && is_valid_string(volume)) {
            Order book = {.price=-atof(price->valuestring), .volume=atof(volume->valuestring)};
            orders[i++] = book;
            ZF_LOGI("%s.price=\"%s\"", order_side, price->valuestring);
            ZF_LOGI("%s.volume=\"%s\"", order_side, volume->valuestring);
        }
    }
    orders[i] = EMPTY_ORDER;
    return i;
}

bool is_valid_string(const cJSON *node) { return cJSON_IsString(node) && (node->valuestring != NULL); }

int end(cJSON *node_to_free, int status) {
    cJSON_Delete(node_to_free);
    return status;
}

int binance_parse_depth_update(const char *const json_string) {
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
        ZF_LOGI("type=\"%s\"", type->valuestring);
        if (strcmp("depthUpdate", type->valuestring) != 0) {
            return end(root_node, 0);
        }
    }

    OrderBookLevel2 order_book;
    const cJSON *time = cJSON_GetObjectItemCaseSensitive(root_node, "E");
    if (cJSON_IsNumber(time)) {
        ZF_LOGI("time=\"%f\"", type->valuedouble);
        order_book.time = type->valuedouble;
    }

    const cJSON *market = cJSON_GetObjectItemCaseSensitive(root_node, "s");
    if (cJSON_IsString(market) && (market->valuestring != NULL)) {
        ZF_LOGI("market=\"%s\"", market->valuestring);
        order_book.market = market->valuedouble;
    }

    binance_parse_order_node(root_node, "b", order_book.bids);
    binance_parse_order_node(root_node, "a", order_book.asks);


    return end(root_node, status);
}