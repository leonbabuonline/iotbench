#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <mosquitto.h>
#include "config.h"
#include "device.h"

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

    // init mosquitto library
    mosquitto_lib_init();

    // allocate array of DeviceContext (one per device)
    DeviceContext *devices = calloc(cfg.num_devices, sizeof(DeviceContext));
    pthread_t     *threads = calloc(cfg.num_devices, sizeof(pthread_t));

    // spawn one thread per device
    printf("\n[*] Launching %d device threads...\n", cfg.num_devices);
    for (int i = 0; i < cfg.num_devices; i++) {
        devices[i].device_id = i;
        devices[i].cfg       = &cfg;
        pthread_create(&threads[i], NULL, device_thread, &devices[i]);
    }

    // wait for all threads to finish
    for (int i = 0; i < cfg.num_devices; i++) {
        pthread_join(threads[i], NULL);
    }

    // print results
    printf("\n==========================================\n");
    printf("  RESULTS\n");
    printf("==========================================\n");
    for (int i = 0; i < cfg.num_devices; i++) {
        printf("  sensor_%d → sent: %d | received: %d | errors: %d\n",
            devices[i].device_id,
            devices[i].messages_sent,
            devices[i].messages_received,
            devices[i].errors);
    }
    printf("==========================================\n");

    // cleanup
    free(devices);
    free(threads);
    mosquitto_lib_cleanup();

    return 0;
}