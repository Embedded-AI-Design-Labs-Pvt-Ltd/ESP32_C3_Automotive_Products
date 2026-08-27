/**
 * @file ae_error.h
 * @brief Error helpers. Thread-safe (no shared state).
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 * @context Any task. Not required from ISR.
 */
#ifndef AE_ERROR_H
#define AE_ERROR_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

ae_status_t ae_err_make(ae_status_t module, uint8_t reason);
ae_status_t ae_err_module(ae_status_t status);
uint8_t ae_err_reason(ae_status_t status);
int ae_err_is_ok(ae_status_t status);

#ifdef __cplusplus
}
#endif

#endif /* AE_ERROR_H */
