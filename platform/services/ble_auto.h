/**
 * @file ble_auto.h
 * @brief BLE automotive GATT transport (host shim or NimBLE on C3).
 */
#ifndef BLE_AUTO_H
#define BLE_AUTO_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AE_BLE_CHAR_CAN_CMD        1u
#define AE_BLE_CHAR_DIAG_CMD       2u
#define AE_BLE_CHAR_VEHICLE_STATUS 3u
#define AE_BLE_CHAR_BATT_STATUS    4u
#define AE_BLE_CHAR_DTC            5u
#define AE_BLE_CHAR_CONFIG         6u
#define AE_BLE_CHAR_SENSOR         7u
#define AE_BLE_CHAR_LOGGER         8u
#define AE_BLE_CHAR_TPMS           9u
#define AE_BLE_CHAR_OTA            10u

typedef void (*ble_auto_write_cb_t)(uint16_t char_id, const uint8_t *data, uint16_t len, void *ctx);

ae_status_t ble_auto_init(ble_auto_write_cb_t cb, void *ctx);
ae_status_t ble_auto_notify(uint16_t char_id, const uint8_t *data, uint16_t len);
ae_status_t ble_auto_inject_write(uint16_t char_id, const uint8_t *data, uint16_t len);
const uint8_t *ble_auto_last_notify(uint16_t *char_id, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif /* BLE_AUTO_H */
