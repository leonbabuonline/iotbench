#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int  num_devices;
    char host[128];
    int  port;
    int  interval_ms;
    int  duration_sec;
} Config;

void config_defaults(Config *cfg);
void config_parse(Config *cfg, int argc, char *argv[]);
void config_print(Config *cfg);

#endif