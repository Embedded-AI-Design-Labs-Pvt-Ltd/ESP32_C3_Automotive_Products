/**
 * @file test_all_products.c
 * @brief Run every product use-case. Setup/Stimulus live inside each *_run().
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include <stdio.h>

#include "ae_error.h"
#include "product_api.h"

int main(void)
{
    uint8_t n = 0u;
    uint8_t i;
    int fail = 0;
    const ae_product_desc_t *tab = ae_products_table(&n);

    printf("AEGW-C3 product use-cases: %u\n", (unsigned)n);
    for (i = 0; i < n; i++) {
        ae_status_t st = tab[i].run();
        if (ae_err_is_ok(st)) {
            printf("PASS %s %s\n", tab[i].id, tab[i].name);
        } else {
            printf("FAIL %s %s status=%ld\n", tab[i].id, tab[i].name, (long)st);
            fail = 1;
        }
    }
    printf(fail ? "RESULT FAIL\n" : "RESULT PASS\n");
    return fail;
}
