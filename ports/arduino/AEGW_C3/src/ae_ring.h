/**
 * @file ae_ring.h
 * @brief Static byte ring buffer. Single producer / single consumer.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 * @ownership Caller owns ae_ring_t and the storage buffer.
 * @isr_safe Put from ISR if only one producer; get from one task.
 */
#ifndef AE_RING_H
#define AE_RING_H

#include <stddef.h>
#include <stdint.h>

#include "ae_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *buf;     /**< Caller-provided storage. */
    size_t cap;       /**< Capacity in bytes. */
    volatile size_t head;
    volatile size_t tail;
    volatile uint32_t drops;
} ae_ring_t;

ae_status_t ae_ring_init(ae_ring_t *ring, uint8_t *storage, size_t cap);
ae_status_t ae_ring_put(ae_ring_t *ring, uint8_t byte);
ae_status_t ae_ring_get(ae_ring_t *ring, uint8_t *byte);
size_t ae_ring_count(const ae_ring_t *ring);
uint32_t ae_ring_drops(const ae_ring_t *ring);

#ifdef __cplusplus
}
#endif

#endif /* AE_RING_H */
