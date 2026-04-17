# iotbench — MQTT Benchmarking Tool

A lightweight, multi-threaded MQTT benchmarking tool written in C.  
Simulates concurrent IoT devices publishing messages to an MQTT broker  
and measures throughput, latency percentiles, and error rates.

---

## Features

- Multi-threaded device simulation using pthreads
- Measures p50 / p95 / p99 / max latency per run
- Tracks message delivery rate and error rate
- JSON output via `-o` flag for easy result storage
- Zero memory leaks (Valgrind verified)

---

## Build

**Dependencies:**
```bash
sudo apt install libmosquitto-dev valgrind -y
```

**Compile:**
```bash
make
```

---

## Usage

```bash
./iotbench -n <devices> -d <duration_sec> [-h <host>] [-p <port>] [-i <interval_ms>] [-o <output.json>]
```

| Flag | Description | Default |
|------|-------------|---------|
| `-n` | Number of simulated devices | 10 |
| `-h` | MQTT broker host | localhost |
| `-p` | MQTT broker port | 1883 |
| `-i` | Publish interval per device (ms) | 1000 |
| `-d` | Test duration (seconds) | 30 |
| `-o` | Save results to JSON file | (none) |

**Example:**
```bash
./iotbench -n 100 -d 30 -o results.json
```

---

## Benchmark Results

Tested against **Mosquitto 2.x** on localhost (Ubuntu 24.04).

| Devices | Sent | Received | Errors | Throughput | p50 | p95 | p99 | Max |
|---------|------|----------|--------|------------|-----|-----|-----|-----|
| 10      | 100  | 100      | 0      | 10 msg/s   | 0ms | 1ms | 3ms | 3ms  |
| 50      | 600  | 600      | 0      | 60 msg/s   | 0ms | 2ms | 3ms | 7ms  |
| 100     | 1200 | 1200     | 0      | 120 msg/s  | 0ms | 2ms | 5ms | 60ms |
| 500     | 5000 | 2060     | 0      | 500 msg/s  | 1ms | 9ms | 16ms| 68ms |
| 1000    | 9667 | 1370     | 333    | 966 msg/s  | 0ms | 2ms | 22ms| 60ms |

**Findings:**
- Mosquitto handles up to **100 concurrent devices** with zero message loss
- Message loss begins at **500 devices** due to broker queue saturation
- Connection errors appear at **1000 devices** (3.44% error rate)

---

## Memory Safety

```bash
valgrind --leak-check=full ./iotbench -n 5 -d 10
```
