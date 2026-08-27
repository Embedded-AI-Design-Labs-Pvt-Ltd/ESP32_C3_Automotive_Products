# Module dependency graph

## 1. Allowed graph

```mermaid
flowchart TB
  subgraph apps["apps / products"]
    P[product_api]
  end

  subgraph core["core services"]
    DTC[dtc]
    FLT[fault_mgr]
    ECU[ecu_models]
    OTA[ota_agent]
    HLTH[health — future]
    SEC[security — future]
  end

  subgraph proto["protocols"]
    CAN[can_service]
    ISO[isotp]
    UDS[uds]
    INT[integrity — future]
  end

  subgraph mw["middleware — future / partial"]
    LOG[logger]
    BUS[message_bus]
    TEL[telemetry]
  end

  subgraph osal["osal"]
    OS[os_* API]
    POS[posix backend]
    FRT[freertos backend]
  end

  subgraph hal["hal"]
    HAPI[hal_* API]
    HP[posix sim]
    HE[esp32 drivers]
  end

  P --> DTC
  P --> FLT
  P --> ECU
  P --> OTA
  P --> UDS
  P --> CAN
  DTC --> UDS
  FLT --> DTC
  UDS --> ISO
  ISO --> CAN
  CAN --> INT
  INT --> HAPI
  CAN --> HAPI
  ECU --> CAN
  HLTH --> OS
  BUS --> OS
  LOG --> OS
  TEL --> HAPI
  OS --> POS
  OS --> FRT
  HAPI --> HP
  HAPI --> HE
```

Backends (`posix`, `freertos`, `esp32` drivers) are leaves. Nothing above OSAL/HAL may depend on a specific backend.

## 2. Current vs target include roots

| Today | Target |
|---|---|
| `platform/common` | stays |
| `platform/framework` | stays (ring, later SM helpers) |
| `platform/hal` | split API vs `drivers/posix` / `drivers/esp32` |
| `platform/protocols` | stays; add `crc` / integrity |
| `platform/services` | stays; add health, security |
| `platform/middleware` empty | OSAL lives in `osal/` or `platform/osal/` |
| `products/` | stays; optional `apps/` wrappers for CLI binaries |
| `ports/esp32_c3` | IDF project that **links** platform + osal/freertos + drivers/esp32 |
| `ports/virtual_ecu` | POSIX app that **links** platform + osal/posix + drivers/posix |

## 3. Forbidden edges

- `products/*` → `freertos/FreeRTOS.h`
- `products/*` → `esp_wifi.h` / `driver/twai.h`
- `uds.c` → `pthread.h`
- `can_service.c` → vehicle physics
- `hal_host.c` → UDS

## 4. Test dependency

Unit tests may include the module under test plus fakes for OSAL/HAL. They must not require an ESP32 board.

Integration tests on POSIX may start multiple `os_thread`s. HIL tests may use real TWAI.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
