# Hardware architecture — ESP32-C3 Classical CAN lab (Agent 01)

**Status:** Baseline draft (Wave 1)  
**Classical CAN only.** ESP32-C3 TWAI is **not** CAN-FD.

## Block diagram

```text
+------------------+     TX/RX      +------------------+      CANH/CANL
| ESP32-C3         |-------------->| CAN transceiver  |------------------+
| TWAI controller  |<--------------| SN65HVD230 /     |                  |
| USB-SERIAL/JTAG  |               | TJA1051          |     120 Ω*       |
| Debug UART       |               +------------------+                  |
+------------------+                      |                              |
         |                                | 3.3 V / 5 V as required      |
         | USB                            |                              |
         v                                v                              v
   Host PC (flash)                   Lab PSU                    CANable / PCAN
                                                                    |
                                                              SocketCAN can0
```

\* Termination: 120 Ω at each end of the bus (or one 60 Ω equivalent when only two nodes and manufacturer guidance allows).

## Why an external transceiver is required

ESP32-C3 TWAI pins are **logic-level** TX/RX. They do **not** drive CANH/CANL differential. A transceiver (SN65HVD230, TJA1051, etc.) is mandatory for any physical bus.

## Prototype GPIO map (lab default — confirm on your board)

| Function | GPIO (prototype) | Notes |
|---|---|---|
| TWAI TX | GPIO5 | Matches Arduino sketch path |
| TWAI RX | GPIO4 | Matches Arduino sketch path |
| UART0 TX/RX | USB-SERIAL/JTAG | Console 115200 |
| Status LED | Board-dependent | Optional |
| Transceiver STB/EN | Optional GPIO | Keep in normal mode for tests |

Document any board-specific remap in `config/` / `firmware/config/` before flashing.

## Interfaces

| Interface | Spec |
|---|---|
| CAN | Classical CAN 2.0A/B, bitrates 125/250/500/1000 kbit/s |
| Debug | UART USB CDC |
| Programming | USB / esptool / arduino-cli |
| Host PC adapter | CANable (candleLight/SLCAN) or PCAN-USB |

## Protection concepts (lab, not vehicle production)

- ESD: TVS on CANH/CANL where possible
- Reverse polarity: fuse + diode on lab PSU input
- Automotive transients: common-mode choke / TVS for bench robustness only — **not** an ISO 7637 qualification

## Hardware test procedure (bench)

1. Continuity: TWAI TX→transceiver TXD, RX→RXD, GND common.
2. Power: transceiver VCC within device limits; measure CANH/CANL idle (~2.5 V recessive typical).
3. Termination present as designed.
4. Loopback: ESP32 listen/loopback if supported, then two-node with CANable.
5. Bitrate match on both ends (start 500 kbit/s).
6. Flash only a real ESP32-C3 USB port (not Intel AMT SOL).

## Assumptions

- Single TWAI controller (one physical Classical CAN bus).
- Multi-bus gateway is architectural only until a multi-CAN MCU is used.
- Lab / HIL only — never a road vehicle.

---

**Embedded AI Design Labs Pvt Ltd** · Muhammad Samiullah · © 2026  
Not ISO 26262 certified.
