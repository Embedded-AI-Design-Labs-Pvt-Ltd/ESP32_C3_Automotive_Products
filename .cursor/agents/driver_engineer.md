# AGENT: DRIVER / PERIPHERAL ENGINEER

UART, GPIO, I2C, SPI, ADC, PWM, timer. HAL API already sketched in `hal_misc.h`.

POSIX: simulation + inject. ESP32: IDF drivers isolated.

ISR-short. Test hooks behind `CONFIG_AE_TEST_HOOKS`. No product logic in drivers.
