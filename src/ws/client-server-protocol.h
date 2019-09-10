#ifndef CJSON_EXAMPLE_CLIENT_SERVER_PROTOCOL_H
#define CJSON_EXAMPLE_CLIENT_SERVER_PROTOCOL_H

#include <libwebsockets.h>

#include "data.h"

int
callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
                 void *user, void *in, size_t len);

int
websocket_write_back(struct lws *wsi_in, const char *str);

void
ring_buffer_insert(struct lws *wsi, char *message);

#define LWS_PLUGIN_PROTOCOL_MINIMAL \
        {  \
                "lws-minimal-proxy", \
                callback_minimal, \
                sizeof(struct per_session_data__minimal), \
                10240, \
                0, NULL, 0 \
        }

#endif //CJSON_EXAMPLE_CLIENT_SERVER_PROTOCOL_H
