# Host tools

| Tool | Role |
|---|---|
| `gui/` | Web GUI at http://127.0.0.1:8765/ — Virtual ECU, optional hardware mode |
| `virtual_ecu/` | Simulated BCM / VCU / BMS / IC / tester without ESP32 hardware |
| `scripts/` | Flash, log parse, DBC export, regression runners |

GUI and Virtual ECU are C++ (or later a web UI). They consume the same CAN database and UDS DID tables as firmware.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.

