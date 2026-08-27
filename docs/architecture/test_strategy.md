# Test strategy

## 1. Levels

| Level | Environment | Examples |
|---|---|---|
| Unit | Host, no threads required unless testing OSAL | CRC, ring, UDS SID, DTC bit, OSAL mutex |
| Integration | POSIX threads or cooperative ticks | CAN + ISO-TP + UDS; health + fault |
| System | Virtual ECU + vehicle sim + tester | Drive cycle, session, DTC read |
| Fault injection | POSIX virtual hardware | Drop, corrupt, timeout, stuck sensor |
| HIL | ESP32-C3 + bus | TWAI loopback, GPIO, UART |

Every automated case records:

`ID | setup | stimulus | expected | timeout | pass/fail | cleanup`

## 2. What already runs

`tools/scripts/run_all.ps1`:

- docs link/footer check  
- `tests/unit/test_host.c`  
- all 17 product use-cases  
- coverage-oriented module tests  
- Arduino HAL host tests  
- C++ port tests  

These must stay green while OSAL is added.

## 3. OSAL tests (phase A2)

- Thread create/join  
- Mutex exclusion  
- Semaphore count  
- Queue send/recv/timeout  
- Timer fire  
- Event set/wait  
- Timeout expiry  
- Documented race tests (not “sleep and hope”)  
- Deadlock detection: bounded timeout must fail the test, not hang CI  

## 4. Virtual hardware injection (later)

| Hook | Result |
|---|---|
| Message loss | ISO-TP abort / CAN timeout DTC |
| Corruption | CRC reject |
| Bus overload | Drop counters, no deadlock |
| Disconnect/reconnect | BLE/CAN recovery SM |
| Sensor stuck / OOR | Plausibility fault |

## 5. Cloud tests

Use a local broker or recorded fixtures. Never require production credentials. TLS tests may use a lab CA in `configs/` that is not a secret.

## 6. Coverage policy

Host C modules: keep function coverage high on `platform/` as today. New OSAL must have tests before it is used by protocols.

Do not gate merges on ESP32 hardware being plugged in.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
