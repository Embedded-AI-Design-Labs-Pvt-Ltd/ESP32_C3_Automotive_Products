/**
 * @file uds.c
 * @brief Diagnostic session, DID, DTC, security-access prototype.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "uds.h"

#include "ae_can_ids.h"
#include "ae_error.h"
#include "dtc.h"

#include <string.h>

#define AE_DID_MAX 8u
#define AE_RID_MAX 4u

typedef struct {
    uint16_t did;
    uds_did_read_t rd;
    uds_did_write_t wr;
    int used;
} ae_did_slot_t;

typedef struct {
    uint16_t rid;
    uds_routine_t fn;
    int used;
} ae_rid_slot_t;

static uds_tx_t s_tx;
static uds_session_t s_sess;
static ae_did_slot_t s_dids[AE_DID_MAX];
static ae_rid_slot_t s_rids[AE_RID_MAX];
static uint32_t s_last_ms;

static uint8_t s_cli_last[128];
static uint16_t s_cli_len;
static uds_tx_t s_cli_tx;

static void nrc(uint8_t sid, uint8_t code)
{
    uint8_t p[3];

    p[0] = UDS_NRC;
    p[1] = sid;
    p[2] = code;
    if (s_tx != NULL) {
        (void)s_tx(p, 3u);
    }
}

static void pos(const uint8_t *p, uint16_t n)
{
    if (s_tx != NULL) {
        (void)s_tx(p, n);
    }
}

ae_status_t uds_server_init(uds_tx_t tx)
{
    memset(&s_sess, 0, sizeof(s_sess));
    memset(s_dids, 0, sizeof(s_dids));
    memset(s_rids, 0, sizeof(s_rids));
    s_tx = tx;
    s_sess.session = UDS_SESS_DEFAULT;
    s_sess.seed = 0x1234u;
    return AE_OK;
}

ae_status_t uds_register_did(uint16_t did, uds_did_read_t rd, uds_did_write_t wr)
{
    uint8_t i;

    for (i = 0; i < AE_DID_MAX; i++) {
        if (s_dids[i].used == 0) {
            s_dids[i].did = did;
            s_dids[i].rd = rd;
            s_dids[i].wr = wr;
            s_dids[i].used = 1;
            return AE_OK;
        }
    }
    return ae_err_make(AE_MOD_UDS, 2u);
}

ae_status_t uds_register_routine(uint16_t rid, uds_routine_t fn)
{
    uint8_t i;

    for (i = 0; i < AE_RID_MAX; i++) {
        if (s_rids[i].used == 0) {
            s_rids[i].rid = rid;
            s_rids[i].fn = fn;
            s_rids[i].used = 1;
            return AE_OK;
        }
    }
    return ae_err_make(AE_MOD_UDS, 2u);
}

const uds_session_t *uds_server_session(void)
{
    return &s_sess;
}

ae_status_t uds_server_tick(uint32_t now_ms)
{
    /* S3 client timeout returns to default session. */
    if ((s_sess.session != UDS_SESS_DEFAULT) && ((now_ms - s_last_ms) > 5000u)) {
        s_sess.session = UDS_SESS_DEFAULT;
        s_sess.unlocked = 0u;
    }
    return AE_OK;
}

ae_status_t uds_server_on_pdu(const uint8_t *pdu, uint16_t len)
{
    uint8_t sid;
    uint8_t out[80];
    uint16_t n = 0u;

    if ((pdu == NULL) || (len == 0u)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    sid = pdu[0];
    s_last_ms = 0u;

    if (sid == UDS_SID_DSC) {
        if (len < 2u) {
            nrc(sid, 0x13u);
            return AE_OK;
        }
        s_sess.session = pdu[1];
        out[0] = UDS_POS(sid);
        out[1] = pdu[1];
        out[2] = 0x00u;
        out[3] = 0x32u; /* P2 proto 50 ms */
        pos(out, 4u);
        return AE_OK;
    }
    if (sid == UDS_SID_ER) {
        out[0] = UDS_POS(sid);
        out[1] = (len > 1u) ? pdu[1] : 0x01u;
        pos(out, 2u);
        return AE_OK;
    }
    if (sid == UDS_SID_TP) {
        out[0] = UDS_POS(sid);
        pos(out, 1u);
        return AE_OK;
    }
    if (sid == UDS_SID_CDTCI) {
        (void)dtc_clear(0xFFFFFFFFu);
        out[0] = UDS_POS(sid);
        pos(out, 1u);
        return AE_OK;
    }
    if (sid == UDS_SID_RDTCI) {
        dtc_record_t recs[8];
        uint8_t cnt = 8u;
        uint8_t i;

        (void)dtc_read(recs, &cnt);
        out[0] = UDS_POS(sid);
        out[1] = (len > 1u) ? pdu[1] : 0x02u;
        n = 2u;
        for (i = 0; (i < cnt) && (n + 4u <= sizeof(out)); i++) {
            out[n++] = (uint8_t)((recs[i].code >> 16) & 0xFFu);
            out[n++] = (uint8_t)((recs[i].code >> 8) & 0xFFu);
            out[n++] = (uint8_t)(recs[i].code & 0xFFu);
            out[n++] = recs[i].status;
        }
        pos(out, n);
        return AE_OK;
    }
    if (sid == UDS_SID_RDBI) {
        uint16_t did;
        uint8_t i;
        uint16_t dlen = (uint16_t)(sizeof(out) - 3u);

        if (len < 3u) {
            nrc(sid, 0x13u);
            return AE_OK;
        }
        did = (uint16_t)(((uint16_t)pdu[1] << 8) | pdu[2]);
        for (i = 0; i < AE_DID_MAX; i++) {
            if ((s_dids[i].used != 0) && (s_dids[i].did == did) && (s_dids[i].rd != NULL)) {
                if (s_dids[i].rd(did, &out[3], &dlen) == AE_OK) {
                    out[0] = UDS_POS(sid);
                    out[1] = pdu[1];
                    out[2] = pdu[2];
                    pos(out, (uint16_t)(3u + dlen));
                    return AE_OK;
                }
            }
        }
        nrc(sid, 0x31u);
        return AE_OK;
    }
    if (sid == UDS_SID_WDBI) {
        uint16_t did;
        uint8_t i;

        if ((s_sess.session == UDS_SESS_DEFAULT) || (s_sess.unlocked == 0u)) {
            nrc(sid, 0x33u);
            return AE_OK;
        }
        if (len < 4u) {
            nrc(sid, 0x13u);
            return AE_OK;
        }
        did = (uint16_t)(((uint16_t)pdu[1] << 8) | pdu[2]);
        for (i = 0; i < AE_DID_MAX; i++) {
            if ((s_dids[i].used != 0) && (s_dids[i].did == did) && (s_dids[i].wr != NULL)) {
                if (s_dids[i].wr(did, &pdu[3], (uint16_t)(len - 3u)) == AE_OK) {
                    out[0] = UDS_POS(sid);
                    out[1] = pdu[1];
                    out[2] = pdu[2];
                    pos(out, 3u);
                    return AE_OK;
                }
            }
        }
        nrc(sid, 0x31u);
        return AE_OK;
    }
    if (sid == UDS_SID_SA) {
        uint8_t level = (len > 1u) ? pdu[1] : 0x01u;
        if ((level & 0x01u) != 0u) {
            /* Request seed */
            out[0] = UDS_POS(sid);
            out[1] = level;
            out[2] = (uint8_t)(s_sess.seed >> 8);
            out[3] = (uint8_t)(s_sess.seed & 0xFFu);
            pos(out, 4u);
            return AE_OK;
        }
        /* Send key: prototype key = seed XOR 0xA5A5 */
        if (len >= 4u) {
            uint16_t key = (uint16_t)(((uint16_t)pdu[2] << 8) | pdu[3]);
            if (key == (uint16_t)(s_sess.seed ^ 0xA5A5u)) {
                s_sess.unlocked = 1u;
                out[0] = UDS_POS(sid);
                out[1] = level;
                pos(out, 2u);
                return AE_OK;
            }
        }
        s_sess.sa_fails++;
        nrc(sid, 0x35u);
        return AE_OK;
    }
    if (sid == UDS_SID_RC) {
        uint16_t rid;
        uint8_t i;
        uint16_t dlen = (uint16_t)(sizeof(out) - 4u);

        if (len < 4u) {
            nrc(sid, 0x13u);
            return AE_OK;
        }
        rid = (uint16_t)(((uint16_t)pdu[2] << 8) | pdu[3]);
        for (i = 0; i < AE_RID_MAX; i++) {
            if ((s_rids[i].used != 0) && (s_rids[i].rid == rid) && (s_rids[i].fn != NULL)) {
                if (s_rids[i].fn(rid, pdu[1], &out[4], &dlen) == AE_OK) {
                    out[0] = UDS_POS(sid);
                    out[1] = pdu[1];
                    out[2] = pdu[2];
                    out[3] = pdu[3];
                    pos(out, (uint16_t)(4u + dlen));
                    return AE_OK;
                }
            }
        }
        nrc(sid, 0x31u);
        return AE_OK;
    }
    nrc(sid, 0x11u);
    return AE_OK;
}

ae_status_t uds_client_init(uds_tx_t tx)
{
    s_cli_tx = tx;
    s_cli_len = 0u;
    return AE_OK;
}

ae_status_t uds_client_request(uint8_t sid, const uint8_t *data, uint16_t len)
{
    uint8_t p[64];

    if ((len + 1u) > sizeof(p)) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    p[0] = sid;
    if ((data != NULL) && (len > 0u)) {
        memcpy(&p[1], data, len);
    }
    if (s_cli_tx == NULL) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    return s_cli_tx(p, (uint16_t)(len + 1u));
}

ae_status_t uds_client_on_pdu(const uint8_t *pdu, uint16_t len)
{
    if ((pdu == NULL) || (len == 0u) || (len > sizeof(s_cli_last))) {
        return ae_err_make(AE_MOD_UDS, 1u);
    }
    memcpy(s_cli_last, pdu, len);
    s_cli_len = len;
    return AE_OK;
}

const uint8_t *uds_client_last(uint16_t *len)
{
    if (len != NULL) {
        *len = s_cli_len;
    }
    return s_cli_last;
}
