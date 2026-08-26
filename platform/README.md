# Core platform

Reusable modules only. Product behavior lives in `../products/`.

| Module | Language | Responsibility |
|---|---|---|
| `common/` | C | Types, error codes, static config, compile-time product flags |
| `hal/` | C | Hardware-agnostic contracts (GPIO, UART, I2C, SPI, ADC, timer, TWAI, BLE, NVS, WDT) |
| `bsp/` | C | Board pin map, clocks, transceiver enable, LED/button |
| `drivers/` | C | ESP32-C3 (or port) implementations of HAL contracts |
| `middleware/` | C | FreeRTOS IPC wrappers, logger, config store, health, watchdog feeder |
| `protocols/` | C | CAN service, ISO-TP, UDS, OBD-II, XCP/CCP concepts |
| `services/` | C | DTC, session, gateway router, security, OTA, fault manager |
| `framework/` | C | Ring buffer, state machine, callback registry, static pools |

C++ is reserved for host tools and Virtual ECU. Firmware stays C for MISRA-inspired review and predictable ABI.
