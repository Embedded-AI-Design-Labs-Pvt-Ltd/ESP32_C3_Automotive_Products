/**
 * @file dtc.c
 * @brief Confirm after 3 raises. Same table for UDS / BLE / UART.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "dtc.h"

#include "ae_error.h"

#include <string.h>

#define AE_DTC_MAX 16u

static dtc_record_t s_dtc[AE_DTC_MAX];

ae_status_t dtc_init(void)
{
    memset(s_dtc, 0, sizeof(s_dtc));
    return AE_OK;
}

ae_status_t dtc_raise(uint32_t code, dtc_severity_t sev)
{
    uint8_t i;
    int empty = -1;

    for (i = 0; i < AE_DTC_MAX; i++) {
        if (s_dtc[i].code == code) {
            s_dtc[i].status |= 0x01u;
            if (s_dtc[i].confirm_cnt < 3u) {
                s_dtc[i].confirm_cnt++;
            }
            if (s_dtc[i].confirm_cnt >= 3u) {
                s_dtc[i].status |= 0x02u;
            }
            s_dtc[i].sev = sev;
            return AE_OK;
        }
        if ((s_dtc[i].code == 0u) && (empty < 0)) {
            empty = (int)i;
        }
    }
    if (empty < 0) {
        return ae_err_make(AE_MOD_DTC, 2u);
    }
    s_dtc[empty].code = code;
    s_dtc[empty].status = 0x01u;
    s_dtc[empty].confirm_cnt = 1u;
    s_dtc[empty].sev = sev;
    return AE_OK;
}

ae_status_t dtc_clear(uint32_t mask)
{
    uint8_t i;

    for (i = 0; i < AE_DTC_MAX; i++) {
        if ((s_dtc[i].code != 0u) && ((s_dtc[i].code & mask) != 0u)) {
            memset(&s_dtc[i], 0, sizeof(s_dtc[i]));
        }
    }
    if (mask == 0xFFFFFFFFu) {
        memset(s_dtc, 0, sizeof(s_dtc));
    }
    return AE_OK;
}

ae_status_t dtc_read(dtc_record_t *out, uint8_t *count)
{
    uint8_t i;
    uint8_t n = 0u;
    uint8_t max;

    if ((out == NULL) || (count == NULL)) {
        return ae_err_make(AE_MOD_DTC, 1u);
    }
    max = *count;
    for (i = 0; (i < AE_DTC_MAX) && (n < max); i++) {
        if (s_dtc[i].code != 0u) {
            out[n++] = s_dtc[i];
        }
    }
    *count = n;
    return AE_OK;
}

int dtc_confirmed(uint32_t code)
{
    uint8_t i;

    for (i = 0; i < AE_DTC_MAX; i++) {
        if ((s_dtc[i].code == code) && ((s_dtc[i].status & 0x02u) != 0u)) {
            return 1;
        }
    }
    return 0;
}
