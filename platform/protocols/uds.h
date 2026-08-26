/**
 * @file uds.h
 * @brief UDS client and server (ISO 14229 prototype SIDs).
 */
#ifndef UDS_H
#define UDS_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UDS_SID_DSC        0x10u
#define UDS_SID_ER         0x11u
#define UDS_SID_CDTCI      0x14u
#define UDS_SID_RDTCI      0x19u
#define UDS_SID_RDBI       0x22u
#define UDS_SID_SA         0x27u
#define UDS_SID_WDBI       0x2Eu
#define UDS_SID_RC         0x31u
#define UDS_SID_TP         0x3Eu
#define UDS_POS(sid)       ((uint8_t)((sid) + 0x40u))
#define UDS_NRC            0x7Fu

#define UDS_SESS_DEFAULT   0x01u
#define UDS_SESS_PROG      0x02u
#define UDS_SESS_EXT       0x03u

typedef ae_status_t (*uds_did_read_t)(uint16_t did, uint8_t *out, uint16_t *len);
typedef ae_status_t (*uds_did_write_t)(uint16_t did, const uint8_t *in, uint16_t len);
typedef ae_status_t (*uds_routine_t)(uint16_t rid, uint8_t op, uint8_t *out, uint16_t *len);
typedef ae_status_t (*uds_tx_t)(const uint8_t *pdu, uint16_t len);

typedef struct {
    uint8_t session;
    uint8_t unlocked;
    uint16_t seed;
    uint8_t sa_fails;
    uint32_t s3_ms;
} uds_session_t;

ae_status_t uds_server_init(uds_tx_t tx);
ae_status_t uds_register_did(uint16_t did, uds_did_read_t rd, uds_did_write_t wr);
ae_status_t uds_register_routine(uint16_t rid, uds_routine_t fn);
ae_status_t uds_server_on_pdu(const uint8_t *pdu, uint16_t len);
ae_status_t uds_server_tick(uint32_t now_ms);
const uds_session_t *uds_server_session(void);

ae_status_t uds_client_init(uds_tx_t tx);
ae_status_t uds_client_request(uint8_t sid, const uint8_t *data, uint16_t len);
ae_status_t uds_client_on_pdu(const uint8_t *pdu, uint16_t len);
const uint8_t *uds_client_last(uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif /* UDS_H */
