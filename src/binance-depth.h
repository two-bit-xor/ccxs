#ifndef CJSON_EXAMPLE_BINANCE_DEPTH_H
#define CJSON_EXAMPLE_BINANCE_DEPTH_H

#include "cjson/cJSON.h"
#include "depth.h"

int binance_parse_order_node(cJSON *root_node, char *order_side, Order *orders);

int binance_parse_depth_update(const char *json_string);

#endif //CJSON_EXAMPLE_BINANCE_DEPTH_H
