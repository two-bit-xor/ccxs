
add_executable(cJSON_example
        main.c
        binance/binance.c binance/binance.h
        bitfinex/bitfinex.c bitfinex/bitfinex.h
        ws/client-server-protocol.c ws/client-server-protocol.h
        ws/client-connect.c ws/client-connect.h ws/data.h)


target_link_libraries(cJSON_example cjson_lib zf_log_lib lws_lib)
