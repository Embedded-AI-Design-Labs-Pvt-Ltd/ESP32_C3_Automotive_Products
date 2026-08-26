/**
 * @file products_security_ota.c
 * @brief P14 CAN IDS, P16 training kit, P17 bootloader/OTA.
 */

#include "product_api.h"

#include "ae_can_ids.h"
#include "ae_error.h"
#include "ble_auto.h"
#include "can_service.h"
#include "dtc.h"
#include "ecu_models.h"
#include "ota_agent.h"

#include <string.h>

static const uint32_t s_allow[] = {
    AE_CAN_BCM_STATUS, AE_CAN_VCU_HB, AE_CAN_BMS_STATUS,
    AE_CAN_UDS_REQ, AE_CAN_UDS_RESP, AE_CAN_OBD_FUNC
};

static int id_allowed(uint32_t id)
{
    uint8_t i;

    for (i = 0; i < (uint8_t)(sizeof(s_allow) / sizeof(s_allow[0])); i++) {
        if (s_allow[i] == id) {
            return 1;
        }
    }
    return 0;
}

ae_status_t p14_can_ids_init(void)
{
    (void)dtc_init();
    return can_svc_init();
}

ae_status_t p14_can_ids_run(void)
{
    ae_can_frame_t ok;
    ae_can_frame_t bad;
    uint8_t i;

    (void)p14_can_ids_init();
    memset(&ok, 0, sizeof(ok));
    ok.id = AE_CAN_BCM_STATUS;
    ok.dlc = 8u;
    memset(&bad, 0, sizeof(bad));
    bad.id = 0x666u; /* unexpected / not on allow-list (SIM) */
    bad.dlc = 8u;
    (void)can_svc_on_rx(&ok);
    (void)can_svc_on_rx(&bad);
    if (id_allowed(bad.id) != 0) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    for (i = 0; i < 3u; i++) {
        (void)dtc_raise(AE_DTC_COMMS, DTC_SEV_WARN);
    }
    return AE_OK;
}

ae_status_t p16_training_init(void)
{
    ecu_models_init();
    (void)dtc_init();
    return can_svc_init();
}

ae_status_t p16_training_run(void)
{
    ae_can_frame_t f;

    (void)p16_training_init();
    (void)bcm_fill_status(&f);
    (void)can_svc_send(&f);
    (void)vcu_fill_hb(&f);
    (void)can_svc_send(&f);
    (void)bms_fill_status(&f);
    (void)can_svc_send(&f);
    return AE_OK;
}

ae_status_t p17_ota_init(void)
{
    (void)ble_auto_init(NULL, NULL);
    return ota_init();
}

ae_status_t p17_ota_run(void)
{
    const uint8_t img[8] = {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};
    uint32_t crc;
    uint32_t c = 0xFFFFFFFFu;
    uint32_t i;
    uint8_t b;

    (void)p17_ota_init();
    for (i = 0; i < sizeof(img); i++) {
        c ^= img[i];
        for (b = 0; b < 8u; b++) {
            c = (c >> 1) ^ (0xEDB88320u & (uint32_t)(-(int32_t)(c & 1u)));
        }
    }
    crc = ~c;
    if (ota_begin((uint32_t)sizeof(img), crc) != AE_OK) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    if (ota_write(img, (uint16_t)sizeof(img)) != AE_OK) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    if (ota_finish() != AE_OK) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    if (ota_running_slot_valid() == 0) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    /* Bad CRC must discard inactive slot only. */
    (void)ota_begin(4u, 0u);
    (void)ota_write(img, 4u);
    if (ae_err_is_ok(ota_finish())) {
        return ae_err_make(AE_MOD_APP, 10u);
    }
    return (ota_running_slot_valid() != 0) ? AE_OK : ae_err_make(AE_MOD_APP, 10u);
}
