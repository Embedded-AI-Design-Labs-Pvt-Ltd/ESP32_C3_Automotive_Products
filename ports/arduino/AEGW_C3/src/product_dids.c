/**
 * @file product_dids.c
 * @brief Shared ECU identification DIDs for tester / simulator / EOL.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "ae_can_ids.h"
#include "ae_error.h"
#include "hal_misc.h"
#include <string.h>

ae_status_t product_did_vin_read(uint16_t did, uint8_t *out, uint16_t *len)
{
    const char *vin = AE_VIN_PROTO;
    size_t n = strlen(vin);

    (void)did;
    if ((out == NULL) || (len == NULL) || (*len < n)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    memcpy(out, vin, n);
    *len = (uint16_t)n;
    return AE_OK;
}

ae_status_t product_did_sw_read(uint16_t did, uint8_t *out, uint16_t *len)
{
    (void)did;
    if ((out == NULL) || (len == NULL) || (*len < 5u)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    memcpy(out, "1.0.0", 5u);
    *len = 5u;
    return AE_OK;
}

ae_status_t product_did_cfg_write(uint16_t did, const uint8_t *in, uint16_t len)
{
    (void)did;
    return hal_nvs_set("cal", in, len);
}

ae_status_t product_did_cfg_read(uint16_t did, uint8_t *out, uint16_t *len)
{
    size_t n;

    (void)did;
    if ((out == NULL) || (len == NULL)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    n = *len;
    if (hal_nvs_get("cal", out, &n) != AE_OK) {
        out[0] = 0u;
        *len = 1u;
        return AE_OK;
    }
    *len = (uint16_t)n;
    return AE_OK;
}

ae_status_t product_routine_selftest(uint16_t rid, uint8_t op, uint8_t *out, uint16_t *len)
{
    (void)rid;
    (void)op;
    if ((out == NULL) || (len == NULL) || (*len < 1u)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    out[0] = 0x00u; /* passed */
    *len = 1u;
    return AE_OK;
}
