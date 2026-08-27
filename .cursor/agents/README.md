# Specialized agents — three-target platform

Architecture for POSIX Virtual ECU, ESP32-C3/FreeRTOS, and cloud is in `docs/architecture/`.

**Next implementation agent: `osal_engineer.md` (phase A2).** Do not start BLE or cloud first.

| File | Use for |
|---|---|
| [system_architect.md](system_architect.md) | Layering, nodes, no IDF in app |
| [repository_architect.md](repository_architect.md) | Folders, CMake, no big-bang rename |
| [cpp_engineer.md](cpp_engineer.md) | Types, errors, rings, tests |
| [osal_engineer.md](osal_engineer.md) | **Next** — POSIX OSAL |
| [posix_virtual_hw.md](posix_virtual_hw.md) | Simulated HAL + fault inject |
| [esp32c3_platform.md](esp32c3_platform.md) | ESP-IDF isolation |
| [freertos_engineer.md](freertos_engineer.md) | Task catalog |
| [can_engineer.md](can_engineer.md) | CAN HAL/service |
| [can_integrity.md](can_integrity.md) | CRC, alive, plausibility |
| [uds_engineer.md](uds_engineer.md) | ISO-TP/UDS harden |
| [virtual_ecu.md](virtual_ecu.md) | ECU state machine |
| [vehicle_simulator.md](vehicle_simulator.md) | Vehicle model + CLI |
| [health_monitor_engineer.md](health_monitor_engineer.md) | Health API |
| [diagnostics_engineer.md](diagnostics_engineer.md) | DTC |
| [safety_engineer.md](safety_engineer.md) | FMEA, no ISO 26262 claim |
| [cybersecurity_engineer.md](cybersecurity_engineer.md) | Prototype IDS |
| [bootloader_engineer.md](bootloader_engineer.md) | POSIX OTA flow |
| [cloud_engineer.md](cloud_engineer.md) | MQTT/TLS, no secrets in git |
| [ble_engineer.md](ble_engineer.md) | After CAN/UDS |
| [driver_engineer.md](driver_engineer.md) | UART/GPIO/I2C/SPI/ADC |
| [hil_engineer.md](hil_engineer.md) | HIL |
| [test_engineer.md](test_engineer.md) | Every phase |
| [posix_build.md](posix_build.md) | CMake/CTest |
| [esp_idf_build.md](esp_idf_build.md) | idf.py |
| [cicd_engineer.md](cicd_engineer.md) | GitHub Actions |
| [code_reviewer.md](code_reviewer.md) | CRITICAL→LOW |
| [product_integration.md](product_integration.md) | Thin products, no duplicated core |

Numbered files `01-*.md` … `20-*.md` are short legacy reminders.

Rules: `.cursor/rules/*.mdc`.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
