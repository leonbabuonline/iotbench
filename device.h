#ifndef DEVICE_H
#define DEVICE_H

#include <mosquitto.h>
#include <stdint.h>
#include "config.h"

#define MAX_LATENCY_SAMPLES 10000

typedef struct {
    int               device_id;
    const Config     *cfg;

    struct mosquitto *mosq;

    // stats
    int               messages_sent;
    int               messages_received;
    int               errors;

    // latency tracking
    int64_t           latency_samples[MAX_LATENCY_SAMPLES];
    int               latency_count;
    int64_t           t_sent;
} DeviceContext;

void *device_thread(void *arg);

#endif