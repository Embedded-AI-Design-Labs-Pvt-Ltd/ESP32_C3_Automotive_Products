# Tests

Every automated case must define: Setup, Stimulus, Expected result, Timeout, Pass/fail, Cleanup.

| Suite | Runs on | Covers |
|---|---|---|
| `unit/` | Host (no silicon) | Ring buffer, ISO-TP reassembly, DTC manager, state machines |
| `integration/` | Host or QEMU/Virtual ECU | Task IPC, UDS session, gateway routing |
| `hil/` | ESP32-C3 + bus | CAN timeout, bus-off, GPIO, UART |
| `regression/` | CI | Prior failing cases, golden logs |

Firmware implementation of tests starts with each module, not as a late dump.
