#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>
#include <depth.h>

#include "kraken/kraken-depth.h"
#include "../file.h"

static void
test_kraken_parse_subbed() {
    char *depth_json = read_file("resources/kraken-subbed.json", 193);
    OrderBookLevel2 *actual = kraken_parse_depth_update(depth_json);

    assert_null(actual);

    free(depth_json);
    orderbook_delete(actual);
}

static void
test_kraken_parse_heartbeat() {
    char *depth_json = read_file("resources/kraken-heartbeat.json", 27);
    OrderBookLevel2 *actual = kraken_parse_depth_update(depth_json);

    assert_null(actual);

    free(depth_json);
    orderbook_delete(actual);
}

static void
test_kraken_parse_info() {
    char *depth_json = read_file("resources/kraken-info.json", 113);
    OrderBookLevel2 *actual = kraken_parse_depth_update(depth_json);

    assert_null(actual);

    free(depth_json);
    orderbook_delete(actual);
}

static void
test_kraken_parse_snapshot() {
    char *subbed_json = read_file("resources/kraken-subbed.json", 193);

    assert_null(kraken_parse_depth_update(subbed_json));
    free(subbed_json);

    char *depth_json = read_file("resources/kraken-snapshot.json", 1882);
    OrderBookLevel2 *actual = kraken_parse_depth_update(depth_json);

    assert_string_equal(actual->exchange, "kraken");
    assert_string_equal(actual->market_name, "XBT/USD");
    assert_int_equal(actual->bids_length, 10);
    assert_int_equal(actual->asks_length, 10);
    assert_float_equal(actual->asks[0].price, 10161.40000, 0.01);
    assert_float_equal(actual->asks[0].amount, 5.01437941, 0.01);
    assert_float_equal(actual->bids[0].price, 10161.30000, 0.01);
    assert_float_equal(actual->bids[0].amount, 1.60177616, 0.01);
    free(depth_json);
    orderbook_delete(actual);
}

static void
test_kraken_parse_update() {
    char *subbed_json = read_file("resources/kraken-subbed.json", 193);

    assert_null(kraken_parse_depth_update(subbed_json));
    free(subbed_json);

    char *depth_json = read_file("resources/kraken-update.json", 151);
    OrderBookLevel2 *actual = kraken_parse_depth_update(depth_json);

    assert_string_equal(actual->exchange, "kraken");
    assert_string_equal(actual->market_name, "XBT/USD");
    assert_int_equal(actual->bids_length, 0);
    assert_int_equal(actual->asks_length, 1);
    assert_float_equal(actual->asks[0].price, 9871.20000, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.25068544, 0.01);

    free(depth_json);
    orderbook_delete(actual);
}

int
main() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_kraken_parse_subbed),
            cmocka_unit_test(test_kraken_parse_heartbeat),
            cmocka_unit_test(test_kraken_parse_info),
            cmocka_unit_test(test_kraken_parse_snapshot),
            cmocka_unit_test(test_kraken_parse_update),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
