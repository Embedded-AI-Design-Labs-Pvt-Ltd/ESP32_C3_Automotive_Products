# Directory mapping — lab layout ↔ live code

Do **not** big-bang rename. New folders hold README pointers until a module is ready to move.

## firmware/

| Path | Maps to / notes |
|---|---|
| `firmware/README.md` | ESP-IDF target overview |
| `firmware/bootloader/` | Future; today `platform/services/ota_agent.*` |
| `firmware/drivers/` | → `platform/drivers/esp32/` (TWAI TBD) |
| `firmware/hal/` | → `platform/hal/` |
| `firmware/services/` | → `platform/services/` |
| `firmware/application/` | → `products/` |
| `firmware/diagnostics/` | → `isotp` + `uds` + `dtc` |
| `firmware/can/` | → `can_service` + TWAI driver |
| `firmware/safety/` | → `fault_mgr` + docs/safety |
| `firmware/security/` | IDS / SecurityAccess (not started) |
| `firmware/health/` | Health monitor (not started) |
| `firmware/storage/` | NVS/config (HAL stubs today) |
| `firmware/communication/` | BLE/network later |
| `firmware/tasks/` | FreeRTOS catalog (docs today) |
| `firmware/config/` | Board/Kconfig → also `config/` |

## linux/

| Path | Status |
|---|---|
| `linux/socketcan/` | Scripts for can0 bring-up |
| `linux/can_tools/` | can-utils wrappers |
| `linux/python/` | python-can automation |
| `linux/dbc/` | DBC load helpers |
| `linux/uds/` | UDS client over ISO-TP |
| `linux/isotp/` | Linux ISO-TP helpers |

## Other

| Path | Status |
|---|---|
| `tests/unit` | **Exists** — keep extending |
| `tests/{integration,hil,fault_injection,security,performance,regression}` | Scaffold |
| `docs/{architecture,requirements,design,test,safety,security}` | Extend |
| `tools/{can,diagnostics,dbc,logging,automation}` | Scaffold; GUI stays `tools/gui` |
| `config/`, `dbc/`, `scripts/`, `ci/` | Scaffold |

---

**Embedded AI Design Labs Pvt Ltd** · Muhammad Samiullah · © 2026
