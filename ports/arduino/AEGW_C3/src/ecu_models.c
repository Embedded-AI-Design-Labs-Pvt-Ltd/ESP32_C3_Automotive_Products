/**
 * @file ecu_models.c
 * @brief Simulated BCM / VCU / BMS on prototype CAN IDs.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "ecu_models.h"

#include "ae_can_ids.h"
#include "ae_error.h"

#include <string.h>

static bcm_state_t s_bcm;
static vcu_state_t s_vcu;
static bms_state_t s_bms;

static uint8_t cksum(const uint8_t *p, uint8_t n)
{
    uint8_t s = 0u;
    uint8_t i;

    for (i = 0; i < n; i++) {
        s = (uint8_t)(s + p[i]);
    }
    return (uint8_t)(0xFFu - s);
}

void ecu_models_init(void)
{
    memset(&s_bcm, 0, sizeof(s_bcm));
    memset(&s_vcu, 0, sizeof(s_vcu));
    memset(&s_bms, 0, sizeof(s_bms));
    s_bcm.ign = 1u;
    s_vcu.ready = 1u;
    s_vcu.mode = 1u;
    s_bms.pack_v = 3800u;
    s_bms.soc = 80u;
    s_bms.soh = 95u;
    s_bms.tmin = 25;
    s_bms.tmax = 28;
    s_bms.lim_dch = 200u;
}

ae_status_t bcm_fill_status(ae_can_frame_t *f)
{
    if (f == NULL) {
        return ae_err_make(AE_MOD_APP, 1u);
    }
    memset(f, 0, sizeof(*f));
    f->id = AE_CAN_BCM_STATUS;
    f->dlc = 8u;
    f->data[0] = (uint8_t)((s_bcm.ign & 0x03u) | ((s_bcm.doors & 0x0Fu) << 2));
    f->data[1] = s_bcm.lights;
    f->data[2] = s_bcm.speed;
    s_bcm.counter++;
    f->data[3] = (uint8_t)(s_bcm.counter >> 8);
    f->data[4] = (uint8_t)(s_bcm.counter & 0xFFu);
    f->data[5] = cksum(f->data, 5u);
    return AE_OK;
}

ae_status_t bcm_on_light_cmd(const ae_can_frame_t *cmd, ae_can_frame_t *status)
{
    if ((cmd == NULL) || (status == NULL) || (cmd->id != AE_CAN_BCM_LIGHT_CMD)) {
        return ae_err_make(AE_MOD_APP, 1u);
    }
    s_bcm.lights = cmd->data[0];
    memset(status, 0, sizeof(*status));
    status->id = AE_CAN_BCM_LIGHT_ST;
    status->dlc = 8u;
    status->data[0] = s_bcm.lights;
    return AE_OK;
}

ae_status_t vcu_fill_hb(ae_can_frame_t *f)
{
    if (f == NULL) {
        return ae_err_make(AE_MOD_APP, 1u);
    }
    memset(f, 0, sizeof(*f));
    f->id = AE_CAN_VCU_HB;
    f->dlc = 8u;
    f->data[0] = s_vcu.ready;
    f->data[1] = s_vcu.mode;
    return AE_OK;
}

ae_status_t vcu_fill_torque(ae_can_frame_t *f, int16_t torque)
{
    if (f == NULL) {
        return ae_err_make(AE_MOD_APP, 1u);
    }
    if (s_bms.lim_dch == 0u) {
        torque = 0;
    }
    s_vcu.torque = torque;
    s_vcu.counter++;
    memset(f, 0, sizeof(*f));
    f->id = AE_CAN_VCU_TORQUE;
    f->dlc = 8u;
    f->data[0] = (uint8_t)((uint16_t)torque >> 8);
    f->data[1] = (uint8_t)((uint16_t)torque & 0xFFu);
    f->data[2] = s_vcu.brake;
    f->data[3] = (uint8_t)(s_vcu.counter & 0xFFu);
    f->data[4] = cksum(f->data, 4u);
    return AE_OK;
}

ae_status_t bms_fill_status(ae_can_frame_t *f)
{
    if (f == NULL) {
        return ae_err_make(AE_MOD_APP, 1u);
    }
    memset(f, 0, sizeof(*f));
    f->id = AE_CAN_BMS_STATUS;
    f->dlc = 8u;
    f->data[0] = (uint8_t)(s_bms.pack_v >> 8);
    f->data[1] = (uint8_t)(s_bms.pack_v & 0xFFu);
    f->data[2] = s_bms.soc;
    f->data[3] = s_bms.soh;
    return AE_OK;
}

ae_status_t bms_fill_temp(ae_can_frame_t *f)
{
    if (f == NULL) {
        return ae_err_make(AE_MOD_APP, 1u);
    }
    memset(f, 0, sizeof(*f));
    f->id = AE_CAN_BMS_TEMP;
    f->dlc = 8u;
    f->data[0] = (uint8_t)s_bms.tmin;
    f->data[1] = (uint8_t)s_bms.tmax;
    return AE_OK;
}

ae_status_t bms_set_temp(int8_t tmin, int8_t tmax)
{
    s_bms.tmin = tmin;
    s_bms.tmax = tmax;
    if (tmax > 55) {
        s_bms.lim_dch = 0u;
    }
    return AE_OK;
}

const bcm_state_t *bcm_get(void)
{
    return &s_bcm;
}

const vcu_state_t *vcu_get(void)
{
    return &s_vcu;
}

const bms_state_t *bms_get(void)
{
    return &s_bms;
}
