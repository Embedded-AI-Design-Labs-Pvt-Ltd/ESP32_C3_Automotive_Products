/**
 * @file crc_e2e.h
 * @brief Classical CAN E2E helpers: CRC-8/16/32, alive counter, validate.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 * @note Prototype lab integrity — not AUTOSAR E2E Profile certified.
 */
#ifndef CRC_E2E_H
#define CRC_E2E_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** CRC-8 SAE J1850 polynomial 0x1D, init 0xFF, XOR out 0xFF (common auto). */
uint8_t ae_crc8_j1850(const uint8_t *data, size_t len);

uint16_t ae_crc16_ccitt(const uint8_t *data, size_t len);
uint32_t ae_crc32(const uint8_t *data, size_t len);

/**
 * Pack alive nibble into data[7] high nibble (bits 7..4), keep low nibble.
 * counter wraps 0..15.
 */
uint8_t ae_alive_next(uint8_t counter);

typedef enum {
    AE_E2E_OK = 0,
    AE_E2E_CRC,
    AE_E2E_COUNTER,
    AE_E2E_TIMEOUT,
    AE_E2E_DLC
} ae_e2e_result_t;

typedef struct {
    uint8_t last_counter;
    uint8_t initialized;
    uint32_t last_ms;
    uint32_t timeout_ms;
} ae_e2e_state_t;

/**
 * Validate lab layout: data[0..5] payload, data[6]=CRC8 over [0..5]+id_lo,
 * data[7] high nibble = alive counter. dlc must be 8.
 */
ae_e2e_result_t ae_e2e_protect(uint32_t can_id, uint8_t data[8], uint8_t *counter);
ae_e2e_result_t ae_e2e_check(uint32_t can_id, const uint8_t data[8], uint8_t dlc,
                             ae_e2e_state_t *st, uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif /* CRC_E2E_H */
