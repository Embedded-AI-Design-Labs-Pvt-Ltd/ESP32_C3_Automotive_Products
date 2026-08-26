/**
 * @file isotp.c
 * @brief ISO 15765-2 prototype state machine (N_As / N_Cr simplified).
 */

#include "isotp.h"

#include "ae_error.h"

#include <string.h>

typedef enum {
    ISOTP_IDLE = 0,
    ISOTP_TX_WAIT_FC,
    ISOTP_TX_CF,
    ISOTP_RX_CF
} isotp_state_t;

static isotp_cfg_t s_cfg;
static isotp_can_tx_t s_tx;
static isotp_pdu_cb_t s_cb;
static void *s_ctx;
static isotp_state_t s_st;
static uint8_t s_buf[AE_ISOTP_MAX];
static uint16_t s_len;
static uint16_t s_idx;
static uint8_t s_sn;
static uint32_t s_deadline;

static ae_status_t send_can(uint8_t pci0, const uint8_t *data, uint8_t n)
{
    ae_can_frame_t f;
    uint8_t i;

    memset(&f, 0, sizeof(f));
    f.id = s_cfg.tx_id;
    f.dlc = 8u;
    f.data[0] = pci0;
    for (i = 0; i < n; i++) {
        f.data[1u + i] = data[i];
    }
    if (s_tx == NULL) {
        return ae_err_make(AE_MOD_ISOTP, 1u);
    }
    return s_tx(&f);
}

ae_status_t isotp_init(const isotp_cfg_t *cfg, isotp_can_tx_t tx, isotp_pdu_cb_t cb, void *ctx)
{
    if ((cfg == NULL) || (tx == NULL)) {
        return ae_err_make(AE_MOD_ISOTP, 1u);
    }
    s_cfg = *cfg;
    s_tx = tx;
    s_cb = cb;
    s_ctx = ctx;
    s_st = ISOTP_IDLE;
    return AE_OK;
}

ae_status_t isotp_send(const uint8_t *pdu, uint16_t len)
{
    if ((pdu == NULL) || (len == 0u) || (len > AE_ISOTP_MAX)) {
        return ae_err_make(AE_MOD_ISOTP, 1u);
    }
    if (len <= 7u) {
        /* Single Frame: PCI 0x0 | length */
        return send_can((uint8_t)(0x00u | len), pdu, (uint8_t)len);
    }
    memcpy(s_buf, pdu, len);
    s_len = len;
    s_idx = 6u;
    s_sn = 1u;
    s_st = ISOTP_TX_WAIT_FC;
    /* First Frame: 1 | len[11:8], len[7:0], then 6 data bytes */
    {
        uint8_t head[7];
        head[0] = (uint8_t)(len & 0xFFu);
        memcpy(&head[1], pdu, 6u);
        return send_can((uint8_t)(0x10u | ((len >> 8) & 0x0Fu)), head, 7u);
    }
}

ae_status_t isotp_on_can_frame(const ae_can_frame_t *frame)
{
    uint8_t pci;
    uint8_t type;
    uint16_t n;

    if ((frame == NULL) || (frame->id != s_cfg.rx_id)) {
        return AE_OK;
    }
    pci = frame->data[0];
    type = (uint8_t)(pci >> 4);
    if (type == 0x0u) {
        n = (uint16_t)(pci & 0x0Fu);
        if ((n > 0u) && (n <= 7u) && (s_cb != NULL)) {
            s_cb(&frame->data[1], n, s_ctx);
        }
        return AE_OK;
    }
    if (type == 0x1u) {
        s_len = (uint16_t)(((uint16_t)(pci & 0x0Fu) << 8) | frame->data[1]);
        if (s_len > AE_ISOTP_MAX) {
            s_st = ISOTP_IDLE;
            return ae_err_make(AE_MOD_ISOTP, 2u);
        }
        memcpy(s_buf, &frame->data[2], 6u);
        s_idx = 6u;
        s_sn = 1u;
        s_st = ISOTP_RX_CF;
        /* Flow Control CTS, BS=0, STmin */
        return send_can(0x30u, NULL, 0u);
    }
    if (type == 0x3u) {
        if (s_st == ISOTP_TX_WAIT_FC) {
            s_st = ISOTP_TX_CF;
        }
        return AE_OK;
    }
    if ((type == 0x2u) && (s_st == ISOTP_RX_CF)) {
        uint8_t sn = (uint8_t)(pci & 0x0Fu);
        if (sn != (s_sn & 0x0Fu)) {
            s_st = ISOTP_IDLE;
            return ae_err_make(AE_MOD_ISOTP, 3u);
        }
        {
            uint8_t take = 7u;
            if ((s_idx + take) > s_len) {
                take = (uint8_t)(s_len - s_idx);
            }
            memcpy(&s_buf[s_idx], &frame->data[1], take);
            s_idx = (uint16_t)(s_idx + take);
            s_sn++;
            if (s_idx >= s_len) {
                s_st = ISOTP_IDLE;
                if (s_cb != NULL) {
                    s_cb(s_buf, s_len, s_ctx);
                }
            }
        }
    }
    return AE_OK;
}

ae_status_t isotp_tick(uint32_t now_ms)
{
    if ((s_st == ISOTP_TX_CF) && (s_idx < s_len)) {
        uint8_t take = 7u;
        uint8_t pci;

        if ((s_idx + take) > s_len) {
            take = (uint8_t)(s_len - s_idx);
        }
        pci = (uint8_t)(0x20u | (s_sn & 0x0Fu));
        if (send_can(pci, &s_buf[s_idx], take) == AE_OK) {
            s_idx = (uint16_t)(s_idx + take);
            s_sn++;
            if (s_idx >= s_len) {
                s_st = ISOTP_IDLE;
            }
        }
        s_deadline = now_ms + 50u;
    }
    (void)s_deadline;
    return AE_OK;
}
