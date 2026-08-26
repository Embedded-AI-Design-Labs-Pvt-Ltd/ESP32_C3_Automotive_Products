/**
 * @file test_host.c
 * @brief Host unit tests: error codes and ring buffer.
 *
 * Setup / Stimulus / Expected / Timeout / Pass-fail / Cleanup
 * Timeout is not used on host (runs to completion).
 */

#include <stdio.h>
#include <string.h>

#include "ae_error.h"
#include "ae_ring.h"

static int g_fail;

static void expect(int cond, const char *name)
{
    if (!cond) {
        printf("FAIL %s\n", name);
        g_fail = 1;
    } else {
        printf("PASS %s\n", name);
    }
}

static void test_error(void)
{
    ae_status_t st;

    /* Setup: none. Stimulus: compose a CAN timeout-style code. */
    st = ae_err_make(AE_MOD_CAN, 4u);
    expect(ae_err_is_ok(AE_OK), "error_ok");
    expect(!ae_err_is_ok(st), "error_nonzero");
    expect(ae_err_module(st) == AE_MOD_CAN, "error_module");
    expect(ae_err_reason(st) == 4u, "error_reason");
}

static void test_ring(void)
{
    ae_ring_t ring;
    uint8_t storage[8];
    uint8_t b;
    size_t i;

    memset(storage, 0, sizeof(storage));
    expect(ae_ring_init(&ring, storage, sizeof(storage)) == AE_OK, "ring_init");

    /* Stimulus: fill until drop. Capacity-1 usable slots. */
    for (i = 0; i < (sizeof(storage) - 1u); i++) {
        expect(ae_ring_put(&ring, (uint8_t)i) == AE_OK, "ring_put");
    }
    expect(ae_ring_put(&ring, 0xFFu) != AE_OK, "ring_full");
    expect(ae_ring_drops(&ring) == 1u, "ring_drop");
    expect(ae_ring_count(&ring) == (sizeof(storage) - 1u), "ring_count");

    expect(ae_ring_get(&ring, &b) == AE_OK, "ring_get");
    expect(b == 0u, "ring_order");
}

int main(void)
{
    g_fail = 0;
    test_error();
    test_ring();
    if (g_fail != 0) {
        printf("RESULT FAIL\n");
        return 1;
    }
    printf("RESULT PASS\n");
    return 0;
}
