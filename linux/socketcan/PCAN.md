# PCAN-USB vs CANable

| Topic | CANable (candleLight) | PCAN-USB |
|---|---|---|
| Kernel | Often `gs_usb` / native SocketCAN | `peak_usb` |
| Interface name | Usually `can0` | `can0` / `can1` |
| Userspace | can-utils | can-utils + optional PCANBasic |
| FD | Adapter-dependent; **lab MCU is Classical only** | Same — use Classical bitrate |

Do not assume identical udev rules or firmware. Always pass `IFACE` into scripts.

After `ip link` is up, tooling (`candump`, `linux/python`) is the same SocketCAN API.

## C/C++ host HAL

Use the portable HAL — products never include PCANBasic / `linux/can.h`:

```bash
export AE_CAN_BACKEND=socketcan   # or: canable | pcan | slcan | sim
export AE_CAN_IFACE=can0          # PCAN-USB Windows: PCAN_USBBUS1 ; SLCAN: /dev/ttyACM0 or COM3
export AE_CAN_BITRATE=500000
# then run test_usecases / products — all AE_HOST paths call hal_can_init_from_env()
```

Implementation: `platform/drivers/posix/hal_can_usb.c`  
Docs: `tools/can/README.md`
