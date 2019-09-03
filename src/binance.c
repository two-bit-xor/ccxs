#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <libwebsockets.h>

#include "binance.h"
#include "binance-depth.h"

static int rx_seen, tries;
static struct lws *client_wsi;

int binance_connect_client(struct lws_context *context) {
    struct lws_client_connect_info i;

    memset(&i, 0, sizeof(i));
    i.context = context;
    i.port = 9443;
    i.address = "stream.binance.com";
    i.path = "/ws/btcusdt@depth";
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = LCCSCF_USE_SSL;
    i.protocol = BINANCE_PROTOCOL;
    i.pwsi = &client_wsi;

    tries++;

    if (!lws_client_connect_via_info(&i)) {
        client_wsi = NULL;
        return 1;
    }

    return 0;
}

static void binance_handle_event(enum lws_callback_reasons reason, const void *in) {
    switch (reason) {
        case LWS_CALLBACK_PROTOCOL_INIT:
            lwsl_user("%s: protocol init\n", __func__);
            return;
        case LWS_CALLBACK_WSI_CREATE:
            lwsl_user("%s: wsi create\n", __func__);
            return;
            /* because we are protocols[0] ... */
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: {
            lwsl_err("%s: CLIENT_CONNECTION_ERROR: %s\n", __func__, in ? (char *) in : "(null)");
            client_wsi = NULL;
            return;
        }
        case LWS_CALLBACK_CLIENT_ESTABLISHED: {
            lwsl_user("%s: established\n", __func__);
            return;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE: {
            lwsl_user("%s: RX: %s\n", __func__, (const char *) in);

            binance_parse_depth_update((const char *) in);
            rx_seen++;
            return;
        }
        case LWS_CALLBACK_CLIENT_CLOSED: {
            client_wsi = NULL;
            return;
        }
        default:
            return;
    }
}

int binance_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    binance_handle_event(reason, in);
    return lws_callback_http_dummy(wsi, reason, user, in, len);
}
