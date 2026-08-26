/**
 * @file hal_misc.h
 * @brief UART, GPIO, I2C, SPI, ADC, NVS, WDT HAL (host-stub capable).
 */
#ifndef HAL_MISC_H
#define HAL_MISC_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t baud;
} ae_uart_cfg_t;

typedef struct {
    uint32_t pin;
    uint8_t output;
} ae_gpio_cfg_t;

typedef void (*ae_gpio_isr_cb_t)(uint32_t pin, void *ctx);

ae_status_t hal_uart_init(const ae_uart_cfg_t *cfg);
ae_status_t hal_uart_write(const uint8_t *buf, size_t len, uint32_t timeout_ms);

ae_status_t hal_gpio_init(const ae_gpio_cfg_t *cfg);
ae_status_t hal_gpio_write(uint32_t pin, uint8_t level);
ae_status_t hal_gpio_read(uint32_t pin, uint8_t *level);
ae_status_t hal_gpio_attach_isr(uint32_t pin, ae_gpio_isr_cb_t cb, void *ctx);

ae_status_t hal_i2c_read(uint8_t addr, uint8_t *buf, size_t len, uint32_t to);
ae_status_t hal_i2c_set_sim(uint8_t addr, const uint8_t *buf, size_t len);
ae_status_t hal_spi_xfer(const uint8_t *tx, uint8_t *rx, size_t len, uint32_t to);
ae_status_t hal_adc_read(uint8_t ch, uint16_t *mv);
ae_status_t hal_adc_set_sim(uint8_t ch, uint16_t mv);

ae_status_t hal_nvs_set(const char *key, const void *buf, size_t len);
ae_status_t hal_nvs_get(const char *key, void *buf, size_t *len);

ae_status_t hal_wdg_kick(void);
uint32_t hal_wdg_kicks(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_MISC_H */
