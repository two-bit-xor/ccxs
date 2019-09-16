#ifndef CJSON_EXAMPLE_BINANCE_DEPTH_H
#define CJSON_EXAMPLE_BINANCE_DEPTH_H

#include "cjson/cJSON.h"
#include "../depth.h"
#include "../ws/client-server-protocol.h"

int binance_parse_order_node(cJSON *root_node, char *order_side, Order *orders);

OrderBookLevel2 *
binance_parse_depth_update(const char *const json_string);

#endif //CJSON_EXAMPLE_BINANCE_DEPTH_H
