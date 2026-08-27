/**
 * @file ecu_models.h
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#ifndef ECU_MODELS_H
#define ECU_MODELS_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t ign;
    uint8_t doors;
    uint8_t lights;
    uint8_t speed;
    uint16_t counter;
} bcm_state_t;

typedef struct {
    uint8_t ready;
    uint8_t mode;
    int16_t torque;
    uint8_t brake;
    uint16_t counter;
} vcu_state_t;

typedef struct {
    uint16_t pack_v;
    int16_t pack_i;
    uint8_t soc;
    uint8_t soh;
    int8_t tmin;
    int8_t tmax;
    uint16_t lim_chg;
    uint16_t lim_dch;
} bms_state_t;

void ecu_models_init(void);
ae_status_t bcm_fill_status(ae_can_frame_t *f);
ae_status_t bcm_on_light_cmd(const ae_can_frame_t *cmd, ae_can_frame_t *status);
ae_status_t vcu_fill_hb(ae_can_frame_t *f);
ae_status_t vcu_fill_torque(ae_can_frame_t *f, int16_t torque);
ae_status_t bms_fill_status(ae_can_frame_t *f);
ae_status_t bms_fill_temp(ae_can_frame_t *f);
ae_status_t bms_set_temp(int8_t tmin, int8_t tmax);
const bcm_state_t *bcm_get(void);
const vcu_state_t *vcu_get(void);
const bms_state_t *bms_get(void);

#ifdef __cplusplus
}
#endif

#endif /* ECU_MODELS_H */
