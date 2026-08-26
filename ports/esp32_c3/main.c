/**
 * @file main.c
 * @brief ESP32-C3 product selector. Host builds use AE_HOST.
 */

#include "ae_types.h"
#include "hal_misc.h"
#include "product_api.h"

#ifndef AE_PRODUCT_INDEX
#define AE_PRODUCT_INDEX 0
#endif

#ifdef AE_HOST
int ae_app_run_selected(void)
#else
void app_main(void)
#endif
{
    uint8_t n = 0u;
    const ae_product_desc_t *tab = ae_products_table(&n);
    uint8_t idx = (uint8_t)AE_PRODUCT_INDEX;

    (void)hal_wdg_kick();
    if (idx >= n) {
        idx = 0u;
    }
    (void)tab[idx].init();
#ifndef AE_HOST
    for (;;) {
        (void)tab[idx].run();
        (void)hal_wdg_kick();
    }
#else
    return tab[idx].run();
#endif
}
