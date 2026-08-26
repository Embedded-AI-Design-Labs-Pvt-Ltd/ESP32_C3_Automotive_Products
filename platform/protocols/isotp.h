/**
 * @file isotp.h
 * @brief ISO-TP prototype: SF, FF, CF, FC, timeouts.
 */
#ifndef ISOTP_H
#define ISOTP_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AE_ISOTP_MAX 128u

typedef void (*isotp_pdu_cb_t)(const uint8_t *pdu, uint16_t len, void *ctx);
typedef ae_status_t (*isotp_can_tx_t)(const ae_can_frame_t *frame);

typedef struct {
    uint32_t tx_id;
    uint32_t rx_id;
    uint8_t stmin;
    uint8_t bs;
} isotp_cfg_t;

ae_status_t isotp_init(const isotp_cfg_t *cfg, isotp_can_tx_t tx, isotp_pdu_cb_t cb, void *ctx);
ae_status_t isotp_send(const uint8_t *pdu, uint16_t len);
ae_status_t isotp_on_can_frame(const ae_can_frame_t *frame);
ae_status_t isotp_tick(uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif /* ISOTP_H */
