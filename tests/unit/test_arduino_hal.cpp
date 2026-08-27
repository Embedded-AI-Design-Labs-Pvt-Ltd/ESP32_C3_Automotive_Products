/**
 * @file test_arduino_hal.cpp
 * @brief Host compilation + branch tests for ports/arduino/AEGW_C3/src/hal_arduino.cpp.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include <cstdio>
#include <cstring>

#include "ae_error.h"
#include "hal_can.h"
#include "hal_misc.h"

static int g_fail;
static int g_cb;

static void expect(int cond, const char *name)
{
    if (!cond) {
        std::printf("FAIL %s\n", name);
        g_fail = 1;
    }
}

static void on_rx(const ae_can_frame_t *f, void *c)
{
    (void)f;
    (void)c;
    g_cb = 1;
}

int main(void)
{
    ae_can_cfg_t can = {500000u};
    ae_uart_cfg_t uart = {115200u};
    ae_gpio_cfg_t gpio = {2u, 1u};
    ae_can_filter_t flt = {0x1A0u, 0x7FFu};
    ae_can_frame_t f;
    uint8_t lvl = 0;
    uint16_t mv = 0;
    uint8_t buf[8];
    uint8_t i2c[2] = {1, 2};
    size_t n;

    g_fail = 0;
    hal_host_reset();
    expect(hal_can_init(&can) == AE_OK, "init");
    expect(hal_can_set_filter(&flt, 1) == AE_OK, "filt");
    expect(hal_can_attach_rx(on_rx, NULL) == AE_OK, "isr");
    expect(hal_can_send(NULL, 0) != AE_OK, "txn");
    expect(hal_can_recv(NULL) != AE_OK, "rxn");
    expect(hal_can_recv(&f) != AE_OK, "rxe");
    std::memset(&f, 0, sizeof(f));
    f.id = 0x1A0u;
    f.dlc = 1u;
    g_cb = 0;
    expect(hal_can_send(&f, 10) == AE_OK && g_cb == 1, "tx");
    expect(hal_can_bus_count() >= 1u, "cnt");
    expect(hal_can_recv(&f) == AE_OK, "rx");
    expect(hal_uart_init(&uart) == AE_OK, "uart");
    expect(hal_uart_write(NULL, 1, 0) != AE_OK, "uartb");
    expect(hal_uart_write((const uint8_t *)"x", 1, 0) == AE_OK, "uartw");
    expect(hal_gpio_init(&gpio) == AE_OK, "gpioi");
    expect(hal_gpio_write(99, 1) != AE_OK, "gpiop");
    expect(hal_gpio_write(2, 1) == AE_OK, "gpiow");
    expect(hal_gpio_read(99, &lvl) != AE_OK, "gpiorb");
    expect(hal_gpio_read(2, NULL) != AE_OK, "gpiorn");
    expect(hal_gpio_read(2, &lvl) == AE_OK, "gpior");
    expect(hal_gpio_attach_isr(2, NULL, NULL) == AE_OK, "gpioisr");
    expect(hal_i2c_set_sim(0x48, NULL, 1) != AE_OK, "i2cs");
    expect(hal_i2c_set_sim(0x48, i2c, 2) == AE_OK, "i2cok");
    expect(hal_i2c_read(0x48, NULL, 2, 0) != AE_OK, "i2cr");
    expect(hal_i2c_read(0x48, buf, 2, 0) == AE_OK, "i2crd");
    expect(hal_spi_xfer(NULL, buf, 1, 0) != AE_OK, "spib");
    expect(hal_spi_xfer(i2c, buf, 2, 0) == AE_OK, "spi");
    expect(hal_adc_set_sim(9, 1) != AE_OK, "adcs");
    expect(hal_adc_set_sim(0, 0) == AE_OK, "adc0");
    expect(hal_adc_read(9, &mv) != AE_OK, "adcrb");
    expect(hal_adc_read(0, NULL) != AE_OK, "adcrn");
    expect(hal_adc_read(0, &mv) == AE_OK && mv != 0, "adcr");
    expect(hal_nvs_set(NULL, buf, 1) != AE_OK, "nvss");
    n = 8;
    expect(hal_nvs_get(NULL, buf, &n) != AE_OK, "nvsg");
    expect(hal_nvs_get("nope", buf, &n) != AE_OK, "nvsm");
    expect(hal_nvs_set("k", "ab", 2) == AE_OK, "nvsset");
    n = 1;
    expect(hal_nvs_get("k", buf, &n) != AE_OK, "nvssm");
    n = 8;
    expect(hal_nvs_get("k", buf, &n) == AE_OK && n == 2, "nvsget");
    expect(hal_nvs_set("k", "xy", 2) == AE_OK, "nvsup");
    expect(hal_wdg_kick() == AE_OK && hal_wdg_kicks() >= 1u, "wdg");
    expect(hal_millis() < 1000000u, "ms");
    std::printf(g_fail ? "RESULT FAIL\n" : "RESULT PASS\n");
    return g_fail;
}
