#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <libwebsockets.h>
#include "bitfinex.h"
#include "../ws/client-server-protocol.h"
#include "../ws/client-connect.h"

static void
bitfinex_subscribe(struct lws *wsi_in) {
    lwsl_user("%s: 订阅\n", __func__);
    const char *m = "{\"event\": \"subscribe\",\"channel\": \"book\",\"symbol\": \"tBTCUSD\",\"prec\": \"P0\", \"freq\": \"F0\", \"len\": 25}";
    websocket_write_back(wsi_in, m);
}

static char *
bitfinex_parse(const char *const json_string) {
    lwsl_user("%s: json: %s\n", __func__, json_string);
    return NULL;
}

int
bitfinex_connect_client(const struct per_vhost_data__minimal *vhd) {
    struct client_user_data *userdata = malloc(sizeof(struct client_user_data));
    userdata->name = "bitfinex";
    userdata->subscribe = bitfinex_subscribe;
    userdata->parse_json = (char *(*)(const void *const)) bitfinex_parse;
    return connect_client(vhd, 443, "api.bitfinex.com", "/ws/2", userdata);
}

