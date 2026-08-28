/**
 * @file crc_e2e.c
 * @brief CRC and E2E helpers for Classical CAN lab messages.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "crc_e2e.h"

#include <string.h>

uint8_t ae_crc8_j1850(const uint8_t *data, size_t len)
{
    uint8_t crc = 0xFFu;
    size_t i;
    uint8_t bit;

    if (data == NULL && len > 0u) {
        return 0u;
    }
    for (i = 0u; i < len; i++) {
        crc ^= data[i];
        for (bit = 0u; bit < 8u; bit++) {
            if ((crc & 0x80u) != 0u) {
                crc = (uint8_t)((crc << 1) ^ 0x1Du);
            } else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return (uint8_t)(crc ^ 0xFFu);
}

uint16_t ae_crc16_ccitt(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFu;
    size_t i;
    uint8_t bit;

    if (data == NULL && len > 0u) {
        return 0u;
    }
    for (i = 0u; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (bit = 0u; bit < 8u; bit++) {
            if ((crc & 0x8000u) != 0u) {
                crc = (uint16_t)((crc << 1) ^ 0x1021u);
            } else {
                crc = (uint16_t)(crc << 1);
            }
        }
    }
    return crc;
}

uint32_t ae_crc32(const uint8_t *data, size_t len)
{
    uint32_t crc = 0xFFFFFFFFu;
    size_t i;
    uint8_t bit;

    if (data == NULL && len > 0u) {
        return 0u;
    }
    for (i = 0u; i < len; i++) {
        crc ^= data[i];
        for (bit = 0u; bit < 8u; bit++) {
            if ((crc & 1u) != 0u) {
                crc = (crc >> 1) ^ 0xEDB88320u;
            } else {
                crc >>= 1;
            }
        }
    }
    return ~crc;
}

uint8_t ae_alive_next(uint8_t counter)
{
    return (uint8_t)((counter + 1u) & 0x0Fu);
}

static uint8_t e2e_crc_input(uint32_t can_id, const uint8_t data[8], uint8_t tmp[7])
{
    tmp[0] = (uint8_t)(can_id & 0xFFu);
    memcpy(&tmp[1], data, 6u);
    return ae_crc8_j1850(tmp, 7u);
}

ae_e2e_result_t ae_e2e_protect(uint32_t can_id, uint8_t data[8], uint8_t *counter)
{
    uint8_t tmp[7];
    uint8_t c;

    if (data == NULL || counter == NULL) {
        return AE_E2E_DLC;
    }
    c = (uint8_t)(*counter & 0x0Fu);
    data[7] = (uint8_t)((c << 4) | (data[7] & 0x0Fu));
    data[6] = e2e_crc_input(can_id, data, tmp);
    *counter = ae_alive_next(c);
    return AE_E2E_OK;
}

ae_e2e_result_t ae_e2e_check(uint32_t can_id, const uint8_t data[8], uint8_t dlc,
                             ae_e2e_state_t *st, uint32_t now_ms)
{
    uint8_t tmp[7];
    uint8_t expect;
    uint8_t got;
    uint8_t ctr;

    if (data == NULL || st == NULL) {
        return AE_E2E_DLC;
    }
    if (dlc != 8u) {
        return AE_E2E_DLC;
    }
    if (st->initialized != 0u && st->timeout_ms != 0u) {
        if ((now_ms - st->last_ms) > st->timeout_ms) {
            return AE_E2E_TIMEOUT;
        }
    }
    expect = e2e_crc_input(can_id, data, tmp);
    if (data[6] != expect) {
        return AE_E2E_CRC;
    }
    ctr = (uint8_t)((data[7] >> 4) & 0x0Fu);
    if (st->initialized != 0u) {
        got = ae_alive_next(st->last_counter);
        if (ctr != got) {
            return AE_E2E_COUNTER;
        }
    }
    st->last_counter = ctr;
    st->last_ms = now_ms;
    st->initialized = 1u;
    return AE_E2E_OK;
}
