#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "kraken.h"
#include "../ws/client-connect.h"
#include "../ws/client-server-protocol.h"


static void
kraken_subscribe(struct lws *wsi_in) {
    lwsl_user("%s: 订阅\n", __func__);
    const char *m = "{\"event\":\"subscribe\",\"pair\":[\"XBT/USD\"],\"subscription\":{\"name\":\"book\"}}";
    websocket_write_back(wsi_in, m);
}

static char *
kraken_parse(const char *const json_string) {
    lwsl_user("%s: json: %s\n", __func__, json_string);
    return NULL;
}

int
kraken_connect_client(const struct per_vhost_data__minimal *vhd) {
    struct client_user_data *userdata = malloc(sizeof(struct client_user_data));
    userdata->name = "kraken";
    userdata->subscribe = kraken_subscribe;
    userdata->parse_json = (char *(*)(const void *const)) kraken_parse;
    return connect_client(vhd, 443, "ws.kraken.com", "/", userdata);
}
