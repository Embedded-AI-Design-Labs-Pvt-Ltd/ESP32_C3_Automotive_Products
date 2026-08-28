# PCAN-USB vs CANable

| Topic | CANable (candleLight) | PCAN-USB |
|---|---|---|
| Kernel | Often `gs_usb` / native SocketCAN | `peak_usb` |
| Interface name | Usually `can0` | `can0` / `can1` |
| Userspace | can-utils | can-utils + optional PCANBasic |
| FD | Adapter-dependent; **lab MCU is Classical only** | Same — use Classical bitrate |

Do not assume identical udev rules or firmware. Always pass `IFACE` into scripts.

After `ip link` is up, tooling (`candump`, `linux/python`) is the same SocketCAN API.
