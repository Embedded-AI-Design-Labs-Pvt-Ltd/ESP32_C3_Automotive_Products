#ifndef FAULT_MGR_H
#define FAULT_MGR_H

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FAULT_NORMAL = 0,
    FAULT_DETECTED,
    FAULT_DEGRADED,
    FAULT_RECOVERY,
    FAULT_SAFE
} fault_state_t;

ae_status_t fault_init(void);
ae_status_t fault_report(uint32_t id, int critical);
ae_status_t fault_recover(void);
fault_state_t fault_get_state(void);

#ifdef __cplusplus
}
#endif

#endif /* FAULT_MGR_H */
