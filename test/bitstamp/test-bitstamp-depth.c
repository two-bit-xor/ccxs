#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>
#include <depth.h>

#include "bitstamp/bitstamp-depth.h"
#include "../file.h"

static void
test_bitstamp_parse_subbed() {
    char *depth_json = read_file("resources/bitstamp-subbed.json", 84);
    OrderBookLevel2 *actual = bitstamp_parse_depth_update(depth_json);

    assert_null(actual);

    free(depth_json);
}

static void
test_bitstamp_parse_depth_update_btcusd() {
    char *depth_json = read_file("resources/bitstamp-update.json", 2221);
    OrderBookLevel2 *actual = bitstamp_parse_depth_update(depth_json);
    assert_string_equal(actual->exchange, "bitstamp");
    assert_string_equal(actual->market_name, "BTC-USD");
    assert_int_equal(actual->bids_length, 11);
    assert_int_equal(actual->asks_length, 24);
    assert_float_equal(actual->asks[0].price, 10180.250000, 0.01);
    assert_float_equal(actual->asks[0].amount, 1.8, 0.01);

    free(depth_json);
    orderbook_delete(actual);
}

int
main() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_bitstamp_parse_subbed),
            cmocka_unit_test(test_bitstamp_parse_depth_update_btcusd),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
