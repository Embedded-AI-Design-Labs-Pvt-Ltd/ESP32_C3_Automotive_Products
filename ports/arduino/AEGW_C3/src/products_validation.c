/**
 * @file products_validation.c
 * @brief P06 test box, P07 simulator, P08 fault inject, P09 EOL, P15 HIL.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "product_api.h"
#include "product_dids.h"

#include "ae_can_ids.h"
#include "ae_error.h"
#include "can_service.h"
#include "ecu_models.h"
#include "fault_mgr.h"
#include "hal_can.h"
#include "hal_misc.h"
#if defined(AE_HOST)
#include "hal_can_adapter.h"
#endif
#include "uds.h"

#include <string.h>

static int s_drop_id_armed;
static uint32_t s_drop_id;
static uint8_t s_srv[64];
static uint16_t s_srv_n;

static ae_status_t sim_tx(const uint8_t *pdu, uint16_t len)
{
    if (len > sizeof(s_srv)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    memcpy(s_srv, pdu, len);
    s_srv_n = len;
    return uds_client_on_pdu(pdu, len);
}

static ae_status_t send_maybe(const ae_can_frame_t *f)
{
    if ((s_drop_id_armed != 0) && (f->id == s_drop_id)) {
        return ae_err_make(AE_MOD_CAN, 9u);
    }
    return can_svc_send(f);
}

ae_status_t p06_ecu_test_box_init(void)
{
    ecu_models_init();
    (void)can_svc_init();
    (void)hal_gpio_write(2u, 0u);
    return AE_OK;
}

ae_status_t p06_ecu_test_box_run(void)
{
    ae_can_frame_t cmd;
    ae_can_frame_t st;

    (void)p06_ecu_test_box_init();
    memset(&cmd, 0, sizeof(cmd));
    cmd.id = AE_CAN_BCM_LIGHT_CMD;
    cmd.dlc = 1u;
    cmd.data[0] = 0x07u;
    (void)bcm_on_light_cmd(&cmd, &st);
    (void)can_svc_send(&st);
    (void)hal_gpio_write(2u, 1u);
    return (st.id == AE_CAN_BCM_LIGHT_ST) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}

ae_status_t p07_ecu_simulator_init(void)
{
#if defined(AE_HOST)
    (void)hal_can_init_from_env();
#else
    ae_can_cfg_t cfg = AE_CAN_CFG_500K;
    (void)hal_can_init(&cfg);
#endif
    ecu_models_init();
    (void)can_svc_init();
    (void)uds_server_init(sim_tx);
    (void)uds_register_did(AE_DID_VIN, product_did_vin_read, NULL);
    (void)uds_register_routine(0x0202u, product_routine_selftest);
    s_srv_n = 0u;
    return AE_OK;
}

ae_status_t p07_ecu_simulator_run(void)
{
    ae_can_frame_t f;

    (void)p07_ecu_simulator_init();
    (void)bcm_fill_status(&f);
    (void)can_svc_send(&f);
    (void)vcu_fill_hb(&f);
    (void)can_svc_send(&f);
    (void)bms_fill_status(&f);
    (void)can_svc_send(&f);
    (void)uds_server_on_pdu((const uint8_t *)"\x3E\x00", 2u);
    return (s_srv_n >= 1u) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}

ae_status_t p08_fault_inject_init(void)
{
    (void)fault_init();
    (void)can_svc_init();
    s_drop_id_armed = 0;
    return AE_OK;
}

ae_status_t p08_fault_inject_run(void)
{
    ae_can_frame_t f;
    ae_status_t st;

    (void)p08_fault_inject_init();
    s_drop_id = AE_CAN_VCU_TORQUE;
    s_drop_id_armed = 1;
    memset(&f, 0, sizeof(f));
    f.id = AE_CAN_BCM_STATUS;
    f.dlc = 8u;
    (void)send_maybe(&f);
    f.id = AE_CAN_VCU_TORQUE;
    st = send_maybe(&f);
    (void)fault_report(s_drop_id, 1);
    return (!ae_err_is_ok(st) && (fault_get_state() == FAULT_RECOVERY))
               ? AE_OK
               : ae_err_make(AE_MOD_APP, 10u);
}

ae_status_t p09_eol_tester_init(void)
{
    (void)uds_server_init(sim_tx);
    (void)uds_client_init(uds_server_on_pdu);
    (void)uds_register_did(AE_DID_VIN, product_did_vin_read, NULL);
    (void)uds_register_routine(0x0202u, product_routine_selftest);
    return AE_OK;
}

ae_status_t p09_eol_tester_run(void)
{
    uint8_t sess[1] = {UDS_SESS_EXT};
    uint8_t did[2] = {(uint8_t)(AE_DID_VIN >> 8), (uint8_t)(AE_DID_VIN & 0xFFu)};
    uint8_t rc[3] = {0x01u, 0x02u, 0x02u};
    uint16_t n = 0u;
    const uint8_t *p;

    (void)p09_eol_tester_init();
    (void)uds_client_request(UDS_SID_DSC, sess, 1u);
    (void)uds_client_request(UDS_SID_RDBI, did, 2u);
    p = uds_client_last(&n);
    if ((n < 4u) || (p[0] != UDS_POS(UDS_SID_RDBI))) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    (void)uds_client_request(UDS_SID_RC, rc, 3u);
    p = uds_client_last(&n);
    return (n >= 1u) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}

ae_status_t p15_hil_rcp_init(void)
{
    (void)can_svc_init();
    (void)hal_gpio_write(4u, 0u);
    return AE_OK;
}

ae_status_t p15_hil_rcp_run(void)
{
    ae_can_frame_t f;
    uint8_t level = 0u;

    (void)p15_hil_rcp_init();
    /* UART/TCP command proto: I 1A0 01 — inject CAN ID 0x1A0 */
    memset(&f, 0, sizeof(f));
    f.id = AE_CAN_BCM_STATUS;
    f.dlc = 8u;
    f.timestamp_us = 1000u;
    (void)can_svc_send(&f);
    (void)hal_gpio_write(4u, 1u);
    (void)hal_gpio_read(4u, &level);
    return (level == 1u) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}
