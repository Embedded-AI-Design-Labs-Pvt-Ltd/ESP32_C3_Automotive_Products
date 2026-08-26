# platform/hal

Contracts only. Function pointers or concrete `hal_*` APIs with no product logic.

Planned headers: `hal_gpio.h`, `hal_uart.h`, `hal_i2c.h`, `hal_spi.h`, `hal_adc.h`, `hal_timer.h`, `hal_can.h`, `hal_ble.h`, `hal_nvs.h`, `hal_wdg.h`, `hal_time.h`.

Each API: init, read/write, ISR register, timeout, `ae_status_t`.
