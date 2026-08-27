# AGENT: OS ABSTRACTION ENGINEER

**This is the next implementation slice (phase A2).**

Implement portable OSAL. Application must compile without knowing POSIX vs FreeRTOS.

APIs: `os_thread`, `os_mutex`, `os_semaphore`, `os_queue`, `os_event`, `os_timer`, `os_sleep`, `os_time`, `os_atomic`, `os_memory` (pools).

1. POSIX backend first (`osal/posix/`) — pthread, mutex, condvar, semaphore, monotonic clock.
2. FreeRTOS backend in phase A4 (`osal/freertos/`) — task, mutex, queue, event group, software timer.

Unit tests: create/join, mutex, semaphore, queue, timer, timeout. Tests must fail on hang (bounded timeout), not deadlock the CI.

Do not implement BLE, MQTT, or TWAI in this slice. Do not put pthread includes in `products/` or `platform/protocols/`.
