/**
 * @file test_crc_e2e.c
 * @brief Unit tests for CRC/E2E (REQ-LAB-012).
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 */
#include "crc_e2e.h"

#include <stdio.h>
#include <string.h>

static int g_fail;

static void expect_true(const char *id, int cond)
{
    if (!cond) {
        printf("FAIL %s\n", id);
        g_fail = 1;
    } else {
        printf("PASS %s\n", id);
    }
}

int main(void)
{
    uint8_t d[8];
    uint8_t ctr = 0u;
    ae_e2e_state_t st;
    uint8_t empty[1] = {0};

    g_fail = 0;
    (void)empty;

    /* TC-E2E-001: CRC8 deterministic */
    {
        const uint8_t p[] = {0x01, 0x02, 0x03};
        uint8_t a = ae_crc8_j1850(p, 3u);
        uint8_t b = ae_crc8_j1850(p, 3u);
        expect_true("TC-E2E-001_crc8_stable", a == b);
    }

    /* TC-E2E-002: protect + check OK */
    memset(d, 0, sizeof(d));
    d[0] = 1u;
    d[1] = 42u;
    memset(&st, 0, sizeof(st));
    st.timeout_ms = 200u;
    expect_true("TC-E2E-002_protect", ae_e2e_protect(0x1A0u, d, &ctr) == AE_E2E_OK);
    expect_true("TC-E2E-002_check", ae_e2e_check(0x1A0u, d, 8u, &st, 10u) == AE_E2E_OK);

    /* TC-E2E-003: corrupted CRC */
    expect_true("TC-E2E-003_protect2", ae_e2e_protect(0x1A0u, d, &ctr) == AE_E2E_OK);
    d[6] ^= 0xFFu;
    expect_true("TC-E2E-003_bad_crc", ae_e2e_check(0x1A0u, d, 8u, &st, 20u) == AE_E2E_CRC);

    /* TC-E2E-004: wrong counter */
    memset(d, 0, sizeof(d));
    ctr = 0u;
    memset(&st, 0, sizeof(st));
    ae_e2e_protect(0x200u, d, &ctr);
    ae_e2e_check(0x200u, d, 8u, &st, 0u);
    ae_e2e_protect(0x200u, d, &ctr);
    d[7] = (uint8_t)((d[7] & 0x0Fu) | (0x5u << 4)); /* force wrong nibble */
    /* recompute would fail CRC anyway — force counter fail after fixing CRC */
    {
        uint8_t tmp[7];
        tmp[0] = (uint8_t)(0x200u & 0xFFu);
        memcpy(&tmp[1], d, 6u);
        d[6] = ae_crc8_j1850(tmp, 7u);
    }
    expect_true("TC-E2E-004_bad_ctr", ae_e2e_check(0x200u, d, 8u, &st, 5u) == AE_E2E_COUNTER);

    /* TC-E2E-005: timeout */
    memset(d, 0, sizeof(d));
    ctr = 0u;
    memset(&st, 0, sizeof(st));
    st.timeout_ms = 50u;
    ae_e2e_protect(0x300u, d, &ctr);
    ae_e2e_check(0x300u, d, 8u, &st, 0u);
    ae_e2e_protect(0x300u, d, &ctr);
    expect_true("TC-E2E-005_timeout", ae_e2e_check(0x300u, d, 8u, &st, 100u) == AE_E2E_TIMEOUT);

    /* TC-E2E-006: DLC */
    expect_true("TC-E2E-006_dlc", ae_e2e_check(0x300u, d, 4u, &st, 100u) == AE_E2E_DLC);

    /* TC-E2E-007: CRC16/32 smoke */
    expect_true("TC-E2E-007_crc16", ae_crc16_ccitt((const uint8_t *)"A", 1u) != 0u);
    expect_true("TC-E2E-007_crc32", ae_crc32((const uint8_t *)"ABC", 3u) != 0u);

    printf(g_fail ? "RESULT FAIL\n" : "RESULT PASS\n");
    return g_fail;
}
