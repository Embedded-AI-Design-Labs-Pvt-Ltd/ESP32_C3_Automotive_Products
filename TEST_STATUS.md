# TEST_STATUS

Last updated: 2026-08-28

## Suite summary

| Suite | Location | Status | Runner |
|---|---|---|---|
| Unit (host) | `tests/unit/` | **PASS** | `run_all.ps1` / CTest |
| Product use-cases P01–P17 | `tests/unit/test_all_products.c` | **PASS** | `run_all.ps1` |
| Coverage | `tests/unit/test_coverage.c` | **PASS** (100% fn / 98.20% lines) | llvm-cov |
| Arduino HAL host | `tests/unit/test_arduino_hal.cpp` | **PASS** | `run_all.ps1` |
| C++ ports | `tests/unit/test_cpp_ports.cpp` | **PASS** | `run_all.ps1` |
| Docs links | `tools/scripts/verify_docs.py` | **PASS** | `run_all.ps1` |
| Arduino MCU compile | `ports/arduino/AEGW_C3` | **PASS** | arduino-cli |
| CMake CTest | `build/cmake` | **PASS** 4/4 | ctest -C Debug |
| Integration | `tests/integration/` | Scaffold | — |
| HIL | `tests/hil/` | Scaffold | — |
| Fault injection | `tests/fault_injection/` | Scaffold | — |
| Security | `tests/security/` | Scaffold | — |
| Performance | `tests/performance/` | Scaffold | — |
| Regression | `tests/regression/` | Scaffold | — |
| Linux python-can | `linux/python/` | Scaffold | — |

## Test case template (new tests)

| Field | Content |
|---|---|
| ID | TEST-LAB-xxx |
| Requirement | REQ-LAB-xxx |
| Setup / Input / Expected / Timeout / Pass-fail / Cleanup / Logs | required |

## Gaps

- No SocketCAN / CANable / PCAN automated tests yet
- No E2E CRC / OSAL concurrency suites yet
- Security tests = lab traffic only

## Last full run

2026-08-28 — `tools/scripts/run_all.ps1` exit 0 (~68 s).
