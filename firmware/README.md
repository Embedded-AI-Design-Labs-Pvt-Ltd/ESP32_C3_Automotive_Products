# firmware/ — ESP32-C3 Classical CAN ECU (ESP-IDF target)

**Classical CAN / TWAI only. No CAN-FD on ESP32-C3.**

Live portable sources today: `../platform/` and `../products/`.  
ESP-IDF project entry (planned): `../ports/esp32_c3/`.  
Arduino sketch (working): `../ports/arduino/AEGW_C3/`.

External CAN transceiver (SN65HVD230 / TJA1051) is **required** between TWAI pins and CANH/CANL.

| Subfolder | Role | Live code |
|---|---|---|
| `hal/` | Hardware abstraction | `platform/hal/` |
| `drivers/` | ESP-IDF TWAI, UART, … | `platform/drivers/esp32/` |
| `can/` | CAN service + driver | `platform/protocols/can_service.*` |
| `diagnostics/` | ISO-TP, UDS, DTC | `platform/protocols/`, `platform/services/dtc.*` |
| `services/` | Fault, OTA, ECU models | `platform/services/` |
| `application/` | Product compositions | `products/` |
| `tasks/` | FreeRTOS task set | docs + future OSAL |
| `bootloader/` | OTA slots | `ota_agent` prototype |
| `safety/` / `security/` / `health/` | Lab mechanisms | Partial / planned |

See `docs/architecture/directory_mapping.md` and `docs/architecture/can_lab_orchestrator.md`.

---

**Embedded AI Design Labs Pvt Ltd** · Muhammad Samiullah · © 2026
