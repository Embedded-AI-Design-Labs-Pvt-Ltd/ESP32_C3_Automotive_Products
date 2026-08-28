# BUILD_STATUS

Last updated: 2026-08-28 · Agent 00 orchestrator

## Targets

| Target | Command | Status |
|---|---|---|
| Host unit + products | `tools/scripts/run_all.ps1` | PASS (2026-08-28) |
| CMake/CTest | `cmake` + `ctest -C Debug` (5 tests) | PASS |
| CRC/E2E | `test_crc_e2e` | PASS |
| DBC Python signals | `linux/python/tests/test_dbc_signals.py` | PASS |
| Arduino ESP32-C3 compile | `arduino-cli` esp32c3 | PASS (~325 KB) |
| ESP-IDF flash TWAI | `eim run "idf.py build" v5.5.1` in `ports/esp32_c3` | **NOT READY** (no IDF project) |
| Linux SocketCAN HIL | `linux/socketcan/setup_can0.sh` | Scripts present; needs Linux + adapter |
| Virtual ECU GUI | `python tools/gui/server.py` | PASS on :8765 |

## Toolchain (developer PC)

| Tool | Status |
|---|---|
| Python 3.12 / 3.14 | Installed |
| LLVM MinGW (gcc/clang) | Installed |
| CMake | Installed |
| Arduino CLI + esp32 3.3.11 | Installed |
| ESP-IDF v5.5.1 via eim | Installed (selected) |
| ESP USB serial drivers | Installed via `eim install-drivers` |
| Cursor: clangd, CMake, Python, ESP-IDF ext | Installed |

## Module build map

| Module | Compiles in host | Notes |
|---|---|---|
| `platform/*` protocols/services | Yes | Via `ae_platform` |
| `crc_e2e` | Yes (this phase) | Unit tested |
| `osal` | No | README only — next |
| `platform/drivers/esp32` | No | TWAI pending |
| `linux/python` | Syntax OK | Needs `python-can` on Linux |

## Known blockers

1. No real ESP32-C3 USB COM in last session (Intel AMT COM3 is not the MCU).
2. SocketCAN requires Linux (or WSL2 with USB passthrough).
3. Do not claim CAN-FD on ESP32-C3.

---

**Embedded AI Design Labs Pvt Ltd · Muhammad Samiullah · © 2026**
