#ifndef CJSON_EXAMPLE_BINANCE_DEPTH_H
#define CJSON_EXAMPLE_BINANCE_DEPTH_H

#include "cjson/cJSON.h"

typedef struct order {
    double price;
    double volume;
} Order;

#ifndef EMPTY_ORDER
#define EMPTY_ORDER (Order){.price=-1.0, .volume=-1.0}
#endif //EMPTY_ORDER

typedef struct {
    double market;
    double time;
    long id;
    Order bids[100];
    Order asks[100];
} OrderBookLevel2;

int binance_parse_order_node(cJSON *root_node, char *order_side, Order *orders);

int binance_parse_depth_update(const char *json_string);

#endif //CJSON_EXAMPLE_BINANCE_DEPTH_H
