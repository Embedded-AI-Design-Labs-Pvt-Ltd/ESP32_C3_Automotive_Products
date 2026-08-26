# Product applications

Each product is a thin composition on `platform/`. Host use-cases are implemented in C and compiled by `tools/scripts/run_all.ps1`.

| ID | Source | Use case |
|---|---|---|
| P01 | `products_connectivity.c` | BLE-CAN gateway |
| P02 | `products_connectivity.c` | BLE OBD-II adapter |
| P03 | `products_diagnostics.c` | UDS tester (0x22 VIN) |
| P04 | `products_diagnostics.c` | CAN logger |
| P05 | `products_diagnostics.c` | Missing-message analyzer |
| P06 | `products_validation.c` | ECU test box (light cmd) |
| P07 | `products_validation.c` | BCM/VCU/BMS simulator + UDS server |
| P08 | `products_validation.c` | Fault injection (drop CAN ID) |
| P09 | `products_validation.c` | EOL sequence |
| P10 | `products_diagnostics.c` | Config/calibration NVS |
| P11 | `products_connectivity.c` | Sensor gateway ADC/I2C |
| P12 | `products_connectivity.c` | TPMS BLE → CAN |
| P13 | `products_connectivity.c` | EV battery + over-temp DTC |
| P14 | `products_security_ota.c` | CAN allow-list IDS |
| P15 | `products_validation.c` | HIL inject/measure |
| P16 | `products_security_ota.c` | Training kit |
| P17 | `products_security_ota.c` | OTA CRC / rollback |

C++ ports: `ports/virtual_ecu`, `ports/arduino`, `ports/raspberry_pi_5`.
