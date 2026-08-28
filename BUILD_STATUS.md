# BUILD_STATUS

Last updated: 2026-08-28 (Agent 00 — W0 after `run_all.ps1`)

## Targets

| Target | Toolchain | Status |
|---|---|---|
| Host unit / products | GCC/Clang + `run_all.ps1` / CMake | **PASS** |
| Arduino ESP32-C3 sketch | arduino-cli `esp32:esp32:esp32c3` | **PASS** compile (~325 KB) |
| ESP-IDF firmware | eim ESP-IDF v5.5.1 | Installed; project CMake not wired yet |
| Linux SocketCAN | can-utils / python-can | Scaffold only |
| CI | GitHub Actions | Not started |

## Last `run_all.ps1` (2026-08-28)

| Step | Result |
|---|---|
| Docs verify | PASS (15 HTML) |
| Host unit | PASS |
| 17 product use-cases | PASS |
| Module coverage | PASS (100% functions, 98.20% lines) |
| Arduino HAL host | PASS |
| C++ ports | PASS |
| CMake CTest (4) | PASS |
| Arduino CLI ESP32-C3 | PASS compile |

## Wave progress

| Wave | Status |
|---|---|
| W0 Orchestrator | **Complete** (status files + lab scaffold + mapping) |
| W1 Hardware docs | Next |
| W2 OSAL / HAL | Next after W1 |

See `docs/architecture/can_lab_orchestrator.md`.
