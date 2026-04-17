#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stats.h"

// comparison function for qsort
static int cmp_int64(const void *a, const void *b) {
    int64_t x = *(int64_t *)a;
    int64_t y = *(int64_t *)b;
    return (x > y) - (x < y);
}

int64_t percentile(int64_t *sorted, int count, double p) {
    if (count == 0) return 0;
    int idx = (int)(p * count);
    if (idx >= count) idx = count - 1;
    return sorted[idx];
}

void stats_compute(GlobalStats *gs, DeviceContext *devices, int num_devices, int duration_sec) {
    memset(gs, 0, sizeof(GlobalStats));

    // count totals across all devices
    for (int i = 0; i < num_devices; i++) {
        gs->total_sent     += devices[i].messages_sent;
        gs->total_received += devices[i].messages_received;
        gs->total_errors   += devices[i].errors;
    }

    // throughput and error rate
    gs->throughput  = (double)gs->total_sent / duration_sec;
    gs->error_rate  = gs->total_sent > 0
                      ? (double)gs->total_errors / gs->total_sent * 100.0
                      : 0.0;

    // collect ALL latency samples from all devices into one big array
    int total_samples = 0;
    for (int i = 0; i < num_devices; i++) {
        total_samples += devices[i].latency_count;
    }

    if (total_samples == 0) return;

    int64_t *all = malloc(total_samples * sizeof(int64_t));
    int idx = 0;
    for (int i = 0; i < num_devices; i++) {
        for (int j = 0; j < devices[i].latency_count; j++) {
            all[idx++] = devices[i].latency_samples[j];
        }
    }

    // sort all samples — needed for percentile calculation
    qsort(all, total_samples, sizeof(int64_t), cmp_int64);

    // calculate percentiles
    gs->p50 = percentile(all, total_samples, 0.50);
    gs->p95 = percentile(all, total_samples, 0.95);
    gs->p99 = percentile(all, total_samples, 0.99);
    gs->max = all[total_samples - 1];

    free(all);
}

void stats_print(GlobalStats *gs, int num_devices, int duration_sec) {
    printf("\n==========================================\n");
    printf("  iotbench FINAL REPORT\n");
    printf("==========================================\n");
    printf("  Devices          : %d\n",    num_devices);
    printf("  Duration         : %d sec\n", duration_sec);
    printf("  Messages sent    : %d\n",    gs->total_sent);
    printf("  Messages received: %d\n",    gs->total_received);
    printf("  Errors           : %d (%.2f%%)\n", gs->total_errors, gs->error_rate);
    printf("  Throughput       : %.2f msg/sec\n", gs->throughput);
    printf("  ------------------------------------------\n");
    printf("  Latency (all devices combined):\n");
    printf("    p50  : %lld ms\n", (long long)gs->p50);
    printf("    p95  : %lld ms\n", (long long)gs->p95);
    printf("    p99  : %lld ms\n", (long long)gs->p99);
    printf("    max  : %lld ms\n", (long long)gs->max);
    printf("==========================================\n");
}