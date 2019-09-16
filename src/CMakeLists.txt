
add_executable(ccxs
        main.c
        ws/client-server-protocol.c ws/client-server-protocol.h
        ws/client-connect.c ws/client-connect.h
        ws/data.h
        depth.c depth.h
        binance/binance.c binance/binance.h binance/binance-depth.c binance/binance-depth.h
        bitfinex/bitfinex.c bitfinex/bitfinex.h
        kraken/kraken.c kraken/kraken.h
        okex/okex.c okex/okex.h
        bitstamp/bitstamp.c bitstamp/bitstamp.h depth.c)


target_link_libraries(ccxs cjson_lib lws_lib ${ZLIB_LIBRARIES})
