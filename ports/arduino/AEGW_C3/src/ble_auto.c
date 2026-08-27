/**
 * @file ble_auto.c
 * @brief Transport only. Products subscribe; no UDS parsing here.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "ble_auto.h"

#include "ae_error.h"

#include <string.h>

static ble_auto_write_cb_t s_cb;
static void *s_ctx;
static uint8_t s_last[80];
static uint16_t s_last_len;
static uint16_t s_last_ch;

ae_status_t ble_auto_init(ble_auto_write_cb_t cb, void *ctx)
{
    s_cb = cb;
    s_ctx = ctx;
    s_last_len = 0u;
    return AE_OK;
}

ae_status_t ble_auto_notify(uint16_t char_id, const uint8_t *data, uint16_t len)
{
    if ((data == NULL) || (len > sizeof(s_last))) {
        return ae_err_make(AE_MOD_BLE, 1u);
    }
    memcpy(s_last, data, len);
    s_last_len = len;
    s_last_ch = char_id;
    return AE_OK;
}

ae_status_t ble_auto_inject_write(uint16_t char_id, const uint8_t *data, uint16_t len)
{
    if (s_cb != NULL) {
        s_cb(char_id, data, len, s_ctx);
    }
    return AE_OK;
}

const uint8_t *ble_auto_last_notify(uint16_t *char_id, uint16_t *len)
{
    if (char_id != NULL) {
        *char_id = s_last_ch;
    }
    if (len != NULL) {
        *len = s_last_len;
    }
    return s_last;
}
