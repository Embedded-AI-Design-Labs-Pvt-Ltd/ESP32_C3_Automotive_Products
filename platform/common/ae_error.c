/**
 * @file ae_error.c
 * @brief Compose and split 16-bit style status codes.
 */

#include "ae_error.h"

/**
 * @brief Build a status from module and reason.
 * @param[in] module Module identifier (low 8 bits used).
 * @param[in] reason Reason identifier.
 * @return Packed status, or AE_OK if both inputs are 0.
 * @thread Any.
 * @isr_safe Yes (no locks).
 */
ae_status_t ae_err_make(ae_status_t module, uint8_t reason)
{
    /* Keep only the module number in the high byte of the low 16 bits. */
    const ae_status_t mod = module & 0xFFu;
    if ((mod == 0) && (reason == 0u)) {
        return AE_OK;
    }
    return AE_ERR(mod, reason);
}

/**
 * @brief Extract the module field.
 */
ae_status_t ae_err_module(ae_status_t status)
{
    return (status >> 8) & 0xFFu;
}

/**
 * @brief Extract the reason field.
 */
uint8_t ae_err_reason(ae_status_t status)
{
    return (uint8_t)(status & 0xFFu);
}

/**
 * @brief True when status is AE_OK.
 */
int ae_err_is_ok(ae_status_t status)
{
    return status == AE_OK;
}
