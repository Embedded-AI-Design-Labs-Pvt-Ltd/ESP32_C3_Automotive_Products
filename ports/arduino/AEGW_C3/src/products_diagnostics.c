/**
 * @file products_diagnostics.c
 * @brief P03 UDS tester, P04 logger, P05 analyzer, P10 config/calibration.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "product_api.h"
#include "product_dids.h"

#include "ae_can_ids.h"
#include "ae_error.h"
#include "can_service.h"
#include "dtc.h"
#include "hal_can.h"
#include "hal_misc.h"
#include "uds.h"

#include <string.h>

static uint8_t s_srv_out[80];
static uint16_t s_srv_n;

static ae_status_t loop_tx(const uint8_t *pdu, uint16_t len)
{
    if (len > sizeof(s_srv_out)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    memcpy(s_srv_out, pdu, len);
    s_srv_n = len;
    return uds_client_on_pdu(pdu, len);
}

ae_status_t p03_uds_tester_init(void)
{
    (void)dtc_init();
    (void)uds_server_init(loop_tx);
    (void)uds_client_init(uds_server_on_pdu);
    (void)uds_register_did(AE_DID_VIN, product_did_vin_read, NULL);
    (void)uds_register_did(AE_DID_SW, product_did_sw_read, NULL);
    s_srv_n = 0u;
    return AE_OK;
}

/**
 * @brief Tester → ECU 0x22 F190 VIN. Expect positive response 0x62.
 */
ae_status_t p03_uds_tester_run(void)
{
    uint8_t did[2] = {(uint8_t)(AE_DID_VIN >> 8), (uint8_t)(AE_DID_VIN & 0xFFu)};
    uint16_t n = 0u;
    const uint8_t *p;

    (void)p03_uds_tester_init();
    (void)uds_client_request(UDS_SID_RDBI, did, 2u);
    p = uds_client_last(&n);
    if ((n < 4u) || (p[0] != UDS_POS(UDS_SID_RDBI))) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    return AE_OK;
}

#define AE_LOG_MAX 32u
static ae_can_frame_t s_log[AE_LOG_MAX];
static uint8_t s_log_n;

ae_status_t p04_can_logger_init(void)
{
    s_log_n = 0u;
    return can_svc_init();
}

ae_status_t p04_can_logger_run(void)
{
    ae_can_frame_t f;
    uint8_t i;

    (void)p04_can_logger_init();
    for (i = 0; i < 4u; i++) {
        memset(&f, 0, sizeof(f));
        f.id = AE_CAN_BCM_STATUS;
        f.dlc = 8u;
        f.timestamp_us = (uint32_t)i * 1000u;
        f.data[2] = i;
        if (s_log_n < AE_LOG_MAX) {
            s_log[s_log_n++] = f;
        }
        (void)can_svc_on_rx(&f);
    }
    return (s_log_n == 4u) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}

ae_status_t p05_ecu_analyzer_init(void)
{
    (void)can_svc_init();
    (void)dtc_init();
    return can_svc_expect(AE_CAN_BCM_STATUS, 100u, 300u);
}

ae_status_t p05_ecu_analyzer_run(void)
{
    uint8_t i;

    (void)p05_ecu_analyzer_init();
    /* No 0x1A0 received: tick past timeout. */
    (void)can_svc_tick(0u);
    (void)can_svc_tick(400u);
    if (can_svc_id_missing(AE_CAN_BCM_STATUS) == 0) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    for (i = 0; i < 3u; i++) {
        (void)dtc_raise(AE_DTC_CAN_TIMEOUT_BCM, DTC_SEV_WARN);
    }
    return dtc_confirmed(AE_DTC_CAN_TIMEOUT_BCM) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}

ae_status_t p10_config_cal_init(void)
{
    (void)uds_server_init(loop_tx);
    (void)uds_register_did(0xF100u, product_did_cfg_read, product_did_cfg_write);
    return AE_OK;
}

ae_status_t p10_config_cal_run(void)
{
    uint8_t val[2] = {0xAAu, 0x55u};
    uint8_t rd[2];
    size_t n = sizeof(rd);

    (void)p10_config_cal_init();
    if (hal_nvs_set("cal", val, sizeof(val)) != AE_OK) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    if (hal_nvs_get("cal", rd, &n) != AE_OK) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    return ((rd[0] == 0xAAu) && (rd[1] == 0x55u)) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}
