/**
 * @file hal_arduino.cpp
 * @brief ESP32-C3 Arduino HAL: TWAI if present, else software CAN bus.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#if defined(ARDUINO)

#include "hal_can.h"
#include "hal_misc.h"
#include "ae_error.h"

#include <Arduino.h>
#include <string.h>

#if defined(ARDUINO_ARCH_ESP32)
#if __has_include(<Preferences.h>)
#include <Preferences.h>
#endif
#if __has_include(<driver/twai.h>)
#include <driver/twai.h>
#define AE_HAVE_TWAI 1
#endif
#if __has_include(<esp_task_wdt.h>)
#include <esp_task_wdt.h>
#define AE_HAVE_WDT 1
#endif
#if defined(AE_HAVE_TWAI)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif
#endif

#define AE_BUS_DEPTH 64u
#define AE_GPIO_MAX  48u
#define AE_NVS_MAX   8u

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
static int s_twai_ok;

typedef struct {
    char key[24];
    uint8_t data[64];
    size_t len;
    int used;
} ae_nvs_slot_t;

static ae_nvs_slot_t s_nvs[AE_NVS_MAX];

void hal_host_reset(void)
{
    memset(s_bus, 0, sizeof(s_bus));
    memset(s_nvs, 0, sizeof(s_nvs));
    s_bus_head = s_bus_tail = s_bus_count = 0;
    s_rx_cb = NULL;
    s_wdg = 0;
}

uint32_t hal_millis(void)
{
    return (uint32_t)millis();
}

void hal_can_bus_reset(void)
{
    s_bus_head = s_bus_tail = s_bus_count = 0;
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

static ae_status_t sw_send(const ae_can_frame_t *frame)
{
    uint32_t next = (s_bus_head + 1u) % AE_BUS_DEPTH;
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

ae_status_t hal_can_init(const ae_can_cfg_t *cfg)
{
    (void)cfg;
    hal_can_bus_reset();
    s_twai_ok = 0;
#if defined(AE_HAVE_TWAI)
    {
        twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)4, (gpio_num_t)5, TWAI_MODE_NORMAL);
        twai_timing_config_t t = TWAI_TIMING_CONFIG_500KBITS();
        twai_filter_config_t f = TWAI_FILTER_CONFIG_ACCEPT_ALL();
        if (twai_driver_install(&g, &t, &f) == ESP_OK && twai_start() == ESP_OK) {
            s_twai_ok = 1;
        }
    }
#endif
    return AE_OK;
}

ae_status_t hal_can_send(const ae_can_frame_t *frame, uint32_t timeout_ms)
{
    if (frame == NULL) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
#if defined(AE_HAVE_TWAI)
    if (s_twai_ok) {
        twai_message_t m;
        memset(&m, 0, sizeof(m));
        m.identifier = frame->id;
        m.extd = frame->ide;
        m.rtr = frame->rtr;
        m.data_length_code = frame->dlc;
        memcpy(m.data, frame->data, 8);
        (void)twai_transmit(&m, pdMS_TO_TICKS(timeout_ms == 0 ? 10 : timeout_ms));
    }
#else
    (void)timeout_ms;
#endif
    return sw_send(frame);
}

ae_status_t hal_can_recv(ae_can_frame_t *frame)
{
    if (frame == NULL) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
#if defined(AE_HAVE_TWAI)
    if (s_twai_ok) {
        twai_message_t m;
        if (twai_receive(&m, 0) == ESP_OK) {
            memset(frame, 0, sizeof(*frame));
            frame->id = m.identifier;
            frame->ide = m.extd;
            frame->rtr = m.rtr;
            frame->dlc = m.data_length_code;
            memcpy(frame->data, m.data, 8);
            frame->timestamp_us = (uint32_t)micros();
            if (s_rx_cb != NULL) {
                s_rx_cb(frame, s_rx_ctx);
            }
            return AE_OK;
        }
    }
#endif
    if (s_bus_head == s_bus_tail) {
        return ae_err_make(AE_MOD_HAL, 3u);
    }
    *frame = s_bus[s_bus_tail];
    s_bus_tail = (s_bus_tail + 1u) % AE_BUS_DEPTH;
    return AE_OK;
}

ae_status_t hal_uart_init(const ae_uart_cfg_t *cfg)
{
    Serial.begin(cfg != NULL ? cfg->baud : 115200);
    return AE_OK;
}

ae_status_t hal_uart_write(const uint8_t *buf, size_t len, uint32_t timeout_ms)
{
    (void)timeout_ms;
    if ((buf == NULL) && (len > 0u)) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    if (buf != NULL && len > 0u) {
        Serial.write(buf, len);
    }
    return AE_OK;
}

ae_status_t hal_gpio_init(const ae_gpio_cfg_t *cfg)
{
    if (cfg != NULL) {
        pinMode(cfg->pin, cfg->output ? OUTPUT : INPUT);
    }
    return AE_OK;
}

ae_status_t hal_gpio_write(uint32_t pin, uint8_t level)
{
    if (pin >= AE_GPIO_MAX) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    s_gpio[pin] = level != 0;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, level ? HIGH : LOW);
    return AE_OK;
}

ae_status_t hal_gpio_read(uint32_t pin, uint8_t *level)
{
    if ((pin >= AE_GPIO_MAX) || (level == NULL)) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
    *level = (uint8_t)digitalRead(pin);
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
    if (*mv == 0u) {
        *mv = (uint16_t)((analogRead(ch) * 3300u) / 4095u);
    }
    return AE_OK;
}

ae_status_t hal_nvs_set(const char *key, const void *buf, size_t len)
{
    uint8_t i;

    if ((key == NULL) || (buf == NULL) || (len > 64u)) {
        return ae_err_make(AE_MOD_HAL, 1u);
    }
#if defined(ARDUINO_ARCH_ESP32) && __has_include(<Preferences.h>)
    {
        Preferences p;
        if (p.begin("aegw", false)) {
            (void)p.putBytes(key, buf, (size_t)len);
            p.end();
        }
    }
#endif
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
            s_nvs[i].key[sizeof(s_nvs[i].key) - 1u] = 0;
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
#if defined(ARDUINO_ARCH_ESP32) && __has_include(<Preferences.h>)
    {
        Preferences p;
        if (p.begin("aegw", true)) {
            size_t n = p.getBytes(key, buf, *len);
            p.end();
            if (n > 0u) {
                *len = n;
                return AE_OK;
            }
        }
    }
#endif
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
#if defined(AE_HAVE_WDT)
    (void)esp_task_wdt_reset();
#endif
    return AE_OK;
}

uint32_t hal_wdg_kicks(void)
{
    return s_wdg;
}

#endif /* ARDUINO */
