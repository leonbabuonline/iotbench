#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include "device.h"
#include "config.h"


typedef struct {
    long long *latencies;
    int        count;
    int        capacity;
    int        total_sent;
    int        total_received;
    int        total_errors;
    double     error_rate;
    double     throughput;
    long long  p50;
    long long  p95;
    long long  p99;
    long long  max;
} GlobalStats;

void stats_compute(GlobalStats *gs, DeviceContext *devices, int num_devices, int duration_sec);
void stats_print(GlobalStats *gs, int num_devices, int duration_sec);
void stats_write_json(GlobalStats *gs, Config *cfg);
void stats_save_json(GlobalStats *gs, Config *cfg);

#endif