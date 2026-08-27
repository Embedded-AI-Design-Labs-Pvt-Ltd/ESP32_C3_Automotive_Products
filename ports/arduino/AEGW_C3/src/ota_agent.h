/**
 * @file ota_agent.h
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#ifndef OTA_AGENT_H
#define OTA_AGENT_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OTA_BOOT_SELECT = 0,
    OTA_DOWNLOAD,
    OTA_TRANSFER,
    OTA_VERIFY,
    OTA_COMMIT,
    OTA_DISCARD
} ota_state_t;

ae_status_t ota_init(void);
ae_status_t ota_begin(uint32_t image_len, uint32_t crc32);
ae_status_t ota_write(const uint8_t *chunk, uint16_t len);
ae_status_t ota_finish(void);
ota_state_t ota_state(void);
int ota_running_slot_valid(void);

#ifdef __cplusplus
}
#endif

#endif /* OTA_AGENT_H */
