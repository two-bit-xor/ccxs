#ifndef CJSON_EXAMPLE_BINANCE_H
#define CJSON_EXAMPLE_BINANCE_H

#include <libwebsockets.h>

#define BINANCE_PROTOCOL "binance-protocol"

int binance_connect_client(struct lws_context *context);

int binance_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

#endif //CJSON_EXAMPLE_BINANCE_H
