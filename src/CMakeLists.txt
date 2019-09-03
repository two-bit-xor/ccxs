
add_executable(cJSON_example
        main.c
        binance.c
        binance.h
        binance-depth.c
        binance-depth.h
        bitfinex-depth.c
        depth.h
        bitfinex.c
        bitfinex.h)

target_link_libraries(cJSON_example cjson_lib zf_log_lib lws_lib)
