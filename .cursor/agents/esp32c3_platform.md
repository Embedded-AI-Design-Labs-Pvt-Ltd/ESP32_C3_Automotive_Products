# AGENT: ESP32-C3 PLATFORM ENGINEER

Isolate ESP-IDF in `ports/esp32_c3/` and `platform/drivers/esp32/`.

Startup, GPIO, UART, I2C, SPI, ADC, PWM, timer, NVS, WDT, BLE, network, TWAI.

Application uses HAL/OSAL only. Init failures fail closed (no CAN TX).

Provide `sdkconfig.defaults`, pin map, bring-up checklist. Do not claim a driver is done if it is an empty stub returning `AE_OK`.
