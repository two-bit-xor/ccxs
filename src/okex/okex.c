#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <zlib.h>

#include <libwebsockets.h>
#include "okex.h"
#include "../ws/client-server-protocol.h"
#include "../ws/client-connect.h"

#define OKEX_Z_CHUNK 16384

// 看到 https://github.com/okcoin-okex/open-api-v3-sdk

static int
okex_subscribe(struct lws *wsi_in) {
    lwsl_user("%s: 订阅\n", __func__);
    const char *m = "{\"op\": \"subscribe\", \"args\": [\"spot/optimized_depth:BTC-USDT\"]}";
    websocket_write_back(wsi_in, m);
}

static char *
z_msg(const z_stream *stream) { return stream->msg == NULL ? "" : stream->msg; }

/* report a zlib or i/o error */
static void
okex_z_err(int ret, z_stream *stream) {
    switch (ret) {
        case Z_STREAM_ERROR:
            lwsl_warn("%s: invalid compression level: %s\n", __func__, z_msg(stream));
            break;
        case Z_DATA_ERROR:
            lwsl_warn("%s: invalid or incomplete deflate data: %s\n", __func__, z_msg(stream));
            break;
        case Z_MEM_ERROR:
            lwsl_err("%s: out of memory: %s\n", __func__, z_msg(stream));
            break;
        case Z_VERSION_ERROR:
            lwsl_warn("%s: zlib version mismatch: %s\n", __func__, z_msg(stream));
            break;
        default:
            lwsl_warn("%s: unknown issue: %s\n", __func__, z_msg(stream));
            break;
    }
}

static char *
okex_parse(const void *const in) {
    lwsl_user("%s: 接收\n", __func__);
    z_stream stream;

    /* allocate deflate state */
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    int ret = inflateInit2(&stream, -MAX_WBITS);
    if (ret != Z_OK) {
        okex_z_err(ret, &stream);
        return NULL;
    }

    Bytef *out = (Bytef *) malloc(OKEX_Z_CHUNK * sizeof(Bytef));
    stream.next_in = (Bytef *) in;
    stream.avail_in = 0;
    stream.next_out = out;

    int result = Z_OK;
    while (result == Z_OK) {
        stream.avail_in = stream.avail_out = 1; /* force small buffers */
        result = inflate(&stream, Z_NO_FLUSH);

        if (result != Z_OK && result != Z_STREAM_END) {
            okex_z_err(result, &stream);
        }
    }
    if (result == Z_STREAM_END) {
        lwsl_user("%s: json: %s\n", __func__, out);
    }
    inflateEnd(&stream);
    free(out);
    return NULL;
}

int
okex_connect_client(const struct per_vhost_data__minimal *vhd) {
    struct client_user_data *userdata = malloc(sizeof(struct client_user_data));
    userdata->name = "okex";
    userdata->subscribe = okex_subscribe;
    userdata->parse_json = okex_parse;
    return connect_client(vhd, 8443, "real.okex.com", "/ws/v3", userdata);
}
