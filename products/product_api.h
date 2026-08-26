/**
 * @file product_api.h
 * @brief Common product composition interface. One binary selects one product.
 */
#ifndef PRODUCT_API_H
#define PRODUCT_API_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ae_status_t (*ae_product_fn_t)(void);

typedef struct {
    const char *id;
    const char *name;
    ae_product_fn_t init;
    ae_product_fn_t run; /**< Real-time use-case scenario (host or HIL). */
} ae_product_desc_t;

const ae_product_desc_t *ae_products_table(uint8_t *count);

ae_status_t p01_ble_can_gateway_init(void);
ae_status_t p01_ble_can_gateway_run(void);
ae_status_t p02_ble_obd_init(void);
ae_status_t p02_ble_obd_run(void);
ae_status_t p03_uds_tester_init(void);
ae_status_t p03_uds_tester_run(void);
ae_status_t p04_can_logger_init(void);
ae_status_t p04_can_logger_run(void);
ae_status_t p05_ecu_analyzer_init(void);
ae_status_t p05_ecu_analyzer_run(void);
ae_status_t p06_ecu_test_box_init(void);
ae_status_t p06_ecu_test_box_run(void);
ae_status_t p07_ecu_simulator_init(void);
ae_status_t p07_ecu_simulator_run(void);
ae_status_t p08_fault_inject_init(void);
ae_status_t p08_fault_inject_run(void);
ae_status_t p09_eol_tester_init(void);
ae_status_t p09_eol_tester_run(void);
ae_status_t p10_config_cal_init(void);
ae_status_t p10_config_cal_run(void);
ae_status_t p11_sensor_gw_init(void);
ae_status_t p11_sensor_gw_run(void);
ae_status_t p12_tpms_gw_init(void);
ae_status_t p12_tpms_gw_run(void);
ae_status_t p13_ev_batt_gw_init(void);
ae_status_t p13_ev_batt_gw_run(void);
ae_status_t p14_can_ids_init(void);
ae_status_t p14_can_ids_run(void);
ae_status_t p15_hil_rcp_init(void);
ae_status_t p15_hil_rcp_run(void);
ae_status_t p16_training_init(void);
ae_status_t p16_training_run(void);
ae_status_t p17_ota_init(void);
ae_status_t p17_ota_run(void);

#ifdef __cplusplus
}
#endif

#endif /* PRODUCT_API_H */
