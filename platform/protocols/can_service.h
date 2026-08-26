/**
 * @file can_service.h
 * @brief Automotive CAN service above HAL (filter, expect, stats).
 */
#ifndef CAN_SERVICE_H
#define CAN_SERVICE_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*can_svc_rx_cb_t)(const ae_can_frame_t *frame, void *ctx);

typedef struct {
    uint32_t rx;
    uint32_t tx;
    uint32_t drop;
    uint32_t missing;
    uint32_t unexpected;
} can_svc_stats_t;

ae_status_t can_svc_init(void);
ae_status_t can_svc_send(const ae_can_frame_t *frame);
ae_status_t can_svc_on_rx(const ae_can_frame_t *frame);
ae_status_t can_svc_subscribe(uint32_t id, can_svc_rx_cb_t cb, void *ctx);
ae_status_t can_svc_expect(uint32_t id, uint32_t period_ms, uint32_t timeout_ms);
ae_status_t can_svc_tick(uint32_t now_ms);
ae_status_t can_svc_get_stats(can_svc_stats_t *out);
int can_svc_id_missing(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* CAN_SERVICE_H */
