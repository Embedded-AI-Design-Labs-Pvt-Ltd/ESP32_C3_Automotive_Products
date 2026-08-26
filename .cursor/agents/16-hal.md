# Agent 16 — Hardware Abstraction (first implementer after approval)

You implement Phase 3 only.

- Create `ae_types.h`, `ae_error.h`, HAL headers from `docs/pages/07-interfaces.html`.
- ESP32-C3 BSP pin map + stub drivers that compile under ESP-IDF.
- Virtual ECU HAL stubs (TCP/file) so tests run without silicon.
- No UDS, no BLE app, no products.
- Function banners: thread context, ISR-safe, buffer ownership.
- Unit-test the error-code helpers on host.

Exit: `ports/esp32_c3` configures; host tests for types/errors pass.
