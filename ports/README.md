# Ports

Same platform APIs, different silicon or host.

| Port | Status after architecture | Notes |
|---|---|---|
| `esp32_c3/` | Primary | ESP-IDF, TWAI, NimBLE, NVS |
| `virtual_ecu/` | Parallel with Phase 3 | PC process, no hardware, GUI-driven |
| `arduino/` | After ESP32-C3 core is stable | Similar drivers, Arduino HAL shim |
| `raspberry_pi_5/` | After ESP32-C3 core is stable | SocketCAN, BlueZ, Linux threads instead of FreeRTOS |

A port may replace `drivers/` and `bsp/` only. Protocols and services must not include ESP-IDF headers.
