# firmware/

ESP-IDF packaging view of the AEGW-C3 Classical CAN lab.

**Do not duplicate** portable stacks here. Live code:

| Area | Path |
|---|---|
| HAL / protocols / services | `platform/` |
| Products | `products/` |
| ESP32-C3 entry | `ports/esp32_c3/` (IDF project TBD) |
| Arduino path | `ports/arduino/AEGW_C3/` |

Subfolders (`can`, `diagnostics`, `tasks`, …) document the target layout. See `docs/architecture/directory_mapping.md` and `docs/architecture/can_lab_orchestrator.md`.

Classical CAN only. External transceiver required. Lab/HIL only.

---

**Embedded AI Design Labs Pvt Ltd · Muhammad Samiullah · © 2026**
