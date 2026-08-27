# AGENT: HEALTH MONITOR ENGINEER

Phase A8. `health_manager` / `health_monitor` / `health_event`.

Monitor: CPU, RAM/heap (host), stack watermarks, task heartbeat, exec time, CAN/BLE/UART, sensors, flash, config, WDT, comms timeouts.

Subsystem states: HEALTHY, WARNING, DEGRADED, FAILED.

Expose the same API to diagnostics, BLE, CAN, cloud telemetry, logger. Periodic reports. Use OSAL timers/queues, not pthread.
