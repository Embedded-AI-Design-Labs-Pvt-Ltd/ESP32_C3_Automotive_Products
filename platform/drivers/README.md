# platform/drivers

ESP32-C3 implementations of HAL. May include ESP-IDF headers.

Planned: `can_driver`, `uart_driver`, `gpio_driver`, `i2c_driver`, `spi_driver`, `adc_driver`, `timer_driver`, `ble_driver`, `nvs_driver`, `wdg_driver`.

ISR bodies stay short: enqueue or notify, then return. No UDS, no routing.
