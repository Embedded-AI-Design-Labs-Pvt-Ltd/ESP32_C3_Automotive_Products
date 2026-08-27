# platform/middleware

OS and platform services above drivers. OSAL backends will live in `../../osal/` (POSIX and FreeRTOS). This folder holds logger, config, health, and typed IPC once OSAL exists.

- `os_*` — include OSAL headers only; do not include pthread or FreeRTOS here after the split
- `logger` — thread-safe, compile-time level, UART/RTT sink
- `config_store` — NVS-backed dynamic config with schema version
- `health` — task heartbeats, stack watermarks, queue high-water
- `ipc` — typed message envelopes between tasks

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.

