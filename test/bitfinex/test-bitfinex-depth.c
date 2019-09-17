#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "bitfinex/bitfinex-depth.h"
#include "../file.h"

static void
test_binance_parse_info() {
    char *depth_json = read_file("resources/test-bitfinex-info.json", 103);
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json);
    assert_null(actual);
}

static void
test_binance_parse_subbed() {
    char *depth_json = read_file("resources/test-bitfinex-subbed.json", 125);
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json);
    assert_null(actual);
}

static void
test_binance_parse_depth_snapshot_btcusd() {
    char *depth_json = read_file("resources/test-bitfinex-snapshot.json", 973);
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "BTC-USD");
    assert_int_equal(actual->bids_length, 25);
    assert_int_equal(actual->asks_length, 25);
    assert_float_equal(actual->bids[0].price, 10196, 0.01);
    assert_float_equal(actual->bids[0].amount, 8.27552856, 0.01);
    assert_float_equal(actual->bids[1].price, 10195, 0.01);
    assert_float_equal(actual->bids[1].amount, 0.001, 0.01);
    assert_float_equal(actual->asks[0].price, 10197, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.0053093, 0.01);

    orderbook_delete(actual);
}

static void
test_binance_parse_depth_update_btcusd() {
    char *depth_json = read_file("resources/test-bitfinex-update.json", 22);
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "BTC-USD");
    assert_int_equal(actual->bids_length, 0);
    assert_int_equal(actual->asks_length, 1);
    assert_float_equal(actual->asks[0].price, 10222, 0.01);
    assert_float_equal(actual->asks[0].amount, 0, 0.01);

    orderbook_delete(actual);
}

int
main() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_binance_parse_info),
            cmocka_unit_test(test_binance_parse_subbed),
            cmocka_unit_test(test_binance_parse_depth_snapshot_btcusd),
            cmocka_unit_test(test_binance_parse_depth_update_btcusd),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
