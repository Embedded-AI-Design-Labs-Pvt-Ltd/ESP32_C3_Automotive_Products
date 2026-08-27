/**
 * @file ae_ring.c
 * @brief Byte ring: ISR may put, task may get. No heap.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include "ae_ring.h"

#include "ae_error.h"

/**
 * @brief Bind a ring to caller storage.
 * @param[in,out] ring Control block. Must not be NULL.
 * @param[in] storage Buffer owned by the caller. Must not be NULL.
 * @param[in] cap Byte capacity. Must be >= 2.
 * @return AE_OK or AE_ERR(AE_MOD_OS, 1) on bad args.
 */
ae_status_t ae_ring_init(ae_ring_t *ring, uint8_t *storage, size_t cap)
{
    if ((ring == NULL) || (storage == NULL) || (cap < 2u)) {
        return ae_err_make(AE_MOD_OS, 1u);
    }

    ring->buf = storage;
    ring->cap = cap;
    ring->head = 0u;
    ring->tail = 0u;
    ring->drops = 0u;
    return AE_OK;
}

/**
 * @brief Push one byte. Drops and counts if full.
 */
ae_status_t ae_ring_put(ae_ring_t *ring, uint8_t byte)
{
    size_t next;

    if (ring == NULL) {
        return ae_err_make(AE_MOD_OS, 1u);
    }

    next = ring->head + 1u;
    if (next >= ring->cap) {
        next = 0u;
    }
    if (next == ring->tail) {
        ring->drops++;
        return ae_err_make(AE_MOD_OS, 2u);
    }

    ring->buf[ring->head] = byte;
    ring->head = next;
    return AE_OK;
}

/**
 * @brief Pop one byte.
 */
ae_status_t ae_ring_get(ae_ring_t *ring, uint8_t *byte)
{
    size_t next;

    if ((ring == NULL) || (byte == NULL)) {
        return ae_err_make(AE_MOD_OS, 1u);
    }
    if (ring->head == ring->tail) {
        return ae_err_make(AE_MOD_OS, 3u);
    }

    *byte = ring->buf[ring->tail];
    next = ring->tail + 1u;
    if (next >= ring->cap) {
        next = 0u;
    }
    ring->tail = next;
    return AE_OK;
}

size_t ae_ring_count(const ae_ring_t *ring)
{
    size_t h;
    size_t t;

    if (ring == NULL) {
        return 0u;
    }
    h = ring->head;
    t = ring->tail;
    if (h >= t) {
        return h - t;
    }
    return (ring->cap - t) + h;
}

uint32_t ae_ring_drops(const ae_ring_t *ring)
{
    if (ring == NULL) {
        return 0u;
    }
    return ring->drops;
}
