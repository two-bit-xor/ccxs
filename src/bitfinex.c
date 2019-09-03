#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <libwebsockets.h>
#include "bitfinex.h"

static int rx_seen, tries;
static struct lws *client_wsi;

int bitfinex_connect_client(struct lws_context *context) {
    struct lws_client_connect_info i;

    memset(&i, 0, sizeof(i));
    i.context = context;
    i.port = 443;
    i.address = "api.bitfinex.com";
    i.path = "/ws/2";
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = LCCSCF_USE_SSL;
    i.protocol = BITFINEX_PROTOCOL;
    i.pwsi = &client_wsi;

    tries++;

    if (!lws_client_connect_via_info(&i)) {
        lwsl_user("%s: connection failed\n", __func__);
        client_wsi = NULL;
        return 1;
    }

    return 0;
}

/* *
 * websocket_write_back: write the string data to the destination wsi.
 */
static int websocket_write_back(struct lws *wsi_in, const char *str) {
    if (str == NULL || wsi_in == NULL) {
        return -1;
    }

    int n;
    size_t len = strlen(str);
    unsigned char *out = (unsigned char *) malloc(sizeof(char) * (LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING));

    //* setup the buffer*/
    memcpy (out + LWS_SEND_BUFFER_PRE_PADDING, str, len);

    //* write out*/
    n = lws_write(wsi_in, out + LWS_SEND_BUFFER_PRE_PADDING, len, LWS_WRITE_TEXT);

    lwsl_user("%s: written string %s\n", __func__, str);
    //* free the buffer*/
    free(out);

    return n;
}

void bitfinex_handle_event(struct lws *wsi, enum lws_callback_reasons reason, const void *in) {
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

            const char *m = "{\"event\": \"subscribe\",\"channel\": \"book\",\"symbol\": \"tBTCUSD\",\"prec\": \"P0\", \"freq\": \"F0\", \"len\": 25}";
            websocket_write_back(wsi, m);
            return;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE: {
            lwsl_user("%s: RX: %s\n", __func__, (const char *) in);

//            binance_parse_depth_update((const char *) in);
            rx_seen++;
            return;
        }
        case LWS_CALLBACK_CLIENT_CLOSED: {
            lwsl_user("%s: closed\n", __func__);
            client_wsi = NULL;
            return;
        }
        case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
            lwsl_user("%s: callback cancelled\n", __func__);
            client_wsi = NULL;
            return;
        default:
            lwsl_user("%s: handle: reason=%d\n", __func__, reason);
            return;
    }
}

int bitfinex_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    bitfinex_handle_event(wsi, reason, in);
    return lws_callback_http_dummy(wsi, reason, user, in, len);
}
