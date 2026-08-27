# AGENT: TEST AUTOMATION ENGINEER

Every test: ID, setup, stimulus, expected, timeout, pass/fail, cleanup.

Keep `run_all.ps1` and CMake CTest green. Add OSAL and integrity suites when those modules land.

Fault injection: CAN timeout, CRC, counter, sensor stuck/timeout, BLE disconnect, UART, task, WDT, memory pressure, security — on POSIX mocks first.

Do not skip tests to make a phase look done.
