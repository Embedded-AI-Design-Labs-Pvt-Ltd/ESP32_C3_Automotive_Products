/**
 * @file hal_can.h
 * @brief CAN HAL contract. Host uses an in-memory bus; MCU uses TWAI.
 */
#ifndef HAL_CAN_H
#define HAL_CAN_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t bitrate;
} ae_can_cfg_t;

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

#ifdef __cplusplus
}
#endif

#endif /* HAL_CAN_H */
