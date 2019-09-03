#ifndef CJSON_EXAMPLE_BITFINEX_H
#define CJSON_EXAMPLE_BITFINEX_H

#include <libwebsockets.h>

#define BITFINEX_PROTOCOL "bitfinex-protocol"

int bitfinex_connect_client(struct lws_context *context);

int bitfinex_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

#endif //CJSON_EXAMPLE_BITFINEX_H
