/**
 * @file ae_types.h
 * @brief Shared scalar types for the AEGW-C3 platform.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 * @note Host and MCU. No ESP-IDF includes.
 * @ownership Callers own objects they allocate. This header allocates nothing.
 */
#ifndef AE_TYPES_H
#define AE_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 0 = success. Non-zero = (module << 8) | reason. */
typedef int32_t ae_status_t;

#define AE_OK ((ae_status_t)0)

#define AE_MOD_HAL   ((ae_status_t)0x01)
#define AE_MOD_DRV   ((ae_status_t)0x02)
#define AE_MOD_OS    ((ae_status_t)0x03)
#define AE_MOD_CAN   ((ae_status_t)0x10)
#define AE_MOD_BLE   ((ae_status_t)0x11)
#define AE_MOD_ISOTP ((ae_status_t)0x12)
#define AE_MOD_UDS   ((ae_status_t)0x13)
#define AE_MOD_DTC   ((ae_status_t)0x14)
#define AE_MOD_SEC   ((ae_status_t)0x15)
#define AE_MOD_OTA   ((ae_status_t)0x16)
#define AE_MOD_APP   ((ae_status_t)0x20)

#define AE_ERR(mod, reason) ((ae_status_t)(((mod) << 8) | ((reason) & 0xFFu)))

#define AE_CAN_DATA_MAX (8u)

/**
 * Canonical CAN 2.0 frame. Copy by value on the data path.
 * timestamp_us is free-running microseconds from the port clock.
 */
typedef struct {
    uint32_t id;
    uint8_t ide;
    uint8_t rtr;
    uint8_t dlc;
    uint8_t flags;
    uint8_t data[AE_CAN_DATA_MAX];
    uint32_t timestamp_us;
} ae_can_frame_t;

#ifdef __cplusplus
}
#endif

#endif /* AE_TYPES_H */
