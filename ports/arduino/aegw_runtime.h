/**
 * @file aegw_runtime.h
 * @brief Real-time product loop for Arduino IDE / ESP32-C3.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#ifndef AEGW_RUNTIME_H
#define AEGW_RUNTIME_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void aegw_runtime_setup(void);
void aegw_runtime_loop(void);
ae_status_t aegw_runtime_select(uint8_t product_index);
uint8_t aegw_runtime_index(void);
/** Set index without init. setup() clamps if out of range. */
void aegw_runtime_bind(uint8_t product_index);

#ifdef __cplusplus
}
#endif

#endif /* AEGW_RUNTIME_H */
