/**
 * @file virtual_ecu.cpp
 * @brief Host-side ECU cluster for GUI-without-hardware.
 */

#include "virtual_ecu.hpp"

#include "ae_can_ids.h"
#include "can_service.h"
#include "dtc.h"
#include "ecu_models.h"

VirtualEcu::VirtualEcu()
{
    (void)reset();
}

ae_status_t VirtualEcu::reset()
{
    ecu_models_init();
    (void)dtc_init();
    return can_svc_init();
}

ae_status_t VirtualEcu::tick(uint32_t now_ms)
{
    ae_can_frame_t f;

    (void)bcm_fill_status(&f);
    (void)can_svc_send(&f);
    (void)vcu_fill_hb(&f);
    (void)can_svc_send(&f);
    (void)bms_fill_status(&f);
    (void)can_svc_send(&f);
    return can_svc_tick(now_ms);
}

ae_status_t VirtualEcu::inject_overtemp()
{
    return bms_set_temp(40, 60);
}

extern "C" ae_status_t virtual_ecu_run_demo(void)
{
    VirtualEcu ecu;
    ae_status_t st = ecu.tick(0u);

    if (st != AE_OK) {
        return st;
    }
    (void)ecu.inject_overtemp();
    return AE_OK;
}
