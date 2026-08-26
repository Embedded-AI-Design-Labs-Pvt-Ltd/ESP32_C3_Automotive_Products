/**
 * @file dtc.h
 * @brief Diagnostic trouble code manager (detect / confirm / clear).
 */
#ifndef DTC_H
#define DTC_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DTC_SEV_INFO = 0,
    DTC_SEV_WARN,
    DTC_SEV_CRIT
} dtc_severity_t;

typedef struct {
    uint32_t code;
    uint8_t status; /* bit0 pending, bit1 confirmed */
    uint8_t confirm_cnt;
    dtc_severity_t sev;
} dtc_record_t;

ae_status_t dtc_init(void);
ae_status_t dtc_raise(uint32_t code, dtc_severity_t sev);
ae_status_t dtc_clear(uint32_t mask);
ae_status_t dtc_read(dtc_record_t *out, uint8_t *count);
int dtc_confirmed(uint32_t code);

#ifdef __cplusplus
}
#endif

#endif /* DTC_H */
