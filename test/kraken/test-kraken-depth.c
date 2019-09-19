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
    char *depth_json = read_file("resources/kraken-subbed.json", 153);
    OrderBookLevel2 *actual = kraken_parse_depth_update(depth_json);

    assert_null(actual);

    free(depth_json);
    orderbook_delete(actual);
}

int
main() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_kraken_parse_subbed),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
