#ifndef CCXS_BITSTAMP_DEPTH_H
#define CCXS_BITSTAMP_DEPTH_H

#include "cjson/cJSON.h"
#include "../depth.h"
#include "../ws/client-server-protocol.h"

OrderBookLevel2 *
bitstamp_parse_depth_update(const char *json_string);

#endif //CCXS_BITSTAMP_DEPTH_H
