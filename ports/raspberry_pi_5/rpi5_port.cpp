/**
 * @file rpi5_port.cpp
 * @brief Raspberry Pi 5 shim: SocketCAN later; products unchanged.
 */
#include "product_api.h"
#include "ae_error.h"

extern "C" ae_status_t rpi5_run_product(uint8_t index)
{
    uint8_t n = 0u;
    const ae_product_desc_t *tab = ae_products_table(&n);

    if (index >= n) {
        return ae_err_make(AE_MOD_APP, 1u);
    }
    return tab[index].run();
}
