# Directory mapping — playbook tree ↔ live code

Do **not** big-bang rename. Live builds depend on `platform/`, `products/`, `ports/`.

| Playbook path | Live path | Notes |
|---|---|---|
| `firmware/hal` | `platform/hal` + `platform/drivers/esp32` | Host: `hal_host.c` |
| `firmware/drivers` | `platform/drivers/{posix,esp32}` | ESP-IDF TWAI not yet |
| `firmware/can` | `platform/protocols/can_service.*` + `crc_e2e.*` | Classical CAN only |
| `firmware/diagnostics` | `platform/protocols/{isotp,uds}.*` + `services/dtc.*` | |
| `firmware/services` | `platform/services/*` | |
| `firmware/application` | `products/` + `ports/esp32_c3/main.c` | |
| `firmware/tasks` | FreeRTOS after `osal/freertos` | Catalog in runtime_architecture.md |
| `firmware/safety` | `docs/safety` + fault_mgr | Prototype only |
| `firmware/security` | products P14 + future IDS service | Lab only |
| `firmware/health` | future `platform/services/health_*` | |
| `firmware/bootloader` | `ota_agent` + future IDF partitions | |
| `firmware/config` | `configs/` + compile flags | |
| `linux/socketcan` | `linux/socketcan/` | Scripts |
| `linux/python` | `linux/python/` | python-can automation |
| `linux/dbc` | symlink conceptually to `dbc/` | Single DBC root |
| `linux/uds` / `isotp` | Python clients calling same SIDs | Do not reimplement C stack |
| `tests/*` | `tests/unit` live; others grow by phase | |
| `docs/*` | `docs/` HTML + markdown | |
| `tools/*` | `tools/gui`, `tools/scripts` | |
| `ci/` | `ci/` + `.github/workflows` when added | |
| `dbc/` | `dbc/aegw_c3_proto.dbc` | Source of truth |

---

**Embedded AI Design Labs Pvt Ltd · © 2026**
