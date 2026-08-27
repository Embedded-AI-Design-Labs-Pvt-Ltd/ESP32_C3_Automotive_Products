# Development phases

Work **one phase at a time**. Each phase: interface → implementation → tests → review. Do not open BLE and cloud in the same change as OSAL.

The HTML portal’s old “Phase 3 not started” table is obsolete for host C. This table is the live plan.

| Phase | Name | Exit criteria | State |
|---|---|---|---|
| A0 | Architecture (this package) | Architecture markdown + Cursor rules/agents + honest gap | **This change** |
| A1 | POSIX CMake | `cmake --build` runs existing host tests | Started (root CMake) |
| A2 | OSAL | `os_*` API + POSIX backend + unit tests (thread, mutex, queue, timer, timeout) | Next |
| A3 | HAL split | `hal_host.c` moved behind POSIX driver; fault-inject hooks | Not started |
| A4 | FreeRTOS OSAL | Headers compile; IDF stub project links OSAL | Not started |
| A5 | ESP32-C3 HAL | TWAI, UART, GPIO, WDT, NVS init with fail-closed | Not started |
| A6 | CAN integrity | CRC/checksum/alive/timeout/range/plausibility → fault_mgr | Not started |
| A7 | ISO-TP / UDS harden | Loss, bad FC, P2, buffer cap tests | Partial (code exists) |
| A8 | Health + fault | Subsystem HEALTHY/WARNING/DEGRADED/FAILED; DTC map | Partial (fault_mgr/DTC) |
| A9 | Vehicle simulator app | Deterministic model, CLI, 0.1x/1x/10x | Not started |
| A10 | Safety docs | FMEA + reactions; no certification claim | Not started |
| A11 | Security prototype | Anomaly IDs, rate limit, documented limits | Not started |
| A12 | BLE | NimBLE behind `hal_ble`; after CAN/UDS | Not started |
| A13 | Cloud | MQTT/TLS gateway, gitignored creds, lab dashboard | Not started |
| A14 | CI | Format, build, unit, warnings | Not started |
| A15 | Products | Thin apps on the same core; no duplicated stacks | Ongoing |

## Phase A2 detail (next implementation)

Implement only:

- `osal/os_*.h` contracts  
- `osal/posix/`  
- `tests/unit` for OSAL  
- No BLE, no MQTT, no IDF drivers  

FreeRTOS backend may be headers + `#ifdef` compile test on host using a stub, or deferred to A4 if a kernel is not available on Windows.

## What “done” means for a module

- Interface header with ownership and thread-safety comments  
- Implementation (or an explicit POSIX mock that models hardware behavior)  
- Error handling (`ae_status_t`)  
- Unit tests with ID, setup, stimulus, expected, timeout, pass/fail  
- Short README in the module folder  

Do not add a `.c` file that only returns `AE_OK` to make a linker happy.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
