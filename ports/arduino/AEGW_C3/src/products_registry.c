/**
 * @file products_registry.c
 * @brief Table of all 17 product compositions.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "product_api.h"

static const ae_product_desc_t s_products[] = {
    {"P01", "BLE-CAN Gateway", p01_ble_can_gateway_init, p01_ble_can_gateway_run},
    {"P02", "BLE OBD-II Adapter", p02_ble_obd_init, p02_ble_obd_run},
    {"P03", "UDS Diagnostic Tester", p03_uds_tester_init, p03_uds_tester_run},
    {"P04", "CAN Data Logger", p04_can_logger_init, p04_can_logger_run},
    {"P05", "ECU Communication Analyzer", p05_ecu_analyzer_init, p05_ecu_analyzer_run},
    {"P06", "ECU Test Box", p06_ecu_test_box_init, p06_ecu_test_box_run},
    {"P07", "ECU Simulator", p07_ecu_simulator_init, p07_ecu_simulator_run},
    {"P08", "ECU Fault Injection Box", p08_fault_inject_init, p08_fault_inject_run},
    {"P09", "ECU End-of-Line Tester", p09_eol_tester_init, p09_eol_tester_run},
    {"P10", "ECU Config/Calibration", p10_config_cal_init, p10_config_cal_run},
    {"P11", "Automotive Sensor Gateway", p11_sensor_gw_init, p11_sensor_gw_run},
    {"P12", "TPMS BLE Gateway", p12_tpms_gw_init, p12_tpms_gw_run},
    {"P13", "EV Battery Diagnostic Gateway", p13_ev_batt_gw_init, p13_ev_batt_gw_run},
    {"P14", "CAN Intrusion/Anomaly Monitor", p14_can_ids_init, p14_can_ids_run},
    {"P15", "HIL/RCP Interface", p15_hil_rcp_init, p15_hil_rcp_run},
    {"P16", "Automotive Training Kit", p16_training_init, p16_training_run},
    {"P17", "ECU Bootloader/OTA Prototype", p17_ota_init, p17_ota_run},
};

const ae_product_desc_t *ae_products_table(uint8_t *count)
{
    if (count != NULL) {
        *count = (uint8_t)(sizeof(s_products) / sizeof(s_products[0]));
    }
    return s_products;
}
