#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "bitstamp.h"
#include "../ws/client-connect.h"
#include "../ws/client-server-protocol.h"


static void
bitstamp_subscribe(struct lws *wsi_in) {
    lwsl_user("%s: 订阅\n", __func__);
    websocket_write_back(wsi_in,
                         "{\n"
                         "    \"event\": \"bts:subscribe\",\n"
                         "    \"data\": {\n"
                         "        \"channel\": \"diff_order_book_btcusd\"\n"
                         "    }\n"
                         "}");
    websocket_write_back(wsi_in,
                         "{\n"
                         "    \"event\": \"bts:subscribe\",\n"
                         "    \"data\": {\n"
                         "        \"channel\": \"detail_order_book_btcusd\"\n"
                         "    }\n"
                         "}");
}

static char *
bitstamp_parse(const char *const json_string) {
    lwsl_user("%s: json: %s\n", __func__, json_string);
    return NULL;
}

int
bitstamp_connect_client(const struct per_vhost_data__minimal *vhd) {
    struct client_user_data *userdata = malloc(sizeof(struct client_user_data));
    userdata->name = "bitstamp";
    userdata->subscribe = bitstamp_subscribe;
    userdata->parse_json = (char *(*)(const void *const)) bitstamp_parse;
    return connect_client(vhd, 443, "ws.bitstamp.net", "/", userdata);
}

