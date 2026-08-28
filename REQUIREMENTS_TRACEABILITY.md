# REQUIREMENTS_TRACEABILITY

Last updated: 2026-08-27  
IDs are **lab/prototype** (REQ-LAB-*). Not ASIL-allocated. Not OEM production requirements.

| Req ID | Statement | Implementation | Tests | Status |
|---|---|---|---|---|
| REQ-LAB-001 | One core platform; products are thin compositions | `platform/`, `products/` | `test_all_products` | Done |
| REQ-LAB-002 | Portable C; no IDF/pthread in app/protocols | Portability rule + existing sources | Review / grep | Partial (no OSAL yet) |
| REQ-LAB-003 | Classical CAN only on ESP32-C3 (no CAN-FD) | Policy + TWAI path | Docs | Done (policy) |
| REQ-LAB-004 | External CAN transceiver required | Hardware docs Agent 01 | HW procedure | W1 |
| REQ-LAB-005 | Configurable bitrate 125/250/500/1000 | `hal_can` / TWAI cfg | Unit + HIL | Partial (host fixed sim) |
| REQ-LAB-006 | CAN filter / TX / RX / stats | `can_service` | `test_coverage` | Partial |
| REQ-LAB-007 | ISO-TP SF/FF/CF/FC | `isotp.*` | coverage | Partial |
| REQ-LAB-008 | UDS SIDs 0x10/11/14/19/22/27/2E/31/3E | `uds.*` | products P03 | Partial |
| REQ-LAB-009 | Extended UDS 0x23/28/34–37 | — | — | Not started |
| REQ-LAB-010 | DTC create/confirm/clear | `dtc.*` | coverage | Partial |
| REQ-LAB-011 | Freeze frame / aging | — | — | Not started |
| REQ-LAB-012 | Fault manager NORMAL→SAFE | `fault_mgr.*` | coverage | Partial |
| REQ-LAB-013 | Health monitor CPU/stack/CAN | — | — | Not started |
| REQ-LAB-014 | Watchdog + reset reason | HAL WDT stub | — | Partial |
| REQ-LAB-015 | CRC / alive / E2E layer | — | — | Not started |
| REQ-LAB-016 | DBC encode/decode | — | — | Not started |
| REQ-LAB-017 | Virtual PT / VCU / BMS / BCM / HVAC | `ecu_models` (BCM/VCU/BMS) | products | Partial |
| REQ-LAB-018 | SocketCAN Linux bring-up | `linux/socketcan` | scripts | Not started |
| REQ-LAB-019 | CANable + PCAN documented | `linux/` docs | — | Not started |
| REQ-LAB-020 | python-can automation | `linux/python` | pytest | Not started |
| REQ-LAB-021 | HIL closed-loop | `tests/hil` | HIL suite | Not started |
| REQ-LAB-022 | Fault injection lab-only | `tests/fault_injection` | — | Not started |
| REQ-LAB-023 | CAN IDS / spoof-flood detect | security module | lab traffic | Not started |
| REQ-LAB-024 | SecurityAccess demo algorithm | — | — | Not started |
| REQ-LAB-025 | OTA/boot validate before commit | `ota_agent` | coverage | Partial |
| REQ-LAB-026 | CI POSIX build+test | `ci/` | Actions | Not started |
| REQ-LAB-027 | Never attach to road vehicle | Docs / SECURITY.md | — | Policy Done |
| REQ-LAB-028 | Requirements↔tests traceability | This file + TEST_STATUS | — | Ongoing |

## Common APIs (do not redefine)

| API | Header | Owner |
|---|---|---|
| `ae_status_t`, `ae_can_frame_t` | `platform/common/ae_types.h` | Orchestrator / cpp |
| CAN IDs (prototype) | `platform/common/ae_can_ids.h` | Automotive architect |
| `hal_can_*` | `platform/hal/hal_can.h` | HAL |
| `can_svc_*` | `platform/protocols/can_service.h` | CAN service |
| `isotp_*` | `platform/protocols/isotp.h` | ISO-TP |
| `uds_*` | `platform/protocols/uds.h` | UDS |
| `dtc_*` / `fault_*` | `platform/services/` | Diagnostics / fault |

New modules must extend these headers or add **new** files — never fork parallel frame types.
