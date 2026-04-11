#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include <getopt.h>

void config_defaults(Config *cfg) {
    cfg->num_devices  = 10;
    cfg->port         = 1883;
    cfg->interval_ms  = 1000;
    cfg->duration_sec = 30;
    strncpy(cfg->host, "localhost", sizeof(cfg->host));
}

void config_parse(Config *cfg, int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "n:h:p:i:d:")) != -1) {
        switch (opt) {
            case 'n': cfg->num_devices  = atoi(optarg); break;
            case 'h': strncpy(cfg->host, optarg, sizeof(cfg->host)); break;
            case 'p': cfg->port         = atoi(optarg); break;
            case 'i': cfg->interval_ms  = atoi(optarg); break;
            case 'd': cfg->duration_sec = atoi(optarg); break;
            default:
                fprintf(stderr, "Usage: %s -n <devices> -h <host> -p <port> -i <interval_ms> -d <duration_sec>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

void config_print(Config *cfg) {
    printf("==========================================\n");
    printf("  iotbench - MQTT Benchmarking Tool\n");
    printf("==========================================\n");
    printf("  Devices   : %d\n", cfg->num_devices);
    printf("  Host      : %s\n", cfg->host);
    printf("  Port      : %d\n", cfg->port);
    printf("  Interval  : %d ms\n", cfg->interval_ms);
    printf("  Duration  : %d sec\n", cfg->duration_sec);
    printf("==========================================\n");
}

int main(int argc, char *argv[]) {
    Config cfg;
    config_defaults(&cfg);
    config_parse(&cfg, argc, argv);
    config_print(&cfg);
    return 0;
}