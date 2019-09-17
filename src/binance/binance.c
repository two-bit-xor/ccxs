#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <libwebsockets.h>

#include "binance.h"
#include "binance-depth.h"
#include "../ws/client-server-protocol.h"
#include "../ws/client-connect.h"

static char *
binance_parse(const char *const json_string) {
    lwsl_user("%s: json: %s\n", __func__, json_string);
    OrderBookLevel2 *book = binance_parse_depth_update(json_string);
    if (book != NULL) {
        lwsl_user("%s: done. %s\n", __func__, book->market_name);
        orderbook_delete(book);
    }
    return NULL;
}

int
binance_connect_client(const struct per_vhost_data__minimal *vhd) {
    struct client_user_data *userdata = malloc(sizeof(struct client_user_data));
    userdata->name = "binance";
    userdata->subscribe = NULL;
    userdata->parse_json = (char *(*)(const void *const)) binance_parse;
    return connect_client(vhd, 9443, "stream.binance.com", "/ws/btcusdt@depth20@100ms", userdata);
}
