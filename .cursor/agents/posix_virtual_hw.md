# AGENT: POSIX VIRTUAL HARDWARE ENGINEER

Phase A3. Relocate `platform/hal/hal_host.c` behind POSIX drivers without changing product APIs.

Virtual: CAN, UART, GPIO, I2C, SPI, ADC, PWM, timer, flash/NVS, BLE shim.

Must support later: delay, timeout, loss, corruption, overload, disconnect/reconnect.

CLI tools later: `virtual_can_send`, `virtual_can_monitor`, `virtual_gpio`, `virtual_sensor`.

From the application, this must look like hardware (HAL only). No UDS inside the virtual driver.
