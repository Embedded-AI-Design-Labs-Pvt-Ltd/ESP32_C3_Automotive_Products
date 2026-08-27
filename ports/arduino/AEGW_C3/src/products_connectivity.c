/**
 * @file products_connectivity.c
 * @brief P01 gateway, P02 OBD, P11 sensor GW, P12 TPMS, P13 EV battery GW.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "product_api.h"

#include "ae_can_ids.h"
#include "ae_error.h"
#include "ble_auto.h"
#include "can_service.h"
#include "dtc.h"
#include "ecu_models.h"
#include "hal_can.h"
#include "hal_misc.h"
#include "isotp.h"
#include "uds.h"

#include <string.h>

static ae_can_frame_t s_last_can;
static int s_got_can;

static void gw_on_can(const ae_can_frame_t *f, void *ctx)
{
    uint8_t n[12];

    (void)ctx;
    s_last_can = *f;
    s_got_can = 1;
    n[0] = (uint8_t)(f->id >> 8);
    n[1] = (uint8_t)(f->id & 0xFFu);
    n[2] = f->dlc;
    memcpy(&n[3], f->data, 8u);
    (void)ble_auto_notify(AE_BLE_CHAR_CAN_CMD, n, 11u);
}

static void gw_on_ble(uint16_t char_id, const uint8_t *data, uint16_t len, void *ctx)
{
    ae_can_frame_t f;

    (void)ctx;
    if ((char_id != AE_BLE_CHAR_CAN_CMD) || (data == NULL) || (len < 3u)) {
        return;
    }
    memset(&f, 0, sizeof(f));
    f.id = ((uint32_t)data[0] << 8) | data[1];
    f.dlc = data[2];
    if (f.dlc > 8u) {
        f.dlc = 8u;
    }
    if (len >= (uint16_t)(3u + f.dlc)) {
        memcpy(f.data, &data[3], f.dlc);
    }
    (void)can_svc_send(&f);
}

ae_status_t p01_ble_can_gateway_init(void)
{
    ae_can_cfg_t cfg = {500000u};

    (void)hal_can_init(&cfg);
    (void)can_svc_init();
    (void)ble_auto_init(gw_on_ble, NULL);
    s_got_can = 0;
    return can_svc_subscribe(AE_CAN_BCM_STATUS, gw_on_can, NULL);
}

/**
 * @brief BLE command 0x1A2 → CAN; BCM status 0x1A0 → BLE notify.
 */
ae_status_t p01_ble_can_gateway_run(void)
{
    uint8_t cmd[4] = {0x01u, 0xA2u, 0x01u, 0x07u};
    ae_can_frame_t st;

    (void)p01_ble_can_gateway_init();
    /* Reject unknown characteristic and short/oversize DLC (gateway edges). */
    (void)ble_auto_inject_write(0x99u, cmd, sizeof(cmd));
    (void)ble_auto_inject_write(AE_BLE_CHAR_CAN_CMD, cmd, 2u);
    {
        uint8_t long_cmd[12] = {0x01u, 0xA2u, 0x09u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u};
        (void)ble_auto_inject_write(AE_BLE_CHAR_CAN_CMD, long_cmd, sizeof(long_cmd));
    }
    (void)ble_auto_inject_write(AE_BLE_CHAR_CAN_CMD, cmd, sizeof(cmd));
    memset(&st, 0, sizeof(st));
    st.id = AE_CAN_BCM_STATUS;
    st.dlc = 8u;
    st.data[2] = 42u;
    (void)can_svc_on_rx(&st);
    return (s_got_can != 0) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}

static uint8_t s_obd_resp[32];
static uint16_t s_obd_n;

static ae_status_t obd_tx(const uint8_t *pdu, uint16_t len)
{
    if (len > sizeof(s_obd_resp)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    memcpy(s_obd_resp, pdu, len);
    s_obd_n = len;
    return AE_OK;
}

static void obd_on_pdu(const uint8_t *pdu, uint16_t len, void *ctx)
{
    uint8_t r[8];

    (void)ctx;
    if ((pdu == NULL) || (len < 2u) || (pdu[0] != 0x01u)) {
        return;
    }
    /* Mode 01 PID: RPM 0x0C, speed 0x0D */
    r[0] = 0x41u;
    r[1] = pdu[1];
    if (pdu[1] == 0x0Cu) {
        r[2] = 0x1Fu;
        r[3] = 0x40u; /* 2000 rpm proto */
        (void)obd_tx(r, 4u);
    } else if (pdu[1] == 0x0Du) {
        r[2] = 42u;
        (void)obd_tx(r, 3u);
    }
}

ae_status_t p02_ble_obd_init(void)
{
    isotp_cfg_t iso = {AE_CAN_UDS_RESP, AE_CAN_OBD_FUNC, 0u, 0u};

    (void)isotp_init(&iso, can_svc_send, obd_on_pdu, NULL);
    (void)ble_auto_init(NULL, NULL);
    s_obd_n = 0u;
    return AE_OK;
}

ae_status_t p02_ble_obd_run(void)
{
    uint8_t req[2] = {0x01u, 0x0Du};
    uint8_t ble[4];

    (void)p02_ble_obd_init();
    obd_on_pdu(NULL, 0u, NULL);
    {
        uint8_t rpm[2] = {0x01u, 0x0Cu};
        uint8_t unk[2] = {0x01u, 0x00u};
        obd_on_pdu(rpm, 2u, NULL);
        obd_on_pdu(unk, 2u, NULL);
    }
    obd_on_pdu(req, 2u, NULL);
    ble[0] = 0x41u;
    ble[1] = 0x0Du;
    ble[2] = 42u;
    (void)ble_auto_notify(AE_BLE_CHAR_VEHICLE_STATUS, ble, 3u);
    return (s_obd_n >= 3u) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}

ae_status_t p11_sensor_gw_init(void)
{
    uint8_t i2c[2] = {0x12u, 0x34u};

    (void)hal_adc_set_sim(0u, 3300u);
    (void)hal_i2c_set_sim(0x48u, i2c, sizeof(i2c));
    return can_svc_init();
}

ae_status_t p11_sensor_gw_run(void)
{
    uint16_t mv = 0u;
    uint8_t i2c[2];
    ae_can_frame_t f;

    (void)p11_sensor_gw_init();
    (void)hal_adc_read(0u, &mv);
    (void)hal_i2c_read(0x48u, i2c, sizeof(i2c), 10u);
    memset(&f, 0, sizeof(f));
    f.id = AE_CAN_SENSOR;
    f.dlc = 8u;
    f.data[0] = (uint8_t)(mv >> 8);
    f.data[1] = (uint8_t)(mv & 0xFFu);
    f.data[2] = i2c[0];
    f.data[3] = i2c[1];
    (void)can_svc_send(&f);
    (void)ble_auto_notify(AE_BLE_CHAR_SENSOR, f.data, 4u);
    return (mv == 3300u) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}

ae_status_t p12_tpms_gw_init(void)
{
    return can_svc_init();
}

ae_status_t p12_tpms_gw_run(void)
{
    /* BLE advert payload proto: kPa FL/FR/RL/RR */
    uint8_t adv[4] = {220u, 218u, 221u, 219u};
    ae_can_frame_t f;

    (void)p12_tpms_gw_init();
    memset(&f, 0, sizeof(f));
    f.id = AE_CAN_TPMS;
    f.dlc = 8u;
    memcpy(f.data, adv, 4u);
    (void)can_svc_send(&f);
    (void)ble_auto_notify(AE_BLE_CHAR_TPMS, adv, 4u);
    return AE_OK;
}

ae_status_t p13_ev_batt_gw_init(void)
{
    ecu_models_init();
    (void)dtc_init();
    return can_svc_init();
}

ae_status_t p13_ev_batt_gw_run(void)
{
    ae_can_frame_t st;
    ae_can_frame_t tmp;
    uint8_t ble[4];
    uint8_t i;

    (void)p13_ev_batt_gw_init();
    (void)bms_set_temp(40, 60);
    (void)bms_fill_status(&st);
    (void)bms_fill_temp(&tmp);
    (void)can_svc_send(&st);
    (void)can_svc_send(&tmp);
    for (i = 0; i < 3u; i++) {
        (void)dtc_raise(AE_DTC_BMS_OTEMP, DTC_SEV_CRIT);
    }
    ble[0] = bms_get()->soc;
    ble[1] = (uint8_t)bms_get()->tmax;
    ble[2] = (uint8_t)(dtc_confirmed(AE_DTC_BMS_OTEMP) ? 1u : 0u);
    (void)ble_auto_notify(AE_BLE_CHAR_BATT_STATUS, ble, 3u);
    return dtc_confirmed(AE_DTC_BMS_OTEMP) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}
