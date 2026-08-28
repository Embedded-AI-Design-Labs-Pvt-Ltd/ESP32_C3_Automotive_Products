# tools/can — USB-to-CAN host adapters

C/C++ HAL backend: `platform/drivers/posix/hal_can_usb.c`  
API: `platform/hal/hal_can.h` + `platform/hal/hal_can_adapter.h`  
C++: `ports/virtual_ecu/usb_can_adapter.hpp`

## Backends

| Adapter | `AE_CAN_BACKEND` | `AE_CAN_IFACE` example | Notes |
|---|---|---|---|
| Virtual (unit tests) | `sim` (default) | ignored | In-memory bus |
| CANable (candleLight) | `socketcan` or `canable` | `can0` | Linux `gs_usb` after `setup_can0.sh` |
| CANable (SLCAN) | `slcan` | `/dev/ttyACM0` or `COM3` | Serial SLCAN |
| PCAN-USB (Linux) | `pcan` or `socketcan` | `can0` | `peak_usb` → SocketCAN |
| PCAN-USB (Windows) | `pcan` | `PCAN_USBBUS1` | Build with `-DAE_HAVE_PCANBASIC` + PCANBasic SDK |

```powershell
# Linux HIL (example)
export AE_CAN_BACKEND=socketcan
export AE_CAN_IFACE=can0
export AE_CAN_BITRATE=500000
./build/host/test_usecases   # all 120 use cases on adapter

# Windows CANable SLCAN
$env:AE_CAN_BACKEND="slcan"
$env:AE_CAN_IFACE="COM3"
```

All products / 120 use cases call `hal_can_init_from_env()` / `hal_can_cfg_from_env()` on `AE_HOST`.

Lab / Virtual ECU only — Classical CAN — not road-vehicle certified.

---
**Embedded AI Design Labs Pvt Ltd** · Muhammad Samiullah · © 2026
