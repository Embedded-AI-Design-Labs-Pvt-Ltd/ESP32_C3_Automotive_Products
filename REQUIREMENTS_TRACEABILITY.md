# REQUIREMENTS_TRACEABILITY

Prototype requirements (REQ-LAB-*). Not OEM production / not ASIL allocated.

| Req ID | Statement | Implementation | Tests |
|---|---|---|---|
| REQ-LAB-001 | One portable C core; products thin | `platform/`, `products/` | `test_all_products` |
| REQ-LAB-002 | Classical CAN only on ESP32-C3 | Docs + HAL DLC≤8 | Host CAN tests |
| REQ-LAB-003 | External CAN transceiver required | `docs/hardware/hardware_architecture.md` | Review |
| REQ-LAB-004 | HAL isolates hardware | `hal_can.h`, `hal_misc.h` | `hal_host` coverage |
| REQ-LAB-005 | CAN service filter/expect/stats | `can_service.*` | `test_coverage` |
| REQ-LAB-006 | ISO-TP SF/FF/CF/FC | `isotp.*` | `test_coverage` |
| REQ-LAB-007 | UDS SIDs 10/11/14/19/22/27/2E/31/3E | `uds.*` | `test_coverage` |
| REQ-LAB-008 | DTC raise/clear/read | `dtc.*` | `test_coverage` |
| REQ-LAB-009 | Fault state machine | `fault_mgr.*` | `test_coverage` |
| REQ-LAB-010 | ECU models BCM/VCU/BMS | `ecu_models.*` | products P07+ |
| REQ-LAB-011 | Prototype CAN IDs documented | `ae_can_ids.h`, DBC | DBC decode tests |
| REQ-LAB-012 | E2E CRC + alive counter | `crc_e2e.*` | `test_crc_e2e` |
| REQ-LAB-013 | DBC signal encode/decode | `dbc/aegw_c3_proto.dbc`, `linux/python` | `test_dbc_signals.py` |
| REQ-LAB-014 | Linux SocketCAN setup scripts | `linux/socketcan/` | Manual / CI Linux |
| REQ-LAB-014a | USB-CAN C HAL (CANable/PCAN/SLCAN) | `hal_can_usb.c`, `hal_can_adapter.h` | `test_can_adapter` |
| REQ-LAB-015 | python-can automation CLI | `linux/python/aegw_can/` | Unit without HW |
| REQ-LAB-016 | No secrets in git | `.gitignore` | Review |
| REQ-LAB-017 | Lab-only security testing | `products/usecases.c` UC-56..75, GUI IDS | `test_usecases` |
| REQ-LAB-018 | Host tests without silicon | CMake / run_all.ps1 | CI host |
| REQ-LAB-019 | Arduino ESP32-C3 path | `ports/arduino/AEGW_C3` | arduino-cli compile |
| REQ-LAB-020 | ESP-IDF TWAI driver | `platform/drivers/esp32` | **Open** |
| REQ-LAB-021 | OSAL portable threads | `osal/` | **Open** |
| REQ-LAB-022 | HIL closed-loop | `tests/hil`, UC-101..110 | Partial (host sim) |
| REQ-LAB-023 | Fault injection framework | `tests/fault_injection` | **Open** |
| REQ-LAB-024 | CAN IDS service | use-cases + GUI analyzer | Partial (lab) |
| REQ-LAB-025 | HVAC ECU model | future + DBC HVAC msgs | **Open** |
| REQ-LAB-026 | ≥100 auto/cyber use cases | `products/usecases.{h,c}` (120) | `test_usecases` |
| REQ-LAB-027 | UDS simulator GUI sim/live | `tools/gui` `/api/uds` | Manual GUI |
| REQ-LAB-028 | CAN analyzer GUI sim/live | `tools/gui` `/api/analyzer`, `can_analyzer.hpp` | `test_can_analyzer` |

Coverage rule: every **Shall** in `docs/pages/02-requirements.html` maps here as REQ-P-* / REQ-LAB-*.

---

**Embedded AI Design Labs Pvt Ltd · Muhammad Samiullah · © 2026**
