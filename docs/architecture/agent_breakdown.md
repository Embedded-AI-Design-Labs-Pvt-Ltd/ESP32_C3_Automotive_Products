# Agent / task breakdown

Cursor agents live in `.cursor/agents/`. Project rules live in `.cursor/rules/*.mdc`.

Use **one agent per phase**. Agent `code_reviewer` files findings before the next phase.

## 1. Roster (playbook mapped to this repo)

| Agent file | First phase | Depends on |
|---|---|---|
| `system_architect.md` | A0 (done in docs) | — |
| `repository_architect.md` | A0/A1 directories + CMake | system |
| `cpp_engineer.md` | Core harden (types, SM, queues) | A0 |
| `osal_engineer.md` | **A2 next** | cpp |
| `posix_virtual_hw.md` | A3 | osal |
| `esp32c3_platform.md` | A4–A5 | osal |
| `freertos_engineer.md` | A4 task set | osal |
| `can_engineer.md` | CAN HAL/service | posix hw or esp32 |
| `can_integrity.md` | A6 | can |
| `uds_engineer.md` | A7 | isotp (existing) |
| `virtual_ecu.md` | ECU SM | can, osal |
| `vehicle_simulator.md` | A9 | virtual ecu |
| `health_monitor_engineer.md` | A8 | osal |
| `diagnostics_engineer.md` | DTC map | uds, fault |
| `safety_engineer.md` | A10 docs | fault |
| `cybersecurity_engineer.md` | A11 | can, uds |
| `bootloader_engineer.md` | OTA POSIX flow | core |
| `cloud_engineer.md` | A13 last among comms | network HAL |
| `ble_engineer.md` | A12 after CAN/UDS | osal, hal |
| `driver_engineer.md` | peripherals | hal |
| `hil_engineer.md` | HIL | can, gpio |
| `test_engineer.md` | every phase | module owner |
| `posix_build.md` / CMake | A1 | — |
| `esp_idf_build.md` | A5 | esp32 platform |
| `cicd_engineer.md` | A14 | posix tests green |
| `code_reviewer.md` | end of each phase | implementer |

Legacy numbered files (`01-system-architect.md` … `20-review.md`) remain as short reminders; **prefer the named files above**.

## 2. First implementation slice (after this architecture commit)

1. OSAL POSIX + unit tests  
2. Move host CAN/GPIO behind `drivers/posix` without changing product APIs  
3. Review (no pthread in `products/` or `platform/protocols/`)  
4. Only then ESP32-C3 drivers  

Do **not** start with BLE.

## 3. Review protocol

Reviewer does not implement first. File CRITICAL / HIGH / MEDIUM / LOW in `docs/review/` when that phase runs. Fixes are a second pass.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
