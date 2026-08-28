/**
 * @file hal_can.h
 * @brief CAN HAL contract. Host: sim or USB adapter; MCU: TWAI.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#ifndef HAL_CAN_H
#define HAL_CAN_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Host backend. MCU ports ignore backend and use on-chip CAN/TWAI. */
#define AE_CAN_BE_SIM       0u /* In-memory Virtual ECU bus (unit tests) */
#define AE_CAN_BE_SOCKETCAN 1u /* Linux: CANable gs_usb / PCAN peak_usb / any SocketCAN */
#define AE_CAN_BE_PCAN      2u /* Windows/Linux: PEAK PCAN-Basic API */
#define AE_CAN_BE_SLCAN     3u /* CANable (or clone) in SLCAN serial mode */

typedef struct {
    uint32_t bitrate;     /**< Classical CAN bitrate (e.g. 500000). */
    uint8_t backend;      /**< AE_CAN_BE_* ; 0 = sim. */
    char iface[32];       /**< "can0", "PCAN_USBBUS1", "COM3", "/dev/ttyACM0". */
} ae_can_cfg_t;

/** Fully-initialized Classical CAN @ 500 kbit/s, sim backend (host). */
#define AE_CAN_CFG_500K \
    { .bitrate = 500000u, .backend = AE_CAN_BE_SIM, .iface = {0} }

typedef struct {
    uint32_t id;
    uint32_t mask;
} ae_can_filter_t;

typedef void (*ae_can_rx_isr_cb_t)(const ae_can_frame_t *frame, void *ctx);

ae_status_t hal_can_init(const ae_can_cfg_t *cfg);
ae_status_t hal_can_send(const ae_can_frame_t *frame, uint32_t timeout_ms);
ae_status_t hal_can_recv(ae_can_frame_t *frame);
ae_status_t hal_can_set_filter(const ae_can_filter_t *filters, uint8_t count);
ae_status_t hal_can_attach_rx(ae_can_rx_isr_cb_t cb, void *ctx);
void hal_can_bus_reset(void);
uint32_t hal_can_bus_count(void);
void hal_host_reset(void);
uint32_t hal_millis(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_CAN_H */
