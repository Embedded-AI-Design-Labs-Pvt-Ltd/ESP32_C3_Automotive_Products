/**
 * @file aegw_runtime.c
 * @brief Cooperative real-time loop: WDT, cyclic CAN, UDS tick, product work.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 *
 * ISR policy: HAL RX callback only queues; this task does protocol work.
 */

#include "aegw_runtime.h"

#include "ae_can_ids.h"
#include "ae_copyright.h"
#include "ae_error.h"
#include "ble_auto.h"
#include "can_service.h"
#include "dtc.h"
#include "ecu_models.h"
#include "fault_mgr.h"
#include "hal_can.h"
#include "hal_misc.h"
#include "isotp.h"
#include "product_api.h"
#include "product_dids.h"
#include "uds.h"

#include <string.h>

#ifndef AE_PRODUCT_INDEX
#define AE_PRODUCT_INDEX 15u /* P16 training kit: BCM/VCU/BMS on one node */
#endif

static uint8_t s_idx = (uint8_t)AE_PRODUCT_INDEX;
static uint32_t s_last_100;
static uint32_t s_last_10;
static uint8_t s_pdu[80];
static uint16_t s_pdu_n;

static ae_status_t uds_tx(const uint8_t *pdu, uint16_t len)
{
    return isotp_send(pdu, len);
}

static void on_isotp_pdu(const uint8_t *pdu, uint16_t len, void *ctx)
{
    (void)ctx;
    if ((pdu != NULL) && (len <= sizeof(s_pdu))) {
        memcpy(s_pdu, pdu, len);
        s_pdu_n = len;
    }
    (void)uds_server_on_pdu(pdu, len);
}

static ae_status_t on_isotp_can(const ae_can_frame_t *f)
{
    return isotp_on_can_frame(f);
}

static void on_can_rx(const ae_can_frame_t *f, void *ctx)
{
    (void)ctx;
    (void)can_svc_on_rx(f);
    (void)on_isotp_can(f);
}

void aegw_runtime_setup(void)
{
    ae_can_cfg_t can = {500000u};
    ae_uart_cfg_t uart = {115200u};
    ae_gpio_cfg_t gpio = {8u, 1u};
    isotp_cfg_t iso = {AE_CAN_UDS_RESP, AE_CAN_UDS_REQ, 0u, 0u};
    uint8_t n = 0u;
    const ae_product_desc_t *tab;

    (void)hal_can_init(&can);
    (void)hal_uart_init(&uart);
    (void)hal_gpio_init(&gpio);
    (void)hal_can_attach_rx(on_can_rx, NULL);
    (void)can_svc_init();
    (void)dtc_init();
    (void)fault_init();
    (void)ble_auto_init(NULL, NULL);
    ecu_models_init();
    (void)isotp_init(&iso, can_svc_send, on_isotp_pdu, NULL);
    (void)uds_server_init(uds_tx);
    (void)uds_register_did(AE_DID_VIN, product_did_vin_read, NULL);
    (void)uds_register_did(AE_DID_SW, product_did_sw_read, NULL);
    (void)uds_register_did(0xF100u, product_did_cfg_read, product_did_cfg_write);
    (void)uds_register_routine(0x0202u, product_routine_selftest);
    (void)can_svc_expect(AE_CAN_BCM_STATUS, 100u, 300u);

    tab = ae_products_table(&n);
    if (s_idx >= n) {
        s_idx = 0u;
    }
    (void)tab[s_idx].init();
    (void)hal_wdg_kick();
    {
        static const char notice[] = AE_COPYRIGHT_NOTICE "\r\n";
        (void)hal_uart_write((const uint8_t *)notice, sizeof(notice) - 1u, 0u);
    }
    s_last_100 = 0u;
    s_last_10 = 0u;
    s_pdu_n = 0u;
    (void)s_pdu;
}

ae_status_t aegw_runtime_select(uint8_t product_index)
{
    uint8_t n = 0u;
    const ae_product_desc_t *tab = ae_products_table(&n);

    if (product_index >= n) {
        return ae_err_make(AE_MOD_APP, 1u);
    }
    s_idx = product_index;
    return tab[s_idx].init();
}

uint8_t aegw_runtime_index(void)
{
    return s_idx;
}

void aegw_runtime_bind(uint8_t product_index)
{
    s_idx = product_index;
}

void aegw_runtime_loop(void)
{
    uint32_t now = hal_millis();
    ae_can_frame_t f;
    uint8_t spins;

    (void)hal_wdg_kick();

    /* Drain TWAI / software RX. ISR/callback only queues; this task parses. */
    for (spins = 0u; spins < 16u; spins++) {
        if (hal_can_recv(&f) != AE_OK) {
            break;
        }
    }

    (void)can_svc_tick(now);
    (void)isotp_tick(now);
    (void)uds_server_tick(now);

    /* 10 ms VCU torque (cooperative real-time cyclic). */
    if ((now - s_last_10) >= 10u) {
        s_last_10 = now;
        (void)vcu_fill_torque(&f, 80);
        (void)can_svc_send(&f);
    }
    /* 100 ms BCM / VCU / BMS heartbeat. Product use-case runs on select(). */
    if ((now - s_last_100) >= 100u) {
        s_last_100 = now;
        (void)bcm_fill_status(&f);
        (void)can_svc_send(&f);
        (void)vcu_fill_hb(&f);
        (void)can_svc_send(&f);
        (void)bms_fill_status(&f);
        (void)can_svc_send(&f);
        (void)hal_gpio_write(8u, (uint8_t)((now / 100u) & 1u));
    }
}
