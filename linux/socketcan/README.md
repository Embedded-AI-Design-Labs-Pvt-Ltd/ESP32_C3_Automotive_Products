# SocketCAN / CANable / PCAN (Linux)

ESP32-C3 lab uses **Classical CAN only** (no CAN-FD).

## Quick start (CANable candleLight → can0)

```bash
sudo linux/socketcan/setup_can0.sh can0 500000
candump can0
cansend can0 200#0100000000000010
```

## PCAN-USB

```bash
# After peak_usb module loads can0/can1:
sudo linux/socketcan/setup_can0.sh can0 500000
```

See [PCAN.md](PCAN.md) for driver differences.

## Wireshark

Capture on `can0` with SocketCAN link type. Filter example: `can.id == 0x200`.

## Safety

Bench / Virtual ECU only. Never attach experimental tooling to a road vehicle.
