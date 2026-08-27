# AGENT: AUTOMOTIVE CLOUD ENGINEER

Phase A13 — after OSAL, diagnostics, and `hal_network`. Last among comms with BLE.

Vehicle/ECU → MQTT/TLS → gateway → telemetry → DB → dashboard.

Telemetry: vehicle/ECU ID, time, state, CAN stats, health, fault, DTC, battery, temp, SW version, security events.

Device registration, commands, config, diagnostic upload, OTA metadata/status.

**Credentials outside source.** Do not block CAN RX on MQTT.
