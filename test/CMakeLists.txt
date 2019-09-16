
include_directories("${PROJECT_SOURCE_DIR}/src")

file(COPY resources DESTINATION .)

add_cmocka_test(test_binance_depth
        SOURCES
        ${PROJECT_SOURCE_DIR}/test/binance/test-binance-depth.c
        ${PROJECT_SOURCE_DIR}/src/binance/binance-depth.c
        LINK_LIBRARIES cjson_lib lws_lib cmocka_lib
        LINK_OPTIONS --coverage)
