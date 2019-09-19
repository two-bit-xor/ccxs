#ifndef CCXS_BINANCE_DEPTH_H
#define CCXS_BINANCE_DEPTH_H

#include "cjson/cJSON.h"
#include "../depth.h"
#include "../ws/client-server-protocol.h"

OrderBookLevel2 *
binance_parse_depth_update(const char *json_string);

#endif //CCXS_BINANCE_DEPTH_H
