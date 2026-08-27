/**
 * @file arduino_port.hpp
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#ifndef ARDUINO_PORT_HPP
#define ARDUINO_PORT_HPP

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

ae_status_t arduino_run_product(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif
