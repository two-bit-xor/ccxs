#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "binance/binance-depth.h"
#include "../file.h"

static void
test_binance_parse_depth_update_btcusd() {
    char *depth_json = read_file("resources/binance-depth20-100ms.json", 2306);
    OrderBookLevel2 *actual = binance_parse_depth_update(depth_json);
    assert_string_equal(actual->exchange, "binance");
    assert_string_equal(actual->market_name, "BTC-USD");
    assert_int_equal(actual->bids_length, 20);
    assert_int_equal(actual->asks_length, 20);
    assert_float_equal(actual->bids[0].price, 10301.550000, 0.01);
    assert_float_equal(actual->bids[0].amount, 0.00136000, 0.01);
    assert_float_equal(actual->bids[1].price, 10301.12000000, 0.01);
    assert_float_equal(actual->bids[1].amount, 0.09112700, 0.01);
    assert_float_equal(actual->asks[0].price, 10303.19000000, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.14510200, 0.01);

    free(depth_json);
    orderbook_delete(actual);
}

int
main() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_binance_parse_depth_update_btcusd),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
