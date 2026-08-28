/**
 * @file hal_can_adapter.h
 * @brief Host USB-to-CAN adapter selection (CANable / PCAN-USB / SLCAN / SocketCAN).
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 * @note Lab/HIL only. Products include this only on AE_HOST builds.
 *
 * Environment (optional):
 *   AE_CAN_BACKEND = sim | socketcan | pcan | slcan
 *   AE_CAN_IFACE   = can0 | PCAN_USBBUS1 | COM3 | /dev/ttyACM0
 *   AE_CAN_BITRATE = 500000 (default)
 */
#ifndef HAL_CAN_ADAPTER_H
#define HAL_CAN_ADAPTER_H

#include "hal_can.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Fill cfg from environment. Leaves bitrate default 500 kbit/s if unset. */
ae_status_t hal_can_cfg_from_env(ae_can_cfg_t *cfg);

/** Explicit configure before hal_can_init (copies into process defaults). */
ae_status_t hal_can_adapter_set(uint8_t backend, const char *iface, uint32_t bitrate);

/** Active backend after last successful hal_can_init (or set). */
uint8_t hal_can_backend(void);

/** Non-zero when talking to a physical USB-CAN adapter (not sim). */
int hal_can_adapter_is_live(void);

const char *hal_can_backend_name(uint8_t backend);

/** Fill cfg from environment then open (sim if unset). */
ae_status_t hal_can_init_from_env(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_CAN_ADAPTER_H */
