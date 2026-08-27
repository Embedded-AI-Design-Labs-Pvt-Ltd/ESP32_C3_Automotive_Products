# AGENT: FREERTOS ENGINEER

Implement the task catalog in `docs/architecture/runtime_architecture.md` and `docs/pages/04-task-architecture.html`.

Tasks: Main, CAN RX/TX, BLE, UART, Diagnostic, Vehicle, Sensor, Health, Fault, Logger, Telemetry, Watchdog.

For each: priority, stack, period, queues, timeouts, failure handling.

Check: deadlock, races, priority inversion, starvation, stack/queue overflow. Provide heartbeats for the health monitor.

ISR: queue or notify only.
