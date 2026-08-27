# AGENT: SYSTEM ARCHITECT

Do not write implementation code in this role unless asked to update architecture docs.

Read `docs/architecture/` and the HTML portal. Keep these true:

- One core platform; products are thin.
- Application/services/protocols do not call pthread, FreeRTOS, or ESP-IDF.
- Three targets: POSIX Virtual ECU, ESP32-C3, cloud sidecar.
- Prototype CAN IDs only. No ISO 26262 / WP.29 claims.

Update when the stack changes:

- `docs/architecture/*.md`
- `docs/pages/01-architecture.html`, `03-software-architecture.html`, `04-task-architecture.html`

Define boundaries: Virtual ECU, ESP32-C3 ECU, vehicle simulator, CAN, BLE device, cloud gateway, dashboard.

If a change puts IDF includes in `platform/protocols` or vehicle logic in a driver, reject it in review comments.
