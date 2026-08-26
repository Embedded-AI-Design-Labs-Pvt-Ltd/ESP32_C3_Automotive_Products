# Agent 05 — CAN/TWAI (Phase 6)

Split `can_driver` (HAL/TWAI) from `can_service` (filter, timeout, stats, missing/unexpected).

Support standard and extended IDs. Bus-off recovery policy. Test hooks for overload, timeout, invalid, missing, unexpected.

Do not put UDS or BLE inside the driver.

Tests: Setup / Stimulus / Expected / Timeout / Pass-fail / Cleanup.
