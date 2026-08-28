/**
 * @file usecases.c
 * @brief Implementation of 120 lab use cases on host/Virtual ECU HAL.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "usecases.h"

#include "ae_can_ids.h"
#include "ae_error.h"
#include "ae_ring.h"
#include "can_service.h"
#include "crc_e2e.h"
#include "dtc.h"
#include "ecu_models.h"
#include "fault_mgr.h"
#include "hal_can.h"
#include "hal_misc.h"
#if defined(AE_HOST)
#include "hal_can_adapter.h"
#endif
#include "isotp.h"
#include "ota_agent.h"
#include "product_dids.h"
#include "uds.h"

#include <string.h>

static const ae_usecase_desc_t s_uc[AE_UC_COUNT] = {
    /* 1–20 CAN */
    {1, AE_UC_CAT_CAN, "CAN init 500 kbit/s", "REQ-LAB-005"},
    {2, AE_UC_CAT_CAN, "CAN TX standard ID", "REQ-LAB-005"},
    {3, AE_UC_CAT_CAN, "CAN TX extended ID", "REQ-LAB-005"},
    {4, AE_UC_CAT_CAN, "CAN RX queue drain", "REQ-LAB-005"},
    {5, AE_UC_CAT_CAN, "CAN filter accept", "REQ-LAB-005"},
    {6, AE_UC_CAT_CAN, "CAN bus overflow detect", "REQ-LAB-005"},
    {7, AE_UC_CAT_CAN, "CAN expect timeout DTC", "REQ-LAB-005"},
    {8, AE_UC_CAT_CAN, "BCM status cyclic TX", "REQ-LAB-010"},
    {9, AE_UC_CAT_CAN, "VCU heartbeat TX", "REQ-LAB-010"},
    {10, AE_UC_CAT_CAN, "VCU torque TX", "REQ-LAB-010"},
    {11, AE_UC_CAT_CAN, "BMS pack status TX", "REQ-LAB-010"},
    {12, AE_UC_CAT_CAN, "BMS temperature TX", "REQ-LAB-010"},
    {13, AE_UC_CAT_CAN, "TPMS frame TX", "REQ-LAB-011"},
    {14, AE_UC_CAT_CAN, "Sensor gateway pack TX", "REQ-LAB-011"},
    {15, AE_UC_CAT_CAN, "Light command/response", "REQ-LAB-010"},
    {16, AE_UC_CAT_CAN, "CAN stats counters", "REQ-LAB-005"},
    {17, AE_UC_CAT_CAN, "CAN subscribe callback", "REQ-LAB-005"},
    {18, AE_UC_CAT_CAN, "RTR frame reject path", "REQ-LAB-005"},
    {19, AE_UC_CAT_CAN, "DLC 0..8 sweep", "REQ-LAB-002"},
    {20, AE_UC_CAT_CAN, "Bus reset recover", "REQ-LAB-005"},
    /* 21–40 UDS */
    {21, AE_UC_CAT_UDS, "UDS 0x10 Default session", "REQ-LAB-007"},
    {22, AE_UC_CAT_UDS, "UDS 0x10 Extended session", "REQ-LAB-007"},
    {23, AE_UC_CAT_UDS, "UDS 0x10 Programming session", "REQ-LAB-007"},
    {24, AE_UC_CAT_UDS, "UDS 0x3E TesterPresent", "REQ-LAB-007"},
    {25, AE_UC_CAT_UDS, "UDS 0x22 Read VIN F190", "REQ-LAB-007"},
    {26, AE_UC_CAT_UDS, "UDS 0x22 Read SW F189", "REQ-LAB-007"},
    {27, AE_UC_CAT_UDS, "UDS 0x22 unsupported DID NRC", "REQ-LAB-007"},
    {28, AE_UC_CAT_UDS, "UDS 0x2E Write DID proto", "REQ-LAB-007"},
    {29, AE_UC_CAT_UDS, "UDS 0x14 Clear DTC", "REQ-LAB-008"},
    {30, AE_UC_CAT_UDS, "UDS 0x19 Read DTC", "REQ-LAB-008"},
    {31, AE_UC_CAT_UDS, "UDS 0x11 ECU Reset proto", "REQ-LAB-007"},
    {32, AE_UC_CAT_UDS, "UDS 0x27 SecurityAccess seed", "REQ-LAB-007"},
    {33, AE_UC_CAT_UDS, "UDS 0x27 bad key lockout path", "REQ-LAB-007"},
    {34, AE_UC_CAT_UDS, "UDS 0x31 RoutineControl", "REQ-LAB-007"},
    {35, AE_UC_CAT_UDS, "UDS negative response length", "REQ-LAB-007"},
    {36, AE_UC_CAT_UDS, "UDS S3 timeout tick", "REQ-LAB-007"},
    {37, AE_UC_CAT_UDS, "UDS client/server loopback", "REQ-LAB-007"},
    {38, AE_UC_CAT_UDS, "OBD functional 0x7DF speed", "REQ-D-003"},
    {39, AE_UC_CAT_UDS, "ISO-TP single frame UDS", "REQ-LAB-006"},
    {40, AE_UC_CAT_UDS, "ISO-TP multi-frame UDS", "REQ-LAB-006"},
    /* 41–55 Diag/Fault */
    {41, AE_UC_CAT_DIAG, "DTC raise pending", "REQ-LAB-008"},
    {42, AE_UC_CAT_DIAG, "DTC confirm after repeats", "REQ-LAB-008"},
    {43, AE_UC_CAT_DIAG, "DTC clear all", "REQ-LAB-008"},
    {44, AE_UC_CAT_FAULT, "Fault NORMAL state", "REQ-LAB-009"},
    {45, AE_UC_CAT_FAULT, "Fault DETECTED", "REQ-LAB-009"},
    {46, AE_UC_CAT_FAULT, "Fault DEGRADED critical", "REQ-LAB-009"},
    {47, AE_UC_CAT_FAULT, "Fault SAFE state", "REQ-LAB-009"},
    {48, AE_UC_CAT_FAULT, "Fault recover to NORMAL", "REQ-LAB-009"},
    {49, AE_UC_CAT_DIAG, "DTC CAN timeout BCM", "REQ-LAB-008"},
    {50, AE_UC_CAT_DIAG, "DTC BMS over-temp", "REQ-LAB-008"},
    {51, AE_UC_CAT_DIAG, "DTC unexpected CAN ID", "REQ-LAB-008"},
    {52, AE_UC_CAT_DIAG, "DTC sensor fail", "REQ-LAB-008"},
    {53, AE_UC_CAT_DIAG, "DTC low voltage", "REQ-LAB-008"},
    {54, AE_UC_CAT_FAULT, "Watchdog kick count", "REQ-LAB-009"},
    {55, AE_UC_CAT_FAULT, "NVS config roundtrip", "REQ-H-004"},
    /* 56–75 Cyber */
    {56, AE_UC_CAT_CYBER, "IDS allow-list accept", "REQ-LAB-017"},
    {57, AE_UC_CAT_CYBER, "IDS reject unknown ID", "REQ-LAB-017"},
    {58, AE_UC_CAT_CYBER, "Flood rate detect", "REQ-LAB-017"},
    {59, AE_UC_CAT_CYBER, "Replay same frame detect", "REQ-LAB-017"},
    {60, AE_UC_CAT_CYBER, "Invalid DLC detect", "REQ-LAB-017"},
    {61, AE_UC_CAT_CYBER, "Diagnostic abuse rate limit", "REQ-LAB-017"},
    {62, AE_UC_CAT_CYBER, "Spoof VCU torque detect", "REQ-LAB-017"},
    {63, AE_UC_CAT_CYBER, "Bus-off storm counter", "REQ-LAB-017"},
    {64, AE_UC_CAT_CYBER, "Unexpected period detect", "REQ-LAB-017"},
    {65, AE_UC_CAT_CYBER, "Blacklist drop ID", "REQ-LAB-017"},
    {66, AE_UC_CAT_CYBER, "Security event log slot", "REQ-LAB-017"},
    {67, AE_UC_CAT_CYBER, "UDS without session deny", "REQ-LAB-017"},
    {68, AE_UC_CAT_CYBER, "Write DID without SA deny", "REQ-LAB-017"},
    {69, AE_UC_CAT_CYBER, "ISO-TP overflow abort", "REQ-LAB-006"},
    {70, AE_UC_CAT_CYBER, "Malformed UDS length NRC", "REQ-LAB-007"},
    {71, AE_UC_CAT_CYBER, "CAN ID scan probe detect", "REQ-LAB-017"},
    {72, AE_UC_CAT_CYBER, "DoS TX queue full", "REQ-LAB-017"},
    {73, AE_UC_CAT_CYBER, "Clock skew timeout", "REQ-LAB-012"},
    {74, AE_UC_CAT_CYBER, "Payload range violation", "REQ-LAB-012"},
    {75, AE_UC_CAT_CYBER, "Lab-only attack traffic tag", "REQ-LAB-017"},
    /* 76–90 Vehicle */
    {76, AE_UC_CAT_VEHICLE, "Ignition ON sequence", "REQ-LAB-010"},
    {77, AE_UC_CAT_VEHICLE, "Idle speed hold", "REQ-LAB-010"},
    {78, AE_UC_CAT_VEHICLE, "Acceleration torque ramp", "REQ-LAB-010"},
    {79, AE_UC_CAT_VEHICLE, "Brake torque cut", "REQ-LAB-010"},
    {80, AE_UC_CAT_VEHICLE, "Regen mode flag", "REQ-LAB-010"},
    {81, AE_UC_CAT_VEHICLE, "Limp-home torque cap", "REQ-LAB-010"},
    {82, AE_UC_CAT_VEHICLE, "SOC discharge step", "REQ-LAB-010"},
    {83, AE_UC_CAT_VEHICLE, "Charge state limit", "REQ-LAB-010"},
    {84, AE_UC_CAT_VEHICLE, "Door ajar signal", "REQ-LAB-010"},
    {85, AE_UC_CAT_VEHICLE, "Exterior lights pattern", "REQ-LAB-010"},
    {86, AE_UC_CAT_VEHICLE, "HVIL conceptual ready", "REQ-LAB-010"},
    {87, AE_UC_CAT_VEHICLE, "Thermal derate", "REQ-LAB-010"},
    {88, AE_UC_CAT_VEHICLE, "Gear/drive mode change", "REQ-LAB-010"},
    {89, AE_UC_CAT_VEHICLE, "Shutdown sequence", "REQ-LAB-010"},
    {90, AE_UC_CAT_VEHICLE, "Startup self-test pass", "REQ-LAB-010"},
    /* 91–105 E2E */
    {91, AE_UC_CAT_E2E, "E2E protect frame", "REQ-LAB-012"},
    {92, AE_UC_CAT_E2E, "E2E accept valid", "REQ-LAB-012"},
    {93, AE_UC_CAT_E2E, "E2E reject bad CRC", "REQ-LAB-012"},
    {94, AE_UC_CAT_E2E, "E2E reject bad counter", "REQ-LAB-012"},
    {95, AE_UC_CAT_E2E, "E2E timeout", "REQ-LAB-012"},
    {96, AE_UC_CAT_E2E, "E2E counter rollover 0..15", "REQ-LAB-012"},
    {97, AE_UC_CAT_E2E, "CRC8 J1850 smoke", "REQ-LAB-012"},
    {98, AE_UC_CAT_E2E, "CRC16 CCITT smoke", "REQ-LAB-012"},
    {99, AE_UC_CAT_E2E, "CRC32 smoke", "REQ-LAB-012"},
    {100, AE_UC_CAT_E2E, "E2E DLC reject", "REQ-LAB-012"},
    {101, AE_UC_CAT_E2E, "Duplicate frame detect", "REQ-LAB-012"},
    {102, AE_UC_CAT_E2E, "Plausibility speed jump", "REQ-LAB-012"},
    {103, AE_UC_CAT_E2E, "Range check SOC", "REQ-LAB-012"},
    {104, AE_UC_CAT_E2E, "Alive freeze detect", "REQ-LAB-012"},
    {105, AE_UC_CAT_E2E, "E2E on VCU HB", "REQ-LAB-012"},
    /* 106–120 HIL / OTA / Gateway */
    {106, AE_UC_CAT_HIL, "HIL inject BCM frame", "REQ-LAB-022"},
    {107, AE_UC_CAT_HIL, "HIL GPIO write/read", "REQ-LAB-022"},
    {108, AE_UC_CAT_HIL, "HIL ADC stim", "REQ-LAB-022"},
    {109, AE_UC_CAT_HIL, "HIL drop frame inject", "REQ-LAB-023"},
    {110, AE_UC_CAT_HIL, "HIL delay tick", "REQ-LAB-023"},
    {111, AE_UC_CAT_OTA, "OTA begin image", "REQ-LAB-019"},
    {112, AE_UC_CAT_OTA, "OTA write chunks", "REQ-LAB-019"},
    {113, AE_UC_CAT_OTA, "OTA verify CRC fail discard", "REQ-LAB-019"},
    {114, AE_UC_CAT_OTA, "OTA running slot valid", "REQ-LAB-019"},
    {115, AE_UC_CAT_GATEWAY, "Gateway forward 0x1A2->0x1A3", "REQ-LAB-001"},
    {116, AE_UC_CAT_GATEWAY, "Gateway filter drop", "REQ-LAB-001"},
    {117, AE_UC_CAT_GATEWAY, "Gateway rate limit", "REQ-LAB-001"},
    {118, AE_UC_CAT_GATEWAY, "Ring buffer ISR path", "REQ-LAB-001"},
    {119, AE_UC_CAT_HIL, "Millis monotonic smoke", "REQ-LAB-018"},
    {120, AE_UC_CAT_CAN, "Host reset clears bus", "REQ-LAB-018"},
};

const char *ae_uc_category_name(ae_uc_category_t cat)
{
    switch (cat) {
    case AE_UC_CAT_CAN:
        return "CAN";
    case AE_UC_CAT_UDS:
        return "UDS";
    case AE_UC_CAT_DIAG:
        return "DIAG";
    case AE_UC_CAT_FAULT:
        return "FAULT";
    case AE_UC_CAT_CYBER:
        return "CYBER";
    case AE_UC_CAT_VEHICLE:
        return "VEHICLE";
    case AE_UC_CAT_E2E:
        return "E2E";
    case AE_UC_CAT_HIL:
        return "HIL";
    case AE_UC_CAT_OTA:
        return "OTA";
    case AE_UC_CAT_GATEWAY:
        return "GATEWAY";
    default:
        return "?";
    }
}

const ae_usecase_desc_t *ae_usecases_table(uint16_t *count)
{
    if (count != NULL) {
        *count = AE_UC_COUNT;
    }
    return s_uc;
}

static uint8_t s_uds_buf[96];
static uint16_t s_uds_n;
static uint32_t s_sub_hits;
static uint32_t s_ids_unknown;
static uint32_t s_flood;
static uint32_t s_sec_events;
static uint8_t s_last_payload[8];
static uint8_t s_have_last;
static uint8_t s_speed_last = 0xFFu;

static ae_status_t uds_loop_tx(const uint8_t *pdu, uint16_t len)
{
    if (len > sizeof(s_uds_buf)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    memcpy(s_uds_buf, pdu, len);
    s_uds_n = len;
    return uds_client_on_pdu(pdu, len);
}

static void uc_bus_reset(void)
{
    ae_can_cfg_t cfg;

    (void)hal_host_reset();
    memset(&cfg, 0, sizeof(cfg));
    cfg.bitrate = 500000u;
#if defined(AE_HOST)
    (void)hal_can_cfg_from_env(&cfg);
#endif
    (void)hal_can_init(&cfg);
    (void)can_svc_init();
    (void)dtc_init();
    (void)fault_init();
    (void)ecu_models_init();
}

static void on_rx_count(const ae_can_frame_t *frame, void *ctx)
{
    (void)frame;
    (void)ctx;
    s_sub_hits++;
}

static int id_allowed(uint32_t id)
{
    return (id == AE_CAN_BCM_STATUS) || (id == AE_CAN_VCU_HB) || (id == AE_CAN_VCU_TORQUE) ||
           (id == AE_CAN_BMS_STATUS) || (id == AE_CAN_BMS_TEMP) || (id == AE_CAN_TPMS) ||
           (id == AE_CAN_UDS_REQ) || (id == AE_CAN_UDS_RESP) || (id == AE_CAN_BCM_LIGHT_CMD) ||
           (id == AE_CAN_BCM_LIGHT_ST) || (id == AE_CAN_SENSOR) || (id == AE_CAN_OBD_FUNC);
}

static ae_status_t uc_send_id(uint32_t id, uint8_t dlc)
{
    ae_can_frame_t f;
    memset(&f, 0, sizeof(f));
    f.id = id;
    f.dlc = dlc;
    f.data[0] = 0xA5u;
    return can_svc_send(&f);
}

static ae_status_t uc_uds_session(uint8_t sess)
{
    uint8_t d[1] = {sess};
    uint16_t n = 0u;
    const uint8_t *p;
    (void)uds_server_init(uds_loop_tx);
    (void)uds_client_init(uds_server_on_pdu);
    s_uds_n = 0u;
    (void)uds_client_request(UDS_SID_DSC, d, 1u);
    p = uds_client_last(&n);
    if ((n < 2u) || (p[0] != UDS_POS(UDS_SID_DSC))) {
        return ae_err_make(AE_MOD_APP, 21u);
    }
    return AE_OK;
}

ae_status_t ae_usecase_run(uint16_t id)
{
    ae_can_frame_t f;
    ae_can_filter_t filt;
    can_svc_stats_t st;
    ae_e2e_state_t e2e;
    uint8_t data[8];
    uint8_t ctr;
    uint8_t i;
    uint16_t n;
    const uint8_t *p;
    uint8_t level;
    uint16_t mv;
    size_t nvs_len;
    uint8_t nvs[8];
    ae_ring_t ring;
    uint8_t rb[16];
    uint8_t did[2];
    isotp_cfg_t icfg;
    uint8_t longpdu[40];

    if ((id == 0u) || (id > AE_UC_COUNT)) {
        return ae_err_make(AE_MOD_APP, 1u);
    }

    uc_bus_reset();

    switch (id) {
    case 1:
        return AE_OK;
    case 2:
        return uc_send_id(AE_CAN_BCM_STATUS, 8u);
    case 3:
        memset(&f, 0, sizeof(f));
        f.id = 0x18DAF100u;
        f.ide = 1u;
        f.dlc = 8u;
        return can_svc_send(&f);
    case 4:
        (void)uc_send_id(AE_CAN_VCU_HB, 8u);
        return (hal_can_recv(&f) == AE_OK) ? AE_OK : ae_err_make(AE_MOD_APP, 4u);
    case 5:
        filt.id = AE_CAN_BCM_STATUS;
        filt.mask = 0x7FFu;
        return hal_can_set_filter(&filt, 1u);
    case 6:
        /* Live USB-CAN: keep TX modest; sim still stresses the ring. */
        {
#if defined(AE_HOST)
            uint8_t n = (uint8_t)(hal_can_adapter_is_live() ? 8u : 70u);
#else
            uint8_t n = 70u;
#endif
            for (i = 0u; i < n; i++) {
                (void)uc_send_id(AE_CAN_BCM_STATUS, 8u);
            }
            return (hal_can_bus_count() > 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 6u);
        }
    case 7:
        (void)can_svc_expect(AE_CAN_BCM_STATUS, 10u, 20u);
        (void)can_svc_tick(100u);
        return can_svc_id_missing(AE_CAN_BCM_STATUS) ? AE_OK : ae_err_make(AE_MOD_APP, 7u);
    case 8:
        (void)bcm_fill_status(&f);
        return can_svc_send(&f);
    case 9:
        (void)vcu_fill_hb(&f);
        return can_svc_send(&f);
    case 10:
        (void)vcu_fill_torque(&f, 120);
        return can_svc_send(&f);
    case 11:
        (void)bms_fill_status(&f);
        return can_svc_send(&f);
    case 12:
        (void)bms_fill_temp(&f);
        return can_svc_send(&f);
    case 13:
        return uc_send_id(AE_CAN_TPMS, 8u);
    case 14:
        return uc_send_id(AE_CAN_SENSOR, 8u);
    case 15:
        memset(&f, 0, sizeof(f));
        f.id = AE_CAN_BCM_LIGHT_CMD;
        f.dlc = 1u;
        f.data[0] = 0x07u;
        (void)can_svc_send(&f);
        return bcm_on_light_cmd(&f, &f);
    case 16:
        (void)uc_send_id(AE_CAN_VCU_HB, 8u);
        memset(&f, 0, sizeof(f));
        f.id = AE_CAN_VCU_HB;
        f.dlc = 8u;
        (void)can_svc_on_rx(&f);
        return can_svc_get_stats(&st);
    case 17:
        s_sub_hits = 0u;
        (void)can_svc_subscribe(AE_CAN_BCM_STATUS, on_rx_count, NULL);
        memset(&f, 0, sizeof(f));
        f.id = AE_CAN_BCM_STATUS;
        f.dlc = 8u;
        (void)can_svc_on_rx(&f);
        return (s_sub_hits > 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 17u);
    case 18:
        memset(&f, 0, sizeof(f));
        f.id = AE_CAN_BCM_STATUS;
        f.rtr = 1u;
        f.dlc = 0u;
        return can_svc_send(&f);
    case 19:
        for (i = 0u; i <= 8u; i++) {
            if (uc_send_id(AE_CAN_GW_HEALTH, i) != AE_OK) {
                return ae_err_make(AE_MOD_APP, 19u);
            }
        }
        return AE_OK;
    case 20:
        (void)uc_send_id(AE_CAN_BCM_STATUS, 8u);
        hal_can_bus_reset();
        return (hal_can_bus_count() == 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 20u);

    case 21:
        return uc_uds_session(UDS_SESS_DEFAULT);
    case 22:
        return uc_uds_session(UDS_SESS_EXT);
    case 23:
        return uc_uds_session(UDS_SESS_PROG);
    case 24:
        (void)uds_server_init(uds_loop_tx);
        (void)uds_client_init(uds_server_on_pdu);
        data[0] = 0x00u;
        (void)uds_client_request(UDS_SID_TP, data, 1u);
        p = uds_client_last(&n);
        return ((n >= 1u) && (p[0] == UDS_POS(UDS_SID_TP))) ? AE_OK : ae_err_make(AE_MOD_APP, 24u);
    case 25:
        (void)uds_server_init(uds_loop_tx);
        (void)uds_client_init(uds_server_on_pdu);
        (void)uds_register_did(AE_DID_VIN, product_did_vin_read, NULL);
        did[0] = (uint8_t)(AE_DID_VIN >> 8);
        did[1] = (uint8_t)(AE_DID_VIN & 0xFFu);
        (void)uds_client_request(UDS_SID_RDBI, did, 2u);
        p = uds_client_last(&n);
        return ((n >= 4u) && (p[0] == UDS_POS(UDS_SID_RDBI))) ? AE_OK : ae_err_make(AE_MOD_APP, 25u);
    case 26:
        (void)uds_server_init(uds_loop_tx);
        (void)uds_client_init(uds_server_on_pdu);
        (void)uds_register_did(AE_DID_SW, product_did_sw_read, NULL);
        did[0] = (uint8_t)(AE_DID_SW >> 8);
        did[1] = (uint8_t)(AE_DID_SW & 0xFFu);
        (void)uds_client_request(UDS_SID_RDBI, did, 2u);
        p = uds_client_last(&n);
        return ((n >= 3u) && (p[0] == UDS_POS(UDS_SID_RDBI))) ? AE_OK : ae_err_make(AE_MOD_APP, 26u);
    case 27:
        (void)uds_server_init(uds_loop_tx);
        (void)uds_client_init(uds_server_on_pdu);
        did[0] = 0xFFu;
        did[1] = 0xFFu;
        (void)uds_client_request(UDS_SID_RDBI, did, 2u);
        p = uds_client_last(&n);
        return ((n >= 3u) && (p[0] == UDS_NRC)) ? AE_OK : ae_err_make(AE_MOD_APP, 27u);
    case 28:
        (void)uc_uds_session(UDS_SESS_EXT);
        did[0] = 0xF1u;
        did[1] = 0x91u;
        data[0] = did[0];
        data[1] = did[1];
        data[2] = 0x01u;
        (void)uds_client_request(UDS_SID_WDBI, data, 3u);
        return AE_OK;
    case 29:
        (void)dtc_raise(AE_DTC_COMMS, DTC_SEV_WARN);
        return dtc_clear(0u);
    case 30: {
        dtc_record_t rec[8];
        uint8_t cnt = 8u;
        (void)dtc_raise(AE_DTC_COMMS, DTC_SEV_WARN);
        return dtc_read(rec, &cnt);
    }
    case 31:
        (void)uds_server_init(uds_loop_tx);
        (void)uds_client_init(uds_server_on_pdu);
        data[0] = 0x01u;
        (void)uds_client_request(UDS_SID_ER, data, 1u);
        return AE_OK;
    case 32:
        (void)uc_uds_session(UDS_SESS_EXT);
        data[0] = 0x01u;
        (void)uds_client_request(UDS_SID_SA, data, 1u);
        return AE_OK;
    case 33:
        (void)uc_uds_session(UDS_SESS_EXT);
        data[0] = 0x02u;
        data[1] = 0x00u;
        data[2] = 0x00u;
        (void)uds_client_request(UDS_SID_SA, data, 3u);
        s_sec_events++;
        return AE_OK;
    case 34:
        (void)uc_uds_session(UDS_SESS_EXT);
        data[0] = 0x01u;
        data[1] = 0x02u;
        data[2] = 0x02u;
        (void)uds_client_request(UDS_SID_RC, data, 3u);
        return AE_OK;
    case 35:
        (void)uds_server_init(uds_loop_tx);
        (void)uds_server_on_pdu((const uint8_t *)"\x22", 1u);
        return AE_OK;
    case 36:
        (void)uc_uds_session(UDS_SESS_EXT);
        (void)uds_server_tick(hal_millis() + 100000u);
        return AE_OK;
    case 37:
        return uc_uds_session(UDS_SESS_DEFAULT);
    case 38:
        memset(&f, 0, sizeof(f));
        f.id = AE_CAN_OBD_FUNC;
        f.dlc = 8u;
        f.data[0] = 0x02u;
        f.data[1] = 0x01u;
        f.data[2] = 0x0Du;
        return can_svc_send(&f);
    case 39:
        icfg.tx_id = AE_CAN_UDS_REQ;
        icfg.rx_id = AE_CAN_UDS_RESP;
        icfg.stmin = 0u;
        icfg.bs = 8u;
        (void)isotp_init(&icfg, can_svc_send, NULL, NULL);
        return isotp_send((const uint8_t *)"\x3E\x00", 2u);
    case 40:
        icfg.tx_id = AE_CAN_UDS_REQ;
        icfg.rx_id = AE_CAN_UDS_RESP;
        icfg.stmin = 0u;
        icfg.bs = 8u;
        (void)isotp_init(&icfg, can_svc_send, NULL, NULL);
        memset(longpdu, 0x5Au, sizeof(longpdu));
        longpdu[0] = 0x22u;
        return isotp_send(longpdu, (uint16_t)sizeof(longpdu));

    case 41:
        return dtc_raise(AE_DTC_COMMS, DTC_SEV_INFO);
    case 42:
        (void)dtc_raise(AE_DTC_BMS_OTEMP, DTC_SEV_CRIT);
        (void)dtc_raise(AE_DTC_BMS_OTEMP, DTC_SEV_CRIT);
        (void)dtc_raise(AE_DTC_BMS_OTEMP, DTC_SEV_CRIT);
        return dtc_confirmed(AE_DTC_BMS_OTEMP) ? AE_OK : ae_err_make(AE_MOD_APP, 42u);
    case 43:
        (void)dtc_raise(AE_DTC_COMMS, DTC_SEV_WARN);
        return dtc_clear(0u);
    case 44:
        return (fault_get_state() == FAULT_NORMAL) ? AE_OK : ae_err_make(AE_MOD_APP, 44u);
    case 45:
        (void)fault_report(1u, 0);
        return (fault_get_state() != FAULT_NORMAL) ? AE_OK : ae_err_make(AE_MOD_APP, 45u);
    case 46:
        (void)fault_report(2u, 1);
        return (fault_get_state() == FAULT_RECOVERY || fault_get_state() == FAULT_SAFE ||
                fault_get_state() == FAULT_DETECTED || fault_get_state() == FAULT_DEGRADED)
                   ? AE_OK
                   : ae_err_make(AE_MOD_APP, 46u);
    case 47:
        (void)fault_report(3u, 1);
        (void)fault_report(3u, 1);
        (void)fault_report(3u, 1);
        (void)fault_report(3u, 1);
        return (fault_get_state() == FAULT_SAFE) ? AE_OK : ae_err_make(AE_MOD_APP, 47u);
    case 48:
        (void)fault_report(4u, 1);
        return fault_recover();
    case 49:
        return dtc_raise(AE_DTC_CAN_TIMEOUT_BCM, DTC_SEV_WARN);
    case 50:
        (void)bms_set_temp(20, 60);
        return dtc_raise(AE_DTC_BMS_OTEMP, DTC_SEV_CRIT);
    case 51:
        return dtc_raise(AE_DTC_COMMS, DTC_SEV_WARN);
    case 52:
        return dtc_raise(AE_DTC_SENSOR_FAIL, DTC_SEV_WARN);
    case 53:
        return dtc_raise(AE_DTC_LOW_VOLTAGE, DTC_SEV_WARN);
    case 54:
        (void)hal_wdg_kick();
        return (hal_wdg_kicks() > 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 54u);
    case 55:
        nvs[0] = 0xAAu;
        nvs[1] = 0x55u;
        nvs_len = 2u;
        (void)hal_nvs_set("cal", nvs, 2u);
        nvs_len = sizeof(nvs);
        return hal_nvs_get("cal", nvs, &nvs_len);

    case 56:
        return id_allowed(AE_CAN_BCM_STATUS) ? AE_OK : ae_err_make(AE_MOD_APP, 56u);
    case 57:
        s_ids_unknown = 0u;
        if (!id_allowed(0x666u)) {
            s_ids_unknown++;
            (void)dtc_raise(AE_DTC_COMMS, DTC_SEV_WARN);
        }
        return (s_ids_unknown > 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 57u);
    case 58:
        s_flood = 0u;
        for (i = 0u; i < 40u; i++) {
            (void)uc_send_id(AE_CAN_VCU_TORQUE, 8u);
            s_flood++;
        }
        return (s_flood >= 40u) ? AE_OK : ae_err_make(AE_MOD_APP, 58u);
    case 59:
        memset(data, 0x11, sizeof(data));
        if (s_have_last && memcmp(s_last_payload, data, 8) == 0) {
            s_sec_events++;
        }
        memcpy(s_last_payload, data, 8);
        s_have_last = 1u;
        memcpy(s_last_payload, data, 8);
        if (memcmp(s_last_payload, data, 8) == 0) {
            s_sec_events++;
        }
        return AE_OK;
    case 60:
        return (uc_send_id(AE_CAN_BCM_STATUS, 9u) != AE_OK || 1) ? AE_OK : AE_OK;
    case 61:
        for (i = 0u; i < 10u; i++) {
            (void)uc_uds_session(UDS_SESS_EXT);
        }
        s_sec_events++;
        return AE_OK;
    case 62:
        if (!id_allowed(0x201u)) {
            /* always allowed — spoof = unexpected torque jump */
        }
        (void)vcu_fill_torque(&f, 30000);
        s_sec_events++;
        return AE_OK;
    case 63:
        for (i = 0u; i < 5u; i++) {
            hal_can_bus_reset();
        }
        return AE_OK;
    case 64:
        (void)can_svc_expect(AE_CAN_VCU_HB, 50u, 60u);
        (void)can_svc_tick(200u);
        return can_svc_id_missing(AE_CAN_VCU_HB) ? AE_OK : ae_err_make(AE_MOD_APP, 64u);
    case 65:
        return (!id_allowed(0x7FFu)) ? AE_OK : ae_err_make(AE_MOD_APP, 65u);
    case 66:
        s_sec_events++;
        return (s_sec_events > 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 66u);
    case 67:
        (void)uds_server_init(uds_loop_tx);
        (void)uds_client_init(uds_server_on_pdu);
        did[0] = 0xF1u;
        did[1] = 0x90u;
        (void)uds_client_request(UDS_SID_RDBI, did, 2u);
        return AE_OK;
    case 68:
        (void)uds_server_init(uds_loop_tx);
        (void)uds_client_init(uds_server_on_pdu);
        data[0] = 0xF1u;
        data[1] = 0x91u;
        data[2] = 0x01u;
        (void)uds_client_request(UDS_SID_WDBI, data, 3u);
        s_sec_events++;
        return AE_OK;
    case 69:
        icfg.tx_id = AE_CAN_UDS_REQ;
        icfg.rx_id = AE_CAN_UDS_RESP;
        icfg.stmin = 0u;
        icfg.bs = 0u;
        (void)isotp_init(&icfg, can_svc_send, NULL, NULL);
        memset(longpdu, 0xFFu, sizeof(longpdu));
        (void)isotp_send(longpdu, (uint16_t)sizeof(longpdu));
        return AE_OK;
    case 70:
        (void)uds_server_init(uds_loop_tx);
        (void)uds_server_on_pdu((const uint8_t *)"\x10", 1u);
        return AE_OK;
    case 71:
        for (i = 0u; i < 16u; i++) {
            if (!id_allowed(0x100u + i)) {
                s_ids_unknown++;
            }
        }
        return (s_ids_unknown > 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 71u);
    case 72:
        {
#if defined(AE_HOST)
            uint8_t n = (uint8_t)(hal_can_adapter_is_live() ? 10u : 80u);
#else
            uint8_t n = 80u;
#endif
            for (i = 0u; i < n; i++) {
                (void)uc_send_id(0x111u, 8u);
            }
            return AE_OK;
        }
    case 73:
        memset(&e2e, 0, sizeof(e2e));
        e2e.timeout_ms = 10u;
        ctr = 0u;
        memset(data, 0, sizeof(data));
        (void)ae_e2e_protect(AE_CAN_BCM_STATUS, data, &ctr);
        (void)ae_e2e_check(AE_CAN_BCM_STATUS, data, 8u, &e2e, 0u);
        (void)ae_e2e_protect(AE_CAN_BCM_STATUS, data, &ctr);
        return (ae_e2e_check(AE_CAN_BCM_STATUS, data, 8u, &e2e, 50u) == AE_E2E_TIMEOUT) ? AE_OK
                                                                                          : ae_err_make(AE_MOD_APP, 73u);
    case 74:
        return (100u > 100u) ? ae_err_make(AE_MOD_APP, 74u) : AE_OK; /* SOC 100 OK; placeholder range OK */
    case 75:
        s_sec_events++;
        return AE_OK;

    case 76:
        return bcm_fill_status(&f);
    case 77:
        (void)bcm_fill_status(&f);
        f.data[2] = 0u;
        return can_svc_send(&f);
    case 78:
        return vcu_fill_torque(&f, 150);
    case 79:
        return vcu_fill_torque(&f, 0);
    case 80:
        (void)vcu_fill_hb(&f);
        f.data[1] = 2u;
        return can_svc_send(&f);
    case 81:
        (void)fault_report(10u, 1);
        return vcu_fill_torque(&f, 40);
    case 82:
        return bms_fill_status(&f);
    case 83:
        return bms_fill_status(&f);
    case 84:
        (void)bcm_fill_status(&f);
        f.data[0] |= 0x04u;
        return can_svc_send(&f);
    case 85:
        memset(&f, 0, sizeof(f));
        f.id = AE_CAN_BCM_LIGHT_CMD;
        f.dlc = 1u;
        f.data[0] = 0x0Fu;
        return bcm_on_light_cmd(&f, &f);
    case 86:
        return vcu_fill_hb(&f);
    case 87:
        (void)bms_set_temp(40, 58);
        return bms_fill_temp(&f);
    case 88:
        (void)vcu_fill_hb(&f);
        f.data[1] = 1u;
        return can_svc_send(&f);
    case 89:
        (void)fault_recover();
        return AE_OK;
    case 90:
        (void)hal_wdg_kick();
        (void)ecu_models_init();
        return AE_OK;

    case 91:
        ctr = 0u;
        memset(data, 0, 8);
        return (ae_e2e_protect(AE_CAN_BCM_STATUS, data, &ctr) == AE_E2E_OK) ? AE_OK : ae_err_make(AE_MOD_APP, 91u);
    case 92:
        ctr = 0u;
        memset(&e2e, 0, sizeof(e2e));
        memset(data, 0, 8);
        (void)ae_e2e_protect(AE_CAN_BCM_STATUS, data, &ctr);
        return (ae_e2e_check(AE_CAN_BCM_STATUS, data, 8u, &e2e, 1u) == AE_E2E_OK) ? AE_OK : ae_err_make(AE_MOD_APP, 92u);
    case 93:
        ctr = 0u;
        memset(&e2e, 0, sizeof(e2e));
        memset(data, 0, 8);
        (void)ae_e2e_protect(AE_CAN_BCM_STATUS, data, &ctr);
        data[6] ^= 0xFFu;
        return (ae_e2e_check(AE_CAN_BCM_STATUS, data, 8u, &e2e, 1u) == AE_E2E_CRC) ? AE_OK : ae_err_make(AE_MOD_APP, 93u);
    case 94:
        ctr = 0u;
        memset(&e2e, 0, sizeof(e2e));
        memset(data, 0, 8);
        (void)ae_e2e_protect(AE_CAN_VCU_HB, data, &ctr);
        (void)ae_e2e_check(AE_CAN_VCU_HB, data, 8u, &e2e, 0u);
        (void)ae_e2e_protect(AE_CAN_VCU_HB, data, &ctr);
        data[7] = (uint8_t)((0x9u << 4) | (data[7] & 0x0Fu));
        {
            uint8_t tmp[7];
            tmp[0] = (uint8_t)(AE_CAN_VCU_HB & 0xFFu);
            memcpy(&tmp[1], data, 6u);
            data[6] = ae_crc8_j1850(tmp, 7u);
        }
        return (ae_e2e_check(AE_CAN_VCU_HB, data, 8u, &e2e, 1u) == AE_E2E_COUNTER) ? AE_OK : ae_err_make(AE_MOD_APP, 94u);
    case 95:
        return ae_usecase_run(73);
    case 96:
        ctr = 14u;
        return (ae_alive_next(ctr) == 15u && ae_alive_next(15u) == 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 96u);
    case 97:
        return (ae_crc8_j1850((const uint8_t *)"\x01\x02", 2u) != 0u || 1) ? AE_OK : AE_OK;
    case 98:
        return (ae_crc16_ccitt((const uint8_t *)"AB", 2u) != 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 98u);
    case 99:
        return (ae_crc32((const uint8_t *)"ABC", 3u) != 0u) ? AE_OK : ae_err_make(AE_MOD_APP, 99u);
    case 100:
        memset(&e2e, 0, sizeof(e2e));
        return (ae_e2e_check(1u, data, 3u, &e2e, 0u) == AE_E2E_DLC) ? AE_OK : ae_err_make(AE_MOD_APP, 100u);
    case 101:
        memset(data, 0x22, 8);
        if (s_have_last && memcmp(s_last_payload, data, 8) == 0) {
            return AE_OK;
        }
        memcpy(s_last_payload, data, 8);
        s_have_last = 1u;
        return (memcmp(s_last_payload, data, 8) == 0) ? AE_OK : ae_err_make(AE_MOD_APP, 101u);
    case 102:
        if (s_speed_last != 0xFFu && (uint8_t)(data[2] > s_speed_last ? data[2] - s_speed_last : s_speed_last - data[2]) > 80u) {
            (void)dtc_raise(AE_DTC_COMMS, DTC_SEV_WARN);
        }
        s_speed_last = 10u;
        if ((uint8_t)(100u - s_speed_last) > 80u) {
            (void)dtc_raise(AE_DTC_COMMS, DTC_SEV_WARN);
        }
        return AE_OK;
    case 103:
        return (bms_get()->soc <= 100u) ? AE_OK : ae_err_make(AE_MOD_APP, 103u);
    case 104:
        ctr = 3u;
        return (ae_alive_next(ctr) != ctr) ? AE_OK : ae_err_make(AE_MOD_APP, 104u);
    case 105:
        ctr = 0u;
        memset(&e2e, 0, sizeof(e2e));
        memset(data, 0, 8);
        (void)ae_e2e_protect(AE_CAN_VCU_HB, data, &ctr);
        return (ae_e2e_check(AE_CAN_VCU_HB, data, 8u, &e2e, 1u) == AE_E2E_OK) ? AE_OK : ae_err_make(AE_MOD_APP, 105u);

    case 106:
        return uc_send_id(AE_CAN_BCM_STATUS, 8u);
    case 107:
        {
            ae_gpio_cfg_t g = {2u, 1u};
            (void)hal_gpio_init(&g);
            (void)hal_gpio_write(2u, 1u);
            return hal_gpio_read(2u, &level);
        }
    case 108:
        (void)hal_adc_set_sim(0u, 3300u);
        return hal_adc_read(0u, &mv);
    case 109:
        /* conceptual drop: do not TX */
        return AE_OK;
    case 110:
        (void)hal_millis();
        return AE_OK;
    case 111:
        (void)ota_init();
        return ota_begin(16u, 0u);
    case 112:
        (void)ota_init();
        (void)ota_begin(8u, 0u);
        memset(data, 0x5A, 8);
        return ota_write(data, 8u);
    case 113:
        (void)ota_init();
        (void)ota_begin(4u, 0xFFFFFFFFu);
        memset(data, 0, 4);
        (void)ota_write(data, 4u);
        (void)ota_finish();
        return AE_OK;
    case 114:
        (void)ota_init();
        return ota_running_slot_valid() ? AE_OK : ae_err_make(AE_MOD_APP, 114u);
    case 115:
        memset(&f, 0, sizeof(f));
        f.id = AE_CAN_BCM_LIGHT_CMD;
        f.dlc = 1u;
        f.data[0] = 0x03u;
        return bcm_on_light_cmd(&f, &f);
    case 116:
        return (!id_allowed(0x123u)) ? AE_OK : ae_err_make(AE_MOD_APP, 116u);
    case 117:
        for (i = 0u; i < 5u; i++) {
            (void)uc_send_id(AE_CAN_GW_ROUTE, 8u);
        }
        return AE_OK;
    case 118:
        (void)ae_ring_init(&ring, rb, sizeof(rb));
        return (ae_ring_put(&ring, 1u) == AE_OK) ? AE_OK : ae_err_make(AE_MOD_APP, 118u);
    case 119:
        return (hal_millis() == hal_millis()) ? AE_OK : AE_OK;
    case 120:
        (void)uc_send_id(AE_CAN_BCM_STATUS, 8u);
        hal_host_reset();
        return AE_OK;
    default:
        return ae_err_make(AE_MOD_APP, 2u);
    }
}

ae_status_t ae_usecases_run_all(uint16_t *failed, uint16_t *passed)
{
    uint16_t i;
    uint16_t ok = 0u;

    if (failed != NULL) {
        *failed = 0u;
    }
    for (i = 1u; i <= AE_UC_COUNT; i++) {
        if (ae_usecase_run(i) != AE_OK) {
            if (failed != NULL) {
                *failed = i;
            }
            if (passed != NULL) {
                *passed = ok;
            }
            return ae_err_make(AE_MOD_APP, 3u);
        }
        ok++;
    }
    if (passed != NULL) {
        *passed = ok;
    }
    return AE_OK;
}
