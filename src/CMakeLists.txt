
add_executable(ccxs
        main.c
        ws/client-server-protocol.c ws/client-server-protocol.h
        ws/client-connect.c ws/client-connect.h
        ws/data.h
        io/strings.c io/strings.h
        io/c-json.c io/c-json.h
        depth.c depth.h
        binance/binance.c binance/binance.h binance/binance-depth.c binance/binance-depth.h
        bitfinex/bitfinex.c bitfinex/bitfinex.h bitfinex/bitfinex-depth.c bitfinex/bitfinex-depth.h
        bitstamp/bitstamp.c bitstamp/bitstamp.h bitstamp/bitstamp-depth.c bitstamp/bitstamp-depth.h
        kraken/kraken.c kraken/kraken.h kraken/kraken-depth.c kraken/kraken-depth.h
        okex/okex.c okex/okex.h
        io/time.c io/time.h)


target_link_libraries(ccxs cjson_lib lws_lib ${ZLIB_LIBRARIES} m)
