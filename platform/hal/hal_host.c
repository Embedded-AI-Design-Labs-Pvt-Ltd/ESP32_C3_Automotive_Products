/**
 * @file hal_host.c
 * @brief Host / Virtual ECU HAL: in-memory CAN bus and simulated peripherals.
 * @note ESP32-C3 port replaces this file with TWAI / NimBLE / NVS drivers.
 */

#include "hal_can.h"
#include "hal_misc.h"

#include "ae_error.h"

#include <string.h>

#define AE_BUS_DEPTH 64u
#define AE_NVS_MAX   8u
#define AE_GPIO_MAX  16u

static ae_can_frame_t s_bus[AE_BUS_DEPTH];
static uint32_t s_bus_head;
static uint32_t s_bus_tail;
static uint32_t s_bus_count;
static ae_can_rx_isr_cb_t s_rx_cb;
static void *s_rx_ctx;

static uint8_t s_gpio[AE_GPIO_MAX];
static uint16_t s_adc[8];
static uint8_t s_i2c[256];
static uint32_t s_wdg;

typedef struct {
    char key[24];
    uint8_t data[64];
    size_t len;
    int used;
} ae_nvs_slot_t;

static ae_nvs_slot_t s_nvs[AE_NVS_MAX];

ae_status_t hal_can_init(const ae_can_cfg_t *cfg)
{
    (void)cfg;
    hal_can_bus_reset();
    return AE_OK;
}

void hal_can_bus_reset(void)
{
    s_bus_head = 0u;
    s_bus_tail = 0u;
    s_bus_count = 0u;
}

uint32_t hal_can_bus_count(void)
{
    return s_bus_count;
}

ae_status_t hal_can_attach_rx(ae_can_rx_isr_cb_t cb, void *ctx)
{
    s_rx_cb = cb;
    s_rx_ctx = ctx;
    return AE_OK;
}

ae_status_t hal_can_set_filter(const ae_can_filter_t *filters, uint8_t count)
{
    (void)filters;
    (void)count;
    return AE_OK;
}

/**
 * @brief Enqueue a frame. Host "ISR" callback runs in the caller context.
 */
ae_status_t hal_can_send(const ae_can_frame_t *frame, uint32_t timeout_ms)
{
    uint32_t next;

    (void)timeout_ms;
    if (frame == NULL) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    next = (s_bus_head + 1u) % AE_BUS_DEPTH;
    if (next == s_bus_tail) {
        return ae_err_make(AE_MOD_HAL, 2u);
    }
    s_bus[s_bus_head] = *frame;
    s_bus_head = next;
    s_bus_count++;
    if (s_rx_cb != NULL) {
        s_rx_cb(frame, s_rx_ctx);
    }
    return AE_OK;
}

ae_status_t hal_can_recv(ae_can_frame_t *frame)
{
    if (frame == NULL) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    if (s_bus_head == s_bus_tail) {
        return ae_err_make(AE_MOD_HAL, 3u);
    }
    *frame = s_bus[s_bus_tail];
    s_bus_tail = (s_bus_tail + 1u) % AE_BUS_DEPTH;
    return AE_OK;
}

ae_status_t hal_uart_init(const ae_uart_cfg_t *cfg)
{
    (void)cfg;
    return AE_OK;
}

ae_status_t hal_uart_write(const uint8_t *buf, size_t len, uint32_t timeout_ms)
{
    (void)timeout_ms;
    if ((buf == NULL) && (len > 0u)) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    return AE_OK;
}

ae_status_t hal_gpio_init(const ae_gpio_cfg_t *cfg)
{
    (void)cfg;
    return AE_OK;
}

ae_status_t hal_gpio_write(uint32_t pin, uint8_t level)
{
    if (pin >= AE_GPIO_MAX) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    s_gpio[pin] = (uint8_t)(level != 0u);
    return AE_OK;
}

ae_status_t hal_gpio_read(uint32_t pin, uint8_t *level)
{
    if ((pin >= AE_GPIO_MAX) || (level == NULL)) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    *level = s_gpio[pin];
    return AE_OK;
}

ae_status_t hal_gpio_attach_isr(uint32_t pin, ae_gpio_isr_cb_t cb, void *ctx)
{
    (void)pin;
    (void)cb;
    (void)ctx;
    return AE_OK;
}

ae_status_t hal_i2c_set_sim(uint8_t addr, const uint8_t *buf, size_t len)
{
    (void)addr;
    if ((buf == NULL) || (len == 0u) || (len > sizeof(s_i2c))) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    memcpy(s_i2c, buf, len);
    return AE_OK;
}

ae_status_t hal_i2c_read(uint8_t addr, uint8_t *buf, size_t len, uint32_t to)
{
    (void)addr;
    (void)to;
    if ((buf == NULL) || (len == 0u) || (len > sizeof(s_i2c))) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    memcpy(buf, s_i2c, len);
    return AE_OK;
}

ae_status_t hal_spi_xfer(const uint8_t *tx, uint8_t *rx, size_t len, uint32_t to)
{
    (void)to;
    if ((tx == NULL) || (rx == NULL)) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    memcpy(rx, tx, len);
    return AE_OK;
}

ae_status_t hal_adc_set_sim(uint8_t ch, uint16_t mv)
{
    if (ch >= 8u) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    s_adc[ch] = mv;
    return AE_OK;
}

ae_status_t hal_adc_read(uint8_t ch, uint16_t *mv)
{
    if ((ch >= 8u) || (mv == NULL)) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    *mv = s_adc[ch];
    return AE_OK;
}

ae_status_t hal_nvs_set(const char *key, const void *buf, size_t len)
{
    uint8_t i;

    if ((key == NULL) || (buf == NULL) || (len > 64u)) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    for (i = 0; i < AE_NVS_MAX; i++) {
        if ((s_nvs[i].used != 0) && (strncmp(s_nvs[i].key, key, sizeof(s_nvs[i].key)) == 0)) {
            memcpy(s_nvs[i].data, buf, len);
            s_nvs[i].len = len;
            return AE_OK;
        }
    }
    for (i = 0; i < AE_NVS_MAX; i++) {
        if (s_nvs[i].used == 0) {
            strncpy(s_nvs[i].key, key, sizeof(s_nvs[i].key) - 1u);
            memcpy(s_nvs[i].data, buf, len);
            s_nvs[i].len = len;
            s_nvs[i].used = 1;
            return AE_OK;
        }
    }
    return ae_err_make(AE_MOD_HAL, 2u);
}

ae_status_t hal_nvs_get(const char *key, void *buf, size_t *len)
{
    uint8_t i;

    if ((key == NULL) || (buf == NULL) || (len == NULL)) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    for (i = 0; i < AE_NVS_MAX; i++) {
        if ((s_nvs[i].used != 0) && (strncmp(s_nvs[i].key, key, sizeof(s_nvs[i].key)) == 0)) {
            if (*len < s_nvs[i].len) {
                return ae_err_make(AE_MOD_HAL, 4u);
            }
            memcpy(buf, s_nvs[i].data, s_nvs[i].len);
            *len = s_nvs[i].len;
            return AE_OK;
        }
    }
    return ae_err_make(AE_MOD_HAL, 3u);
}

ae_status_t hal_wdg_kick(void)
{
    s_wdg++;
    return AE_OK;
}

uint32_t hal_wdg_kicks(void)
{
    return s_wdg;
}
