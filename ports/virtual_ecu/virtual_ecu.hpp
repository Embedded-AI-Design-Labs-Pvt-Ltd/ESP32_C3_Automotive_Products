/**
 * @file virtual_ecu.hpp
 * @brief C++ Virtual ECU: same CAN models, no ESP32 hardware.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#ifndef VIRTUAL_ECU_HPP
#define VIRTUAL_ECU_HPP

#include "ae_types.h"

#ifdef __cplusplus

class VirtualEcu {
public:
    VirtualEcu();
    ae_status_t reset();
    ae_status_t tick(uint32_t now_ms);
    ae_status_t inject_overtemp();
};

extern "C" {
#endif

ae_status_t virtual_ecu_run_demo(void);

#ifdef __cplusplus
}
#endif

#endif /* VIRTUAL_ECU_HPP */
