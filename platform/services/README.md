# platform/services

Automotive services used by multiple products.

- `dtc_manager` — detect, confirm, clear, freeze-frame lite
- `diag_session` — default / programming / extended
- `gateway_router` — BLE↔CAN and CAN ID route table
- `security_proto` — access control, rate limit, anomaly hooks (not certified)
- `ota_agent` — download / verify / commit / rollback states
- `fault_manager` — NORMAL → FAULT → DEGRADED → RECOVERY → SAFE
- `ecu_ident` — VIN, SW/HW version, fingerprint
