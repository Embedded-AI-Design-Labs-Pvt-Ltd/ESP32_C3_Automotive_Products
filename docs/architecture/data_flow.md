# Data flow

## 1. CAN frame path (hot path)

```mermaid
flowchart LR
  PHY["Transceiver or virtual bus"]
  ISR["ISR / host inject"]
  Q["OS queue"]
  RX["CAN RX task"]
  FILT["Filter + integrity"]
  SVC["can_service"]
  APP["Subscribers: gateway, logger, ISO-TP, IDS"]

  PHY --> ISR --> Q --> RX --> FILT --> SVC --> APP
```

Copy `ae_can_frame_t` by value. No malloc. Integrity failures go to Fault Manager, not silently dropped without a counter.

## 2. Diagnostic path

```mermaid
flowchart TB
  CAN["CAN 0x7E0 / 0x7DF SIM"]
  ISO["ISO-TP reassembly"]
  UDS["UDS server"]
  DID["DID / routine callbacks"]
  DTC["DTC table"]
  TX["ISO-TP TX → 0x7E8"]

  CAN --> ISO --> UDS
  UDS --> DID
  UDS --> DTC
  UDS --> TX
```

BLE and UART (later) are **transports** into the same UDS/DTC tables. They must not own a second diagnostic database.

## 3. Vehicle signal path

```mermaid
flowchart LR
  IN["Accel / brake / gear / faults"]
  SIM["Vehicle simulator"]
  ECU["BCM VCU BMS models"]
  CAN["Periodic CAN TX"]
  IC["Consumers: IC, gateway, cloud"]

  IN --> SIM --> ECU --> CAN --> IC
```

Do not put vehicle physics inside `hal_can` or the TWAI driver.

## 4. Telemetry path (future cloud)

```mermaid
flowchart TB
  H["Health + fault + DTC + CAN stats"]
  T["Telemetry service — bounded queue"]
  N["hal_network MQTT/TLS"]
  G["Cloud gateway"]
  D["Database / dashboard"]

  H --> T --> N --> G --> D
```

Payload (prototype): vehicle ID, ECU ID, timestamp, vehicle state, CAN statistics, health, fault, DTC, battery, temperature, software version, security event IDs.

If the MQTT link is down, drop newest or oldest per policy and increment a counter. Never block CAN RX on cloud.

## 5. Configuration and identity

| Data | Owner | Persistence |
|---|---|---|
| Prototype CAN IDs | `ae_can_ids.h` | Compile-time |
| VIN / SW / HW DIDs | product DID table | ROM + optional NVS |
| Bitrate, BLE name, routes | config store | NVS / file |
| Cloud URL / certs | environment | Not in source |

## 6. Logging

Logger is a **sink**. Control decisions must not wait on UART. ISR-safe ring; task drains to UART/BLE/file.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
