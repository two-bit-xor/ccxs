#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
//#include <libwebsockets/lws-client.h>

#include "cjson/cJSON.h"
#include "zf_log.h"
#include "binance-depth.h"
#include "binance.h"
#include "bitfinex.h"

static int interrupted;

static const struct lws_protocols protocols[] = {
        {BINANCE_PROTOCOL,  binance_callback,  0, 0, 0, NULL, 0},
        {BITFINEX_PROTOCOL, bitfinex_callback, 0, 0, 0, NULL, 0},
        {NULL, NULL,                           0, 0, 0, NULL, 0}
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
    lwsl_user("LWS minimal ws client SPAM\n");

    memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
    info.protocols = protocols;
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

    binance_connect_client(context);
    bitfinex_connect_client(context);

    while (n >= 0 && !interrupted) {
        n = lws_service(context, 1000);
    }

    lws_context_destroy(context);

//    if (tries == limit && closed == tries) {
    lwsl_user("Completed\n");
    return 0;
//    }

//    lwsl_err("Failed\n");
//    return 1;
}
