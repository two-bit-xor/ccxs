#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "cjson/cJSON.h"
#include "binance/binance.h"
#include "bitfinex/bitfinex.h"
#include "ws/client-server-protocol.h"

static int interrupted;

#define LWS_PLUGIN_STATIC

static const struct lws_protocols client_protocols[] = {
        LWS_PLUGIN_PROTOCOL_MINIMAL,
        {NULL, NULL, 0, 0, 0, NULL, 0}
};

static void
sigint_handler(int sig) {
    lwsl_user("LWS sigint received sig=%d\n", sig);
    interrupted = 1;
}

int main() {
    struct lws_context_creation_info info;
    struct lws_context *context;
    int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE
    /* for LLL_ verbosity above NOTICE to be built into lws,
     * lws must have been configured and built with
     * -DCMAKE_BUILD_TYPE=DEBUG instead of =RELEASE */
    /* | LLL_INFO */ /* | LLL_PARSER */ /* | LLL_HEADER */
    /* | LLL_EXT */ /* | LLL_CLIENT */ /* | LLL_LATENCY */
    /* | LLL_DEBUG */;

    signal(SIGINT, sigint_handler);

    lws_set_log_level(logs, NULL);
    lwsl_user("ccxs 你好！\n");

    memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
    info.port = 7681; /* we do not run any server */
    info.protocols = client_protocols;
    info.vhost_name = "localhost";
    info.ws_ping_pong_interval = 60;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT |
                   LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

#if defined(LWS_WITH_MBEDTLS)
    /*
         * OpenSSL uses the system trust store.  mbedTLS has to be told which
         * CA to trust explicitly.
         */
        info.client_ssl_ca_filepath = "./libwebsockets.org.cer";
#endif

    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 1;
    }

    while (n >= 0 && !interrupted) {
        n = lws_service(context, 100);
    }

    lws_context_destroy(context);

    lwsl_user("拜拜\n");
    return 0;
}
