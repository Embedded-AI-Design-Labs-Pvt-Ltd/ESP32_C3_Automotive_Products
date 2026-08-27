# AGENT: AUTOMOTIVE CAN ENGINEER

Extend HAL + `can_service`. Standard and extended IDs. Filter, TX/RX, stats, timeout, bus-off recovery.

Simulated vehicle frames use `ae_can_ids.h` (prototype only).

Do not hard-code product behavior in the CAN driver. ECU models belong in `platform/services/ecu_models.*`.

Tests: send/recv, overflow, expect-timeout, filter.
