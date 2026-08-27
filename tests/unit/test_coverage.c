/**
 * @file test_coverage.c
 * @brief Branch coverage for every public C module (Setup/Stimulus/Expect).
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include <stdio.h>
#include <string.h>

#include "ae_can_ids.h"
#include "ae_error.h"
#include "ae_ring.h"
#include "aegw_runtime.h"
#include "ble_auto.h"
#include "can_service.h"
#include "dtc.h"
#include "ecu_models.h"
#include "fault_mgr.h"
#include "hal_can.h"
#include "hal_misc.h"
#include "isotp.h"
#include "ota_agent.h"
#include "product_api.h"
#include "product_dids.h"
#include "uds.h"

static int g_fail;
static uint8_t g_pdu[160];
static uint16_t g_pdu_n;
static ae_can_frame_t g_tx[16];
static uint8_t g_txn;
static int g_cb;

static void expect(int cond, const char *name)
{
    if (!cond) {
        printf("FAIL %s\n", name);
        g_fail = 1;
    }
}

static ae_status_t capture_tx(const uint8_t *p, uint16_t n)
{
    if (n > sizeof(g_pdu)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    memcpy(g_pdu, p, n);
    g_pdu_n = n;
    return AE_OK;
}

static ae_status_t capture_can(const ae_can_frame_t *f)
{
    if (g_txn < 16u) {
        g_tx[g_txn++] = *f;
    }
    return AE_OK;
}

static void on_pdu(const uint8_t *p, uint16_t n, void *c)
{
    (void)c;
    g_cb = 1;
    memcpy(g_pdu, p, n);
    g_pdu_n = n;
}

static void on_can_isr(const ae_can_frame_t *f, void *c)
{
    (void)f;
    (void)c;
    g_cb = 1;
}

static ae_status_t did_fail(uint16_t d, uint8_t *o, uint16_t *l)
{
    (void)d;
    (void)o;
    (void)l;
    return ae_err_make(AE_MOD_UDS, 9u);
}

static ae_status_t wr_fail(uint16_t d, const uint8_t *i, uint16_t n)
{
    (void)d;
    (void)i;
    (void)n;
    return ae_err_make(AE_MOD_UDS, 9u);
}

static ae_status_t rc_fail(uint16_t r, uint8_t op, uint8_t *o, uint16_t *l)
{
    (void)r;
    (void)op;
    (void)o;
    (void)l;
    return ae_err_make(AE_MOD_UDS, 9u);
}

static void test_error_ring(void)
{
    ae_ring_t ring;
    uint8_t st[4];
    uint8_t b;

    expect(ae_err_make(0, 0) == AE_OK, "err0");
    {
        ae_status_t st = ae_err_make(AE_MOD_CAN, 4u);
        expect(ae_err_module(st) == AE_MOD_CAN, "emod");
        expect(ae_err_reason(st) == 4u, "ereason");
        expect(ae_err_is_ok(AE_OK) && !ae_err_is_ok(st), "eok");
    }
    expect(ae_ring_init(NULL, st, 4) != AE_OK, "ring_null");
    expect(ae_ring_init(&ring, NULL, 4) != AE_OK, "ring_stor");
    expect(ae_ring_init(&ring, st, 1) != AE_OK, "ring_cap");
    expect(ae_ring_init(&ring, st, 4) == AE_OK, "ring_ok");
    expect(ae_ring_put(NULL, 1) != AE_OK, "put_null");
    expect(ae_ring_get(NULL, &b) != AE_OK, "get_nullr");
    expect(ae_ring_get(&ring, NULL) != AE_OK, "get_nullb");
    expect(ae_ring_get(&ring, &b) != AE_OK, "get_empty");
    expect(ae_ring_count(NULL) == 0, "cnt_null");
    expect(ae_ring_drops(NULL) == 0, "drop_null");
    expect(ae_ring_put(&ring, 1) == AE_OK, "put1");
    expect(ae_ring_put(&ring, 2) == AE_OK, "put2");
    expect(ae_ring_put(&ring, 3) == AE_OK, "put3");
    expect(ae_ring_put(&ring, 4) != AE_OK, "put_full");
    expect(ae_ring_drops(&ring) == 1, "drops");
    expect(ae_ring_get(&ring, &b) == AE_OK && b == 1, "wrap_get");
    expect(ae_ring_put(&ring, 9) == AE_OK, "wrap_put");
    expect(ae_ring_count(&ring) == 3, "wrap_cnt");
    expect(ae_ring_get(&ring, &b) == AE_OK && b == 2, "g2");
    expect(ae_ring_get(&ring, &b) == AE_OK && b == 3, "g3");
    expect(ae_ring_get(&ring, &b) == AE_OK && b == 9, "gwrap");
    expect(ae_ring_count(&ring) == 0, "empty_cnt");
}

static void test_hal(void)
{
    ae_can_cfg_t cc = {500000};
    ae_uart_cfg_t uc = {115200};
    ae_gpio_cfg_t gc = {1, 1};
    ae_can_filter_t flt = {0x1A0, 0x7FF};
    ae_can_frame_t f;
    uint8_t lvl = 0;
    uint16_t mv = 0;
    uint8_t buf[8];
    uint8_t i2c[2] = {1, 2};
    size_t n;
    uint8_t i;

    hal_host_reset();
    expect(hal_can_init(&cc) == AE_OK, "can_init");
    expect(hal_can_set_filter(&flt, 1) == AE_OK, "filt");
    expect(hal_can_attach_rx(on_can_isr, NULL) == AE_OK, "isr");
    expect(hal_can_send(NULL, 0) != AE_OK, "tx_null");
    expect(hal_can_recv(NULL) != AE_OK, "rx_null");
    expect(hal_can_recv(&f) != AE_OK, "rx_empty");
    memset(&f, 0, sizeof(f));
    f.id = 0x1A0;
    f.dlc = 1;
    g_cb = 0;
    expect(hal_can_send(&f, 10) == AE_OK && g_cb == 1, "tx_cb");
    expect(hal_can_bus_count() >= 1, "bus_cnt");
    expect(hal_can_recv(&f) == AE_OK, "rx_one");
    expect(hal_uart_init(&uc) == AE_OK, "uart");
    expect(hal_uart_write(NULL, 1, 0) != AE_OK, "uart_bad");
    expect(hal_uart_write((const uint8_t *)"x", 1, 0) == AE_OK, "uart_w");
    expect(hal_uart_write(NULL, 0, 0) == AE_OK, "uart_z");
    expect(hal_gpio_init(&gc) == AE_OK, "gpio_i");
    expect(hal_gpio_write(99, 1) != AE_OK, "gpio_p");
    expect(hal_gpio_write(1, 1) == AE_OK, "gpio_w");
    expect(hal_gpio_read(99, &lvl) != AE_OK, "gpio_rb");
    expect(hal_gpio_read(1, NULL) != AE_OK, "gpio_rn");
    expect(hal_gpio_read(1, &lvl) == AE_OK && lvl == 1, "gpio_r");
    expect(hal_gpio_attach_isr(1, NULL, NULL) == AE_OK, "gpio_isr");
    expect(hal_i2c_set_sim(0x48, NULL, 1) != AE_OK, "i2c_s");
    expect(hal_i2c_set_sim(0x48, i2c, 2) == AE_OK, "i2c_ok");
    expect(hal_i2c_read(0x48, NULL, 2, 0) != AE_OK, "i2c_r");
    expect(hal_i2c_read(0x48, buf, 2, 0) == AE_OK, "i2c_rd");
    expect(hal_spi_xfer(NULL, buf, 1, 0) != AE_OK, "spi_b");
    expect(hal_spi_xfer(i2c, buf, 2, 0) == AE_OK, "spi");
    expect(hal_adc_set_sim(9, 1) != AE_OK, "adc_s");
    expect(hal_adc_set_sim(0, 1200) == AE_OK, "adc_ok");
    expect(hal_adc_read(9, &mv) != AE_OK, "adc_rb");
    expect(hal_adc_read(0, NULL) != AE_OK, "adc_rn");
    expect(hal_adc_read(0, &mv) == AE_OK && mv == 1200, "adc_r");
    expect(hal_nvs_set(NULL, buf, 1) != AE_OK, "nvs_s");
    expect(hal_nvs_get(NULL, buf, &n) != AE_OK, "nvs_g");
    n = 8;
    expect(hal_nvs_get("nope", buf, &n) != AE_OK, "nvs_miss");
    expect(hal_nvs_set("k", "ab", 2) == AE_OK, "nvs_set");
    n = 1;
    expect(hal_nvs_get("k", buf, &n) != AE_OK, "nvs_small");
    n = 8;
    expect(hal_nvs_get("k", buf, &n) == AE_OK && n == 2, "nvs_get");
    expect(hal_nvs_set("k", "xy", 2) == AE_OK, "nvs_upd");
    for (i = 0; i < 8; i++) {
        char key[8];
        sprintf(key, "n%u", (unsigned)i);
        (void)hal_nvs_set(key, "z", 1);
    }
    expect(hal_nvs_set("overflow", "z", 1) != AE_OK, "nvs_full");
    expect(hal_wdg_kick() == AE_OK && hal_wdg_kicks() >= 1, "wdg");
    (void)hal_millis();

    /* Fill software CAN ring (depth 64 → 63 frames). */
    {
        ae_can_cfg_t c2 = {500000};
        uint8_t k;
        (void)hal_can_init(&c2);
        memset(&f, 0, sizeof(f));
        f.id = 0x100;
        f.dlc = 1;
        for (k = 0; k < 63; k++) {
            expect(hal_can_send(&f, 0) == AE_OK, "fill");
        }
        expect(hal_can_send(&f, 0) != AE_OK, "can_full");
        expect(can_svc_init() == AE_OK, "cs_re");
        expect(can_svc_send(&f) != AE_OK, "cs_drop");
        (void)hal_can_init(&c2);
    }
}

static void test_can_iso_uds(void)
{
    isotp_cfg_t iso = {0x7E8, 0x7E0, 0, 0};
    ae_can_frame_t f;
    uint8_t big[20];
    uint8_t i;
    can_svc_stats_t st;
    uint8_t p[8];

    (void)can_svc_init();
    expect(can_svc_send(NULL) != AE_OK, "cs_null");
    expect(can_svc_on_rx(NULL) != AE_OK, "cs_rxn");
    expect(can_svc_get_stats(NULL) != AE_OK, "cs_stn");
    expect(can_svc_id_missing(0x111) == 0, "cs_unk");
    memset(&f, 0, sizeof(f));
    f.id = 0x123;
    f.dlc = 1;
    expect(can_svc_on_rx(&f) == AE_OK, "cs_unexp");
    expect(can_svc_get_stats(&st) == AE_OK && st.unexpected >= 1, "cs_stat");
    for (i = 0; i < 12; i++) {
        expect(can_svc_subscribe(0x200u + i, NULL, NULL) == AE_OK, "sub");
        expect(can_svc_expect(0x300u + i, 10, 20) == AE_OK, "exp");
    }
    expect(can_svc_subscribe(0x999, NULL, NULL) != AE_OK, "sub_full");
    expect(can_svc_expect(0x998, 10, 20) != AE_OK, "exp_full");
    (void)can_svc_init();
    (void)can_svc_expect(0x1A0, 100, 50);
    (void)can_svc_tick(0);
    (void)can_svc_tick(80);
    expect(can_svc_id_missing(0x1A0) == 1, "miss");
    /* Expected ID arrives: clear missing. */
    f.id = 0x1A0;
    f.dlc = 1;
    expect(can_svc_on_rx(&f) == AE_OK, "cs_exp_rx");
    expect(can_svc_id_missing(0x1A0) == 0, "hit");

    p[0] = 1;
    expect(isotp_send(p, 1) != AE_OK, "iso_notx");
    expect(isotp_init(NULL, capture_can, on_pdu, NULL) != AE_OK, "iso_in");
    expect(isotp_init(&iso, capture_can, on_pdu, NULL) == AE_OK, "iso_ok");
    expect(isotp_send(NULL, 1) != AE_OK, "iso_sn");
    expect(isotp_send(p, 0) != AE_OK, "iso_s0");
    expect(isotp_send(p, 200) != AE_OK, "iso_sl");
    p[0] = 1;
    expect(isotp_send(p, 1) == AE_OK, "iso_sf");
    expect(isotp_on_can_frame(NULL) == AE_OK, "iso_fn");
    memset(&f, 0, sizeof(f));
    f.id = 0x111;
    expect(isotp_on_can_frame(&f) == AE_OK, "iso_wid");
    f.id = 0x7E0;
    f.data[0] = 0x00;
    expect(isotp_on_can_frame(&f) == AE_OK, "iso_n0");
    f.data[0] = 0x01;
    f.data[1] = 0xAA;
    g_cb = 0;
    expect(isotp_on_can_frame(&f) == AE_OK && g_cb == 1, "iso_sfrx");
    f.data[0] = 0x1F;
    f.data[1] = 0xFF;
    expect(isotp_on_can_frame(&f) != AE_OK, "iso_ovf");
    memset(big, 0x55, sizeof(big));
    g_txn = 0;
    expect(isotp_send(big, 20) == AE_OK, "iso_ff");
    memset(&f, 0, sizeof(f));
    f.id = 0x7E0;
    f.data[0] = 0x30;
    expect(isotp_on_can_frame(&f) == AE_OK, "iso_fc");
    expect(isotp_tick(10) == AE_OK, "iso_tick");
    expect(isotp_tick(20) == AE_OK, "iso_tick2");
    expect(isotp_tick(30) == AE_OK, "iso_tick3");
    /* TX remainder < 7 bytes (len=8 → one short CF). */
    (void)isotp_init(&iso, capture_can, on_pdu, NULL);
    g_txn = 0;
    expect(isotp_send(big, 8) == AE_OK, "iso_ff8");
    memset(&f, 0, sizeof(f));
    f.id = 0x7E0;
    f.data[0] = 0x30;
    expect(isotp_on_can_frame(&f) == AE_OK, "iso_fc8");
    expect(isotp_tick(1) == AE_OK, "iso_short_cf");
    /* RX multi: FF len=8 then CF */
    (void)isotp_init(&iso, capture_can, on_pdu, NULL);
    memset(&f, 0, sizeof(f));
    f.id = 0x7E0;
    f.data[0] = 0x10;
    f.data[1] = 8;
    f.data[2] = 1;
    f.data[3] = 2;
    f.data[4] = 3;
    f.data[5] = 4;
    f.data[6] = 5;
    f.data[7] = 6;
    g_cb = 0;
    expect(isotp_on_can_frame(&f) == AE_OK, "iso_ffrx");
    f.data[0] = 0x22;
    expect(isotp_on_can_frame(&f) != AE_OK, "iso_snerr");
    f.data[0] = 0x10;
    f.data[1] = 8;
    (void)isotp_on_can_frame(&f);
    f.data[0] = 0x21;
    f.data[1] = 7;
    f.data[2] = 8;
    g_cb = 0;
    expect(isotp_on_can_frame(&f) == AE_OK && g_cb == 1, "iso_done");
    f.data[0] = 0x21;
    expect(isotp_on_can_frame(&f) == AE_OK, "iso_cfidle");
    expect(isotp_tick(0) == AE_OK, "iso_idle_tick");
}

static void test_uds_dtc_svc(void)
{
    uint8_t i;
    uint8_t buf[32];
    uint16_t ln;
    dtc_record_t recs[16];
    uint8_t cnt;
    uint8_t dsc[2] = {UDS_SID_DSC, UDS_SESS_EXT};
    uint8_t seed[2] = {UDS_SID_SA, 0x01};
    uint8_t key[4] = {UDS_SID_SA, 0x02, 0xB7, 0x91}; /* 0x1234 ^ 0xA5A5 = 0xB791 */

    (void)dtc_init();
    expect(dtc_raise(1, DTC_SEV_INFO) == AE_OK, "dtc1");
    expect(dtc_raise(1, DTC_SEV_WARN) == AE_OK, "dtc2");
    expect(dtc_raise(1, DTC_SEV_CRIT) == AE_OK, "dtc3");
    expect(dtc_confirmed(1) == 1, "dtc_c");
    expect(dtc_confirmed(2) == 0, "dtc_n");
    expect(dtc_read(NULL, &cnt) != AE_OK, "dtc_rn");
    cnt = 8;
    expect(dtc_read(recs, &cnt) == AE_OK && cnt >= 1, "dtc_r");
    expect(dtc_clear(1) == AE_OK, "dtc_cl");
    for (i = 0; i < 16; i++) {
        (void)dtc_raise(0x100u + i, DTC_SEV_INFO);
    }
    expect(dtc_raise(0x200, DTC_SEV_INFO) != AE_OK, "dtc_full");
    (void)dtc_clear(0xFFFFFFFFu);

    /* HAL NVS was filled in test_hal; DID 0xF100 writes "cal". */
    hal_host_reset();
    {
        uint8_t tmp[8];
        uint16_t tl = 8;
        expect(product_did_cfg_read(0, tmp, &tl) == AE_OK && tl == 1, "cfg_miss");
    }
    g_pdu_n = 0;
    (void)uds_server_init(capture_tx);
    (void)uds_register_did(AE_DID_VIN, product_did_vin_read, NULL);
    (void)uds_register_did(0xF100u, product_did_cfg_read, product_did_cfg_write);
    (void)uds_register_did(0xF00Fu, did_fail, wr_fail);
    (void)uds_register_routine(0x0202u, product_routine_selftest);
    (void)uds_register_routine(0x0FFFu, rc_fail);
    expect(uds_server_on_pdu(NULL, 1) != AE_OK, "uds_n");
    expect(uds_server_on_pdu(dsc, 1) == AE_OK && g_pdu[0] == UDS_NRC, "dsc_sh");
    expect(uds_server_on_pdu(dsc, 2) == AE_OK, "dsc");
    expect(uds_server_session()->session == UDS_SESS_EXT, "sess");
    expect(uds_server_on_pdu((const uint8_t *)"\x11", 1) == AE_OK, "er");
    expect(uds_server_on_pdu((const uint8_t *)"\x11\x02", 2) == AE_OK, "er2");
    expect(uds_server_on_pdu((const uint8_t *)"\x3E", 1) == AE_OK, "tp");
    expect(uds_server_on_pdu((const uint8_t *)"\x14", 1) == AE_OK, "clr");
    expect(dtc_raise(AE_DTC_COMMS, DTC_SEV_WARN) == AE_OK, "dtc_u");
    expect(uds_server_on_pdu((const uint8_t *)"\x19", 1) == AE_OK && g_pdu_n > 2, "rdtc");
    expect(uds_server_on_pdu((const uint8_t *)"\x22", 1) == AE_OK, "rd_sh");
    expect(uds_server_on_pdu((const uint8_t *)"\x22\xF1\x90", 3) == AE_OK && g_pdu[0] == 0x62, "vin");
    expect(uds_server_on_pdu((const uint8_t *)"\x22\x00\x00", 3) == AE_OK && g_pdu[0] == UDS_NRC, "rd_miss");
    expect(uds_server_on_pdu((const uint8_t *)"\x22\xF0\x0F", 3) == AE_OK, "rd_fail");
    expect(uds_server_on_pdu((const uint8_t *)"\x2E\xF1\x00\x01", 4) == AE_OK && g_pdu[2] == 0x33, "wr_lock");
    expect(uds_server_on_pdu(seed, 2) == AE_OK, "seed");
    expect(uds_server_on_pdu((const uint8_t *)"\x27", 1) == AE_OK, "sa_def");
    expect(uds_server_on_pdu((const uint8_t *)"\x27\x02\x00\x00", 4) == AE_OK && g_pdu[2] == 0x35, "badkey");
    expect(uds_server_on_pdu(key, 4) == AE_OK, "goodkey");
    expect(uds_server_on_pdu((const uint8_t *)"\x10\x03", 2) == AE_OK, "ext2");
    expect(uds_server_on_pdu(key, 4) == AE_OK, "key2");
    expect(uds_server_on_pdu((const uint8_t *)"\x2E\xF1", 2) == AE_OK, "wr_sh");
    expect(uds_server_on_pdu((const uint8_t *)"\x2E\xF1\x00\xAA", 4) == AE_OK && g_pdu[0] == 0x6E, "wr_ok");
    expect(uds_server_on_pdu((const uint8_t *)"\x2E\x00\x00\x01", 4) == AE_OK, "wr_miss");
    expect(uds_server_on_pdu((const uint8_t *)"\x2E\xF0\x0F\x01", 4) == AE_OK, "wr_fail");
    expect(uds_server_on_pdu((const uint8_t *)"\x31\x01", 2) == AE_OK, "rc_sh");
    expect(uds_server_on_pdu((const uint8_t *)"\x31\x01\x02\x02", 4) == AE_OK && g_pdu[0] == 0x71, "rc_ok");
    expect(uds_server_on_pdu((const uint8_t *)"\x31\x01\x00\x00", 4) == AE_OK, "rc_miss");
    expect(uds_server_on_pdu((const uint8_t *)"\x31\x01\x0F\xFF", 4) == AE_OK, "rc_fail");
    expect(uds_server_on_pdu((const uint8_t *)"\x99", 1) == AE_OK && g_pdu[2] == 0x11, "unk");
    expect(uds_server_tick(0) == AE_OK, "tick0");
    expect(uds_server_tick(6000) == AE_OK, "s3");
    expect(uds_server_session()->session == UDS_SESS_DEFAULT, "s3d");
    (void)uds_server_init(NULL);
    expect(uds_server_on_pdu((const uint8_t *)"\x3E", 1) == AE_OK, "notx");
    for (i = 0; i < 8; i++) {
        (void)uds_register_did((uint16_t)(0x1000u + i), product_did_sw_read, NULL);
    }
    expect(uds_register_did(0x10FF, product_did_sw_read, NULL) != AE_OK, "did_full");
    (void)uds_server_init(capture_tx);
    for (i = 0; i < 4; i++) {
        (void)uds_register_routine((uint16_t)(0x30u + i), product_routine_selftest);
    }
    expect(uds_register_routine(0x40, product_routine_selftest) != AE_OK, "rid_full");
    expect(uds_client_init(NULL) == AE_OK, "cli0");
    expect(uds_client_request(0x3E, NULL, 0) != AE_OK, "cli_notx");
    expect(uds_client_request(0x22, buf, 80) != AE_OK, "cli_big");
    (void)uds_client_init(capture_tx);
    expect(uds_client_request(0x3E, NULL, 0) == AE_OK, "cli_tp");
    expect(uds_client_on_pdu(NULL, 1) != AE_OK, "cli_pn");
    expect(uds_client_on_pdu(buf, 0) != AE_OK, "cli_p0");
    expect(uds_client_on_pdu((const uint8_t *)"\x7E", 1) == AE_OK, "cli_p");
    {
        uint8_t huge[129];
        memset(huge, 0, sizeof(huge));
        expect(uds_client_on_pdu(huge, 129) != AE_OK, "cli_huge");
    }
    (void)uds_client_last(NULL);
    (void)uds_client_last(&ln);

    expect(product_did_vin_read(0, NULL, &ln) != AE_OK, "vin_n");
    ln = 4;
    expect(product_did_vin_read(0, buf, &ln) != AE_OK, "vin_s");
    ln = 32;
    expect(product_did_vin_read(0, buf, &ln) == AE_OK, "vin_ok");
    expect(product_did_sw_read(0, NULL, &ln) != AE_OK, "sw_n");
    ln = 2;
    expect(product_did_sw_read(0, buf, &ln) != AE_OK, "sw_s");
    ln = 8;
    expect(product_did_sw_read(0, buf, &ln) == AE_OK, "sw_ok");
    expect(product_did_cfg_read(0, NULL, &ln) != AE_OK, "cfg_n");
    ln = 8;
    expect(product_did_cfg_read(0, buf, &ln) == AE_OK, "cfg_m");
    expect(product_routine_selftest(1, 1, NULL, &ln) != AE_OK, "rcn");
}

static void test_fault_ota_ble_ecu(void)
{
    ae_can_frame_t f;
    ae_can_frame_t st;
    uint16_t ch = 1;
    uint16_t ln;
    const uint8_t *p;

    (void)fault_init();
    expect(fault_report(1, 0) == AE_OK && fault_get_state() == FAULT_DEGRADED, "deg");
    expect(fault_recover() == AE_OK && fault_get_state() == FAULT_NORMAL, "rec");
    expect(fault_report(1, 1) == AE_OK, "recov");
    expect(fault_report(1, 1) == AE_OK, "recov2");
    expect(fault_report(1, 1) == AE_OK, "recov3");
    expect(fault_report(1, 1) == AE_OK && fault_get_state() == FAULT_SAFE, "safe");
    expect(fault_recover() == AE_OK && fault_get_state() == FAULT_SAFE, "safe_stay");

    (void)ota_init();
    expect(ota_begin(0, 0) != AE_OK, "ota0");
    expect(ota_begin(600, 0) != AE_OK, "ota_big");
    expect(ota_begin(4, 1) == AE_OK, "ota_b");
    expect(ota_write(NULL, 1) != AE_OK, "ota_wn");
    expect(ota_state() == OTA_DISCARD, "ota_d");
    (void)ota_begin(4, 1);
    expect(ota_write((const uint8_t *)"abcd", 4) == AE_OK, "ota_w");
    expect(ota_finish() != AE_OK, "ota_badcrc");
    expect(ota_begin(8, 1) == AE_OK, "ota_b2");
    expect(ota_write((const uint8_t *)"abcd", 4) == AE_OK, "ota_part");
    expect(ota_finish() != AE_OK, "ota_short");
    expect(ota_running_slot_valid() == 1, "slot");

    (void)ble_auto_init(NULL, NULL);
    expect(ble_auto_notify(1, NULL, 1) != AE_OK, "ble_n");
    expect(ble_auto_notify(1, (const uint8_t *)"hi", 2) == AE_OK, "ble_ok");
    {
        uint8_t too[81];
        memset(too, 1, sizeof(too));
        expect(ble_auto_notify(1, too, 81) != AE_OK, "ble_big");
    }
    (void)ble_auto_last_notify(NULL, NULL);
    expect(ble_auto_inject_write(1, (const uint8_t *)"x", 1) == AE_OK, "ble_i");
    p = ble_auto_last_notify(&ch, &ln);
    expect(p != NULL && ln == 2, "ble_l");

    ecu_models_init();
    expect(bcm_fill_status(NULL) != AE_OK, "bcm_n");
    expect(bcm_fill_status(&f) == AE_OK && f.id == AE_CAN_BCM_STATUS, "bcm");
    expect(vcu_fill_hb(NULL) != AE_OK, "vcu_n");
    expect(vcu_fill_hb(&f) == AE_OK, "vcu");
    expect(vcu_fill_torque(NULL, 1) != AE_OK, "tq_n");
    expect(vcu_fill_torque(&f, 10) == AE_OK, "tq");
    expect(bms_fill_status(NULL) != AE_OK, "bms_n");
    expect(bms_fill_status(&f) == AE_OK, "bms");
    expect(bms_fill_temp(NULL) != AE_OK, "tmp_n");
    expect(bms_fill_temp(&f) == AE_OK, "tmp");
    expect(bcm_on_light_cmd(NULL, &st) != AE_OK, "lt_n");
    memset(&f, 0, sizeof(f));
    f.id = 0x010;
    expect(bcm_on_light_cmd(&f, &st) != AE_OK, "lt_id");
    f.id = AE_CAN_BCM_LIGHT_CMD;
    f.data[0] = 3;
    expect(bcm_on_light_cmd(&f, &st) == AE_OK, "lt");
    (void)bms_set_temp(40, 60);
    expect(vcu_fill_torque(&f, 99) == AE_OK && f.data[0] == 0 && f.data[1] == 0, "tq0");
    expect(bcm_get() != NULL && vcu_get() != NULL && bms_get() != NULL, "get");
}

static void test_runtime_products(void)
{
    uint8_t n = 0;
    uint8_t i;
    const ae_product_desc_t *tab = ae_products_table(&n);

    expect(n == 17, "n17");
    (void)ae_products_table(NULL);
    hal_host_reset();
    aegw_runtime_bind(99);
    aegw_runtime_setup();
    expect(aegw_runtime_index() == 0, "clamp");
    expect(aegw_runtime_select(99) != AE_OK, "sel_bad");
    expect(aegw_runtime_select(0) == AE_OK, "sel0");
    expect(aegw_runtime_index() == 0, "idx");
    {
        ae_can_frame_t uds;
        memset(&uds, 0, sizeof(uds));
        uds.id = AE_CAN_UDS_REQ;
        uds.dlc = 8;
        uds.data[0] = 0x01;
        uds.data[1] = 0x3E;
        expect(hal_can_send(&uds, 0) == AE_OK, "uds_sf");
    }
    for (i = 0; i < 120; i++) {
        aegw_runtime_loop();
    }
    for (i = 0; i < n; i++) {
        expect(tab[i].init() == AE_OK, tab[i].id);
        expect(tab[i].run() == AE_OK, tab[i].id);
    }
}

int main(void)
{
    g_fail = 0;
    test_error_ring();
    test_hal();
    test_can_iso_uds();
    test_uds_dtc_svc();
    test_fault_ota_ble_ecu();
    test_runtime_products();
    printf(g_fail ? "RESULT FAIL\n" : "RESULT PASS\n");
    return g_fail;
}
