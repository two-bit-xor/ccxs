
add_executable(ccxs
        main.c
        ws/client-server-protocol.c ws/client-server-protocol.h
        ws/client-connect.c ws/client-connect.h
        ws/data.h
        binance/binance.c binance/binance.h
        bitfinex/bitfinex.c bitfinex/bitfinex.h
        kraken/kraken.c kraken/kraken.h
        okex/okex.c okex/okex.h)


target_link_libraries(ccxs cjson_lib zf_log_lib lws_lib ${ZLIB_LIBRARIES})
