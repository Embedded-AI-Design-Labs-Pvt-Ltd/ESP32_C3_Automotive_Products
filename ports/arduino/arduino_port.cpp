/**
 * @file arduino_port.cpp
 * @brief Arduino shim: same products, Arduino HAL later replaces hal_host.c.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#include "product_api.h"
#include "ae_error.h"

extern "C" ae_status_t arduino_run_product(uint8_t index)
{
    uint8_t n = 0u;
    const ae_product_desc_t *tab = ae_products_table(&n);

    if (index >= n) {
        return ae_err_make(AE_MOD_APP, 1u);
    }
    return tab[index].run();
}
