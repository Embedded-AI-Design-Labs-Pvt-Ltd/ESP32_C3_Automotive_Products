/**
 * @file fault_mgr.c
 * @brief NORMAL → DETECTED → DEGRADED/RECOVERY → SAFE.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "fault_mgr.h"

static fault_state_t s_st;
static uint8_t s_budget;

ae_status_t fault_init(void)
{
    s_st = FAULT_NORMAL;
    s_budget = 3u;
    return AE_OK;
}

ae_status_t fault_report(uint32_t id, int critical)
{
    (void)id;
    s_st = FAULT_DETECTED;
    if (critical == 0) {
        s_st = FAULT_DEGRADED;
        return AE_OK;
    }
    if (s_budget == 0u) {
        s_st = FAULT_SAFE;
        return AE_OK;
    }
    s_st = FAULT_RECOVERY;
    s_budget--;
    return AE_OK;
}

ae_status_t fault_recover(void)
{
    if (s_st == FAULT_SAFE) {
        return AE_OK;
    }
    s_st = FAULT_NORMAL;
    return AE_OK;
}

fault_state_t fault_get_state(void)
{
    return s_st;
}
