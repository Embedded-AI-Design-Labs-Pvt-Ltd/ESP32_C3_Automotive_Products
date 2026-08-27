/**
 * @file can_service.c
 * @brief Cycle-time supervision and fan-out. No TWAI registers here.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "can_service.h"

#include "ae_error.h"
#include "hal_can.h"

#include <string.h>

#define AE_SUB_MAX 12u
#define AE_EXP_MAX 12u

typedef struct {
    uint32_t id;
    can_svc_rx_cb_t cb;
    void *ctx;
    int used;
} ae_sub_t;

typedef struct {
    uint32_t id;
    uint32_t timeout_ms;
    uint32_t last_ms;
    int armed;
    int missing;
} ae_exp_t;

static ae_sub_t s_sub[AE_SUB_MAX];
static ae_exp_t s_exp[AE_EXP_MAX];
static can_svc_stats_t s_st;
static uint32_t s_now;

ae_status_t can_svc_init(void)
{
    memset(s_sub, 0, sizeof(s_sub));
    memset(s_exp, 0, sizeof(s_exp));
    memset(&s_st, 0, sizeof(s_st));
    s_now = 0u;
    return AE_OK;
}

ae_status_t can_svc_send(const ae_can_frame_t *frame)
{
    ae_status_t st;

    if (frame == NULL) {
        return ae_err_make(AE_MOD_CAN, 1u);
    }
    st = hal_can_send(frame, 10u);
    if (ae_err_is_ok(st)) {
        s_st.tx++;
    } else {
        s_st.drop++;
    }
    return st;
}

ae_status_t can_svc_subscribe(uint32_t id, can_svc_rx_cb_t cb, void *ctx)
{
    uint8_t i;

    for (i = 0; i < AE_SUB_MAX; i++) {
        if (s_sub[i].used == 0) {
            s_sub[i].id = id;
            s_sub[i].cb = cb;
            s_sub[i].ctx = ctx;
            s_sub[i].used = 1;
            return AE_OK;
        }
    }
    return ae_err_make(AE_MOD_CAN, 2u);
}

ae_status_t can_svc_expect(uint32_t id, uint32_t period_ms, uint32_t timeout_ms)
{
    uint8_t i;

    (void)period_ms;
    for (i = 0; i < AE_EXP_MAX; i++) {
        if (s_exp[i].armed == 0) {
            s_exp[i].id = id;
            s_exp[i].timeout_ms = timeout_ms;
            s_exp[i].last_ms = s_now;
            s_exp[i].armed = 1;
            s_exp[i].missing = 0;
            return AE_OK;
        }
    }
    return ae_err_make(AE_MOD_CAN, 2u);
}

ae_status_t can_svc_on_rx(const ae_can_frame_t *frame)
{
    uint8_t i;
    int known = 0;

    if (frame == NULL) {
        return ae_err_make(AE_MOD_CAN, 1u);
    }
    s_st.rx++;
    for (i = 0; i < AE_EXP_MAX; i++) {
        if ((s_exp[i].armed != 0) && (s_exp[i].id == frame->id)) {
            s_exp[i].last_ms = s_now;
            s_exp[i].missing = 0;
            known = 1;
        }
    }
    for (i = 0; i < AE_SUB_MAX; i++) {
        if ((s_sub[i].used != 0) && (s_sub[i].id == frame->id) && (s_sub[i].cb != NULL)) {
            s_sub[i].cb(frame, s_sub[i].ctx);
            known = 1;
        }
    }
    if (known == 0) {
        s_st.unexpected++;
    }
    return AE_OK;
}

ae_status_t can_svc_tick(uint32_t now_ms)
{
    uint8_t i;

    s_now = now_ms;
    for (i = 0; i < AE_EXP_MAX; i++) {
        if ((s_exp[i].armed != 0) && ((now_ms - s_exp[i].last_ms) > s_exp[i].timeout_ms)) {
            if (s_exp[i].missing == 0) {
                s_st.missing++;
            }
            s_exp[i].missing = 1;
        }
    }
    return AE_OK;
}

ae_status_t can_svc_get_stats(can_svc_stats_t *out)
{
    if (out == NULL) {
        return ae_err_make(AE_MOD_CAN, 1u);
    }
    *out = s_st;
    return AE_OK;
}

int can_svc_id_missing(uint32_t id)
{
    uint8_t i;

    for (i = 0; i < AE_EXP_MAX; i++) {
        if ((s_exp[i].armed != 0) && (s_exp[i].id == id)) {
            return s_exp[i].missing;
        }
    }
    return 0;
}
