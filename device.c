#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <mosquitto.h>
#include "device.h"

// returns current time in milliseconds c
int64_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// called automatically when a message arrives on subscribed topic
void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg) {
    (void)mosq;
    (void)msg;
    DeviceContext *dev = (DeviceContext *)userdata;

    int64_t t_received = now_ms();
    int64_t latency    = t_received - dev->t_sent;

    if (dev->latency_count < MAX_LATENCY_SAMPLES) {
        dev->latency_samples[dev->latency_count++] = latency;
    }

    dev->messages_received++;
}

// the thread function — one per device
void *device_thread(void *arg) {
    DeviceContext *dev = (DeviceContext *)arg;
    const Config  *cfg = dev->cfg;

    // create unique client ID and topics
    char client_id[32];
    char pub_topic[64];
    char sub_topic[64];
    snprintf(client_id, sizeof(client_id), "sensor_%d",  dev->device_id);
    snprintf(pub_topic, sizeof(pub_topic), "bench/data/%s", client_id);
    snprintf(sub_topic, sizeof(sub_topic), "bench/cmd/%s",  client_id);

    // init mosquitto
    dev->mosq = mosquitto_new(client_id, true, dev);
    if (!dev->mosq) {
        fprintf(stderr, "[%s] mosquitto_new failed\n", client_id);
        dev->errors++;
        return NULL;
    }

    // register message callback
    mosquitto_message_callback_set(dev->mosq, on_message);

    // connect to broker
    int rc = mosquitto_connect(dev->mosq, cfg->host, cfg->port, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "[%s] connect failed: %s\n", client_id, mosquitto_strerror(rc));
        dev->errors++;
        mosquitto_destroy(dev->mosq);
        return NULL;
    }

    // subscribe to command topic
    mosquitto_subscribe(dev->mosq, NULL, sub_topic, 0);

    // start background network loop
    mosquitto_loop_start(dev->mosq);

    // publish loop
    int64_t end_time = now_ms() + (int64_t)cfg->duration_sec * 1000;

    while (now_ms() < end_time) {
        char payload[128];
        dev->t_sent = now_ms();

        snprintf(payload, sizeof(payload),
            "{\"device_id\":\"%s\",\"temp\":%.1f,\"ts\":%lld}",
            client_id,
            20.0 + (rand() % 100) / 10.0,
            (long long)dev->t_sent);

        rc = mosquitto_publish(dev->mosq, NULL, pub_topic, strlen(payload), payload, 0, false);
        if (rc != MOSQ_ERR_SUCCESS) {
            dev->errors++;
        } else {
            dev->messages_sent++;
        }

        usleep(cfg->interval_ms * 1000);
    }

    // cleanup
    mosquitto_loop_stop(dev->mosq, true);
    mosquitto_disconnect(dev->mosq);
    mosquitto_destroy(dev->mosq);

    return NULL;
}