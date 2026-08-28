/**
 * @file usecases.h
 * @brief 120 real-time automotive + cybersecurity lab use cases (Classical CAN).
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 * @note Lab/Virtual ECU only. Not for road vehicles. Not ISO 26262 / R155 certified.
 */
#ifndef AE_USECASES_H
#define AE_USECASES_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AE_UC_COUNT 120u

typedef enum {
    AE_UC_CAT_CAN = 0,
    AE_UC_CAT_UDS,
    AE_UC_CAT_DIAG,
    AE_UC_CAT_FAULT,
    AE_UC_CAT_CYBER,
    AE_UC_CAT_VEHICLE,
    AE_UC_CAT_E2E,
    AE_UC_CAT_HIL,
    AE_UC_CAT_OTA,
    AE_UC_CAT_GATEWAY
} ae_uc_category_t;

typedef struct {
    uint16_t id; /* 1..AE_UC_COUNT */
    ae_uc_category_t category;
    const char *name;
    const char *requirement; /* REQ-LAB / REQ-P style tag */
} ae_usecase_desc_t;

const ae_usecase_desc_t *ae_usecases_table(uint16_t *count);

/** Run one use case by ID (1..AE_UC_COUNT). Initializes host HAL as needed. */
ae_status_t ae_usecase_run(uint16_t id);

/** Run all use cases; *failed receives first failing ID (0 if none). */
ae_status_t ae_usecases_run_all(uint16_t *failed, uint16_t *passed);

const char *ae_uc_category_name(ae_uc_category_t cat);

#ifdef __cplusplus
}
#endif

#endif /* AE_USECASES_H */
