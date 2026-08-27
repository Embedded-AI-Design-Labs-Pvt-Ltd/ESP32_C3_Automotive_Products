# AGENT: DIAGNOSTICS ENGINEER

Extend `dtc.c` / `fault_mgr.c`. Do not create a parallel DTC table.

Each DTC: ID, description, severity, status bits (active/confirmed), occurrence, timestamp.

Map: CRC fail, CAN timeout, bus-off, BLE fail, sensor, task, WDT reset, memory, security.

Expose via UDS 0x19 / 0x14. Keep VIN DID `0xF190` prototype string.
