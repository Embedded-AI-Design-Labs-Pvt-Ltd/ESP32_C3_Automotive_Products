# TEST_STATUS

Last updated: 2026-08-28

## Suites

| Suite | Location | Status |
|---|---|---|
| Unit (host C) | `tests/unit/test_host.c` | PASS |
| Products P01–P17 | `tests/unit/test_all_products.c` | PASS |
| Module coverage | `tests/unit/test_coverage.c` | PASS (~98% lines) |
| C++ ports | `tests/unit/test_cpp_ports.cpp` | PASS |
| Arduino HAL host | `tests/unit/test_arduino_hal.cpp` | PASS |
| CRC/E2E | `tests/unit/test_crc_e2e.c` | Added this phase |
| DBC Python | `linux/python/tests/test_dbc_signals.py` | Added this phase |
| Integration | `tests/integration/` | Scaffold |
| HIL | `tests/hil/` | Scaffold |
| Fault injection | `tests/fault_injection/` | Scaffold |
| Security | `tests/security/` | Scaffold |
| Performance | `tests/performance/` | Scaffold |
| Regression | `tests/regression/` | Scaffold |

## Case template (mandatory)

`ID | requirement | setup | input | expected | timeout | pass/fail | logs`

## Environment

- Host Windows: GCC/Clang + CMake — primary CI gate.
- Linux + SocketCAN: required for HIL/adapter tests; not blocking host merges.
- ESP32-C3 board: optional; never use Intel AMT SOL as flash port.

---

**Embedded AI Design Labs Pvt Ltd · Muhammad Samiullah · © 2026**
