/**
 * @file ota_agent.c
 * @brief Two-slot conceptual update. Failed verify never marks running slot bad.
 */

#include "ota_agent.h"

#include "ae_error.h"

#include <string.h>

#define AE_OTA_MAX 512u

static uint8_t s_slot[AE_OTA_MAX];
static uint32_t s_need;
static uint32_t s_got;
static uint32_t s_crc_exp;
static ota_state_t s_st;
static int s_run_ok = 1;

static uint32_t crc32_calc(const uint8_t *p, uint32_t n)
{
    uint32_t c = 0xFFFFFFFFu;
    uint32_t i;
    uint8_t b;

    for (i = 0; i < n; i++) {
        c ^= p[i];
        for (b = 0; b < 8u; b++) {
            c = (c >> 1) ^ (0xEDB88320u & (uint32_t)(-(int32_t)(c & 1u)));
        }
    }
    return ~c;
}

ae_status_t ota_init(void)
{
    s_st = OTA_BOOT_SELECT;
    s_got = 0u;
    s_run_ok = 1;
    return AE_OK;
}

ae_status_t ota_begin(uint32_t image_len, uint32_t crc32)
{
    if ((image_len == 0u) || (image_len > AE_OTA_MAX)) {
        return ae_err_make(AE_MOD_OTA, 1u);
    }
    s_need = image_len;
    s_crc_exp = crc32;
    s_got = 0u;
    s_st = OTA_DOWNLOAD;
    return AE_OK;
}

ae_status_t ota_write(const uint8_t *chunk, uint16_t len)
{
    if ((chunk == NULL) || ((s_got + len) > s_need)) {
        s_st = OTA_DISCARD;
        return ae_err_make(AE_MOD_OTA, 2u);
    }
    memcpy(&s_slot[s_got], chunk, len);
    s_got += len;
    s_st = OTA_TRANSFER;
    return AE_OK;
}

ae_status_t ota_finish(void)
{
    uint32_t c;

    s_st = OTA_VERIFY;
    if (s_got != s_need) {
        s_st = OTA_DISCARD;
        return ae_err_make(AE_MOD_OTA, 3u);
    }
    c = crc32_calc(s_slot, s_got);
    if (c != s_crc_exp) {
        s_st = OTA_DISCARD;
        return ae_err_make(AE_MOD_OTA, 4u);
    }
    s_st = OTA_COMMIT;
    return AE_OK;
}

ota_state_t ota_state(void)
{
    return s_st;
}

int ota_running_slot_valid(void)
{
    return s_run_ok;
}
