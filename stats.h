#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include "device.h"

typedef struct {
    int     total_sent;
    int     total_received;
    int     total_errors;
    double  throughput;       // messages per second
    double  error_rate;       // percentage

    // global latency percentiles
    int64_t p50;
    int64_t p95;
    int64_t p99;
    int64_t max;
} GlobalStats;

void stats_compute(GlobalStats *gs, DeviceContext *devices, int num_devices, int duration_sec);
void stats_print(GlobalStats *gs, int num_devices, int duration_sec);

#endif