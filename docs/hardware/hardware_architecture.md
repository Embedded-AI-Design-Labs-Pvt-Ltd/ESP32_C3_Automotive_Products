# Hardware architecture — ESP32-C3 Classical CAN lab

**Agent 01 — Hardware Architect**  
Prototype / education bench. Not a production vehicle ECU. Not ISO 26262 certified.

## 1. Block diagram

```text
                 5V/3V3 lab PSU (+ reverse polarity diode / polyfuse)
                              |
                    +---------v---------+
                    |     ESP32-C3      |
                    |  UART0 USB-CDC    |---- USB ---- Host PC (flash/monitor)
                    |  GPIO TWAI_TX     |----|
                    |  GPIO TWAI_RX     |----|     +------------------+
                    +-------------------+    +---->| CAN transceiver  |
                                                 | SN65HVD230 /     |
                                                 | TJA1051          |
                                                 +---+----------+---+
                                                     |          |
                                                   CANH       CANL
                                                     |          |
                              120 Ω ------+----------+----------+------ 120 Ω
                              (node A)    |   twisted pair      |       (node B)
                                          |                     |
                                    CANable/PCAN-USB      other ECU / terminator
                                          |
                                     Linux SocketCAN (can0)
```

**Critical:** ESP32-C3 has a **TWAI Classical CAN controller only**. It does **not** include a CAN physical-layer transceiver. An external transceiver (e.g. SN65HVD230 3.3 V or TJA1051) is **required** for CANH/CANL.

ESP32-C3 does **not** support CAN-FD. Do not configure FD bitrates or >8-byte DLC on this MCU.

## 2. Recommended GPIO map (lab default)

| Function | ESP32-C3 GPIO | Notes |
|---|---|---|
| TWAI TX | GPIO 5 | To transceiver TXD |
| TWAI RX | GPIO 4 | From transceiver RXD |
| Transceiver EN / STB | GPIO 6 | Optional; hold out of standby |
| Status LED | GPIO 8 | Active-low on many C3 boards |
| Debug UART TX/RX | USB-CDC | Arduino: CDC on boot |
| I2C SDA/SCL | GPIO 8/9 | Optional sensors (conflicts — board-specific) |

Pin map is **board-specific**. Override in `configs/boards/*.json` / ESP-IDF `sdkconfig`. Document the actual DevKit silk when flashing.

## 3. CAN bus assumptions

| Parameter | Lab default |
|---|---|
| Bitrate | 500 kbit/s (also 125/250/1000) |
| Termination | 120 Ω at **each end** of the stub/bus |
| Stub length | Keep short (<0.3 m preferred for breadboard) |
| Ground | Common GND between ESP32, transceiver, adapter |
| Identifier set | Prototype IDs in `ae_can_ids.h` / `dbc/aegw_c3_proto.dbc` |

## 4. Power and protection (concept)

- Lab supply: regulated 5 V → 3V3 for ESP32; transceiver per datasheet (3V3 for SN65HVD230).
- Reverse polarity: series Schottky or ideal-diode on board input.
- ESD: TVS on CANH/CANL to GND (e.g. PESD1CAN class device) — select for automotive where needed.
- Transient: common-mode choke + split termination optional for noisy benches.
- Do **not** connect this breadboard to a production vehicle harness.

## 5. Programming / test points

| Point | Purpose |
|---|---|
| USB-C / UART | Flash + monitor (`idf.py` / Arduino) |
| TP_CANH / TP_CANL | Scope / logic |
| TP_TXD / TP_RXD | Between MCU and transceiver |
| GND | Star ground to PSU |

## 6. Hardware test procedure

1. Continuity: TX→TXD, RX→RXD, GND common, 120 Ω end-to-end.
2. Power rail: 3V3 stable, transceiver Vcc correct.
3. Loopback (if enabled in driver): MCU TX seen on RX without bus.
4. Adapter: `ip link set can0 up type can bitrate 500000` then `candump can0`.
5. ECU TX heartbeat (VCU 0x200) visible on `candump`.
6. Bus-off recovery: short CANH-CANL briefly; confirm recovery policy (lab only).

## 7. Adapter notes

| Adapter | Typical Linux path |
|---|---|
| CANable (candleLight) | Native SocketCAN `can0` |
| CANable (SLCAN) | `slcand` → `can0` |
| PCAN-USB | `peak_usb` → `can0` (different driver; see `linux/socketcan/PCAN.md`) |

Drivers are **not** identical. Scripts take `IFACE=can0` as a parameter.

---

**Embedded AI Design Labs Pvt Ltd · Muhammad Samiullah · CTO & Founder · © 2026**
