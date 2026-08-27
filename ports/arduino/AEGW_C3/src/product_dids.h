/**
 * @file product_dids.h
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#ifndef PRODUCT_DIDS_H
#define PRODUCT_DIDS_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

ae_status_t product_did_vin_read(uint16_t did, uint8_t *out, uint16_t *len);
ae_status_t product_did_sw_read(uint16_t did, uint8_t *out, uint16_t *len);
ae_status_t product_did_cfg_write(uint16_t did, const uint8_t *in, uint16_t len);
ae_status_t product_did_cfg_read(uint16_t did, uint8_t *out, uint16_t *len);
ae_status_t product_routine_selftest(uint16_t rid, uint8_t op, uint8_t *out, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif /* PRODUCT_DIDS_H */
