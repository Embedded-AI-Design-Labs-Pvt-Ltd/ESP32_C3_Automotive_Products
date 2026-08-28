# POSIX drivers

| File | Role |
|---|---|
| `hal_can_usb.c` | Host Classical CAN: **sim** + **SocketCAN** (CANable/PCAN) + **SLCAN** + optional **PCAN-Basic** |

Products and services include only `hal_can.h`. Adapter selection is via `hal_can_adapter.h` / env vars on `AE_HOST` builds.

See `tools/can/README.md` and `linux/socketcan/`.

---

**Embedded AI Design Labs Pvt Ltd** · Muhammad Samiullah · © 2026
