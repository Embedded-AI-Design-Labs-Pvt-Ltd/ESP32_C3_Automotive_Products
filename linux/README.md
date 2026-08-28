# linux/ — SocketCAN · CANable · PCAN · python-can lab tools

Target stack:

```text
CANable / PCAN-USB → SocketCAN (can0) → can-utils / python-can / Wireshark
```

| Path | Purpose | Status |
|---|---|---|
| `socketcan/` | bring-up / teardown scripts | Scaffold |
| `can_tools/` | candump/cansend wrappers | Scaffold |
| `python/` | python-can automation + pytest | Scaffold |
| `dbc/` | DBC load helpers | Scaffold |
| `uds/` | UDS client over ISO-TP | Scaffold |
| `isotp/` | ISO-TP helpers | Scaffold |

**Rules**

- Lab / virtual ECU / HIL only. Never attach to a road vehicle.
- PCAN and CANable drivers differ — document separately.
- Windows primary host uses Virtual ECU GUI; Linux scripts target Linux CI/VM.

---

**Embedded AI Design Labs Pvt Ltd** · Muhammad Samiullah · © 2026
