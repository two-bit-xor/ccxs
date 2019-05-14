#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "cjson/cJSON.h"
#include "zf_log.h"

typedef struct {

} OrderBook;

int parse_order_node(cJSON *root_node, char *order_side) {
    const cJSON *order_array_node = NULL;
    const cJSON *bid_orders_node = cJSON_GetObjectItemCaseSensitive(root_node, order_side);
    cJSON_ArrayForEach(order_array_node, bid_orders_node) {
        cJSON *amount = cJSON_GetArrayItem(order_array_node, 0);
        cJSON *volume = cJSON_GetArrayItem(order_array_node, 1);

        if (cJSON_IsString(amount) && (amount->valuestring != NULL)) {
            ZF_LOGI("%s.amount=\"%s\"", order_side, amount->valuestring);
        }
        if (cJSON_IsString(volume) && (volume->valuestring != NULL)) {
            ZF_LOGI("%s.volume=\"%s\"", order_side, volume->valuestring);
        }
    }
}

int parse_depth_update(const char *const json_string) {
    int status = 0;
    cJSON *root_node = cJSON_Parse(json_string);
    if (root_node == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            ZF_LOGE("Error before: \"%s\"", error_ptr);
        }
        status = 0;
        goto end;
    }

    const cJSON *type = cJSON_GetObjectItemCaseSensitive(root_node, "e");
    if (cJSON_IsString(type) && (type->valuestring != NULL)) {
        ZF_LOGI("type=\"%s\"", type->valuestring);
    }

    const cJSON *time = cJSON_GetObjectItemCaseSensitive(root_node, "E");
    if (cJSON_IsNumber(time)) {
        ZF_LOGI("time=\"%f\"", type->valuedouble);
    }

    const cJSON *market = cJSON_GetObjectItemCaseSensitive(root_node, "s");
    if (cJSON_IsString(market) && (market->valuestring != NULL)) {
        ZF_LOGI("market=\"%s\"", market->valuestring);
    }

    parse_order_node(root_node, "b");
    parse_order_node(root_node, "a");

    end: /* GOTO */
    cJSON_Delete(root_node);
    return status;
}

int
main(int argc, char *argv[]) {
    int fd, n = 1;
    int max_length = 10000;
    char buf[max_length];


    if (argc == 1) {
        ZF_LOGI("cJSON_example (C) 2019 2bitxor");
        ZF_LOGI("  usage: cat my.json | cJSON_example");

        fd = 0;
        while (n > 0) {
            n = read(fd, buf, sizeof(buf));
            if (n <= 0) {
                continue;
            }

            clock_t t_0 = clock();
            parse_depth_update(buf);
            double t_1 = ((double) clock() - t_0) / CLOCKS_PER_SEC; // in seconds
            ZF_LOGI("Done in %f seconds", t_1);
        }
    } else {
        FILE *input_file = fopen(argv[1], "r");
        if (input_file != NULL) {
            int i = 0;
            while (!feof(input_file) && i < max_length) {
                buf[i++] = fgetc(input_file);
            }
            fclose(input_file);
            ZF_LOGI("cJSON_example (C) 2019 2bitxor");
            clock_t t_0 = clock();
            parse_depth_update(buf);
            double t_1 = ((double) clock() - t_0) / CLOCKS_PER_SEC; // in seconds
            ZF_LOGI("Done in %f seconds", t_1);
        }
    }
    ZF_LOGI("okay");
    return 0;
}