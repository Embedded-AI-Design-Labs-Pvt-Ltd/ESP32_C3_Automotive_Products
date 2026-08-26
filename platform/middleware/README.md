# platform/middleware

OS and platform services above drivers.

- `os_queue`, `os_mutex`, `os_sem`, `os_event`, `os_timer`, `os_notify` — FreeRTOS wrappers
- `logger` — thread-safe, compile-time level, UART/RTT sink
- `config_store` — NVS-backed dynamic config with schema version
- `health` — task heartbeats, stack watermarks, queue high-water
- `ipc` — typed message envelopes between tasks
