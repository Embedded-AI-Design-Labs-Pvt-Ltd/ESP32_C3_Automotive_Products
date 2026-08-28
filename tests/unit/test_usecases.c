/**
 * @file test_usecases.c
 * @brief Run all 120 automotive/cyber lab use cases.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 */
#include "usecases.h"

#include <stdio.h>

int main(void)
{
    uint16_t n = 0u;
    uint16_t failed = 0u;
    uint16_t passed = 0u;
    const ae_usecase_desc_t *tab = ae_usecases_table(&n);
    ae_status_t st;

    if ((tab == NULL) || (n != AE_UC_COUNT)) {
        printf("FAIL table size %u\n", (unsigned)n);
        return 1;
    }
    printf("AEGW-C3 use cases: %u\n", (unsigned)n);
    st = ae_usecases_run_all(&failed, &passed);
    if (st != AE_OK) {
        printf("FAIL UC-%u after %u passed\n", (unsigned)failed, (unsigned)passed);
        return 1;
    }
    printf("PASS all %u use cases\n", (unsigned)passed);
    printf("RESULT PASS\n");
    return 0;
}
