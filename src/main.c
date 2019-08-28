#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "cjson/cJSON.h"
#include "zf_log.h"
#include "binance-depth.h"

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
            binance_parse_depth_update(buf);
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
            binance_parse_depth_update(buf);
            double t_1 = ((double) clock() - t_0) / CLOCKS_PER_SEC; // in seconds
            ZF_LOGI("Done in %f seconds", t_1);
        }
    }
    ZF_LOGI("okay");
    return 0;
}