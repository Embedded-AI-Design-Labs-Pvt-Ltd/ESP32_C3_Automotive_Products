/**
 * @file can_analyzer.hpp
 * @brief Host-side Classical CAN analyzer (simulation / HIL feed).
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 */
#pragma once

#include "ae_types.h"

#include <cstdint>
#include <cstring>

namespace aegw {

class CanAnalyzer {
public:
    static constexpr unsigned kBins = 64;

    void reset()
    {
        std::memset(counts_, 0, sizeof(counts_));
        std::memset(last_ms_, 0, sizeof(last_ms_));
        total_ = 0;
        unknown_ = 0;
        filter_id_ = 0xFFFFFFFFu;
        paused_ = false;
    }

    void set_filter(uint32_t id_or_all)
    {
        filter_id_ = id_or_all;
    }

    void set_paused(bool p) { paused_ = p; }
    bool paused() const { return paused_; }

    void on_frame(const ae_can_frame_t &f, uint32_t now_ms)
    {
        if (paused_) {
            return;
        }
        if (filter_id_ != 0xFFFFFFFFu && f.id != filter_id_) {
            return;
        }
        unsigned b = static_cast<unsigned>(f.id % kBins);
        counts_[b]++;
        last_ms_[b] = now_ms;
        total_++;
        if (f.id > 0x7FFu && f.ide == 0) {
            unknown_++;
        }
    }

    uint32_t total() const { return total_; }
    uint32_t unknown() const { return unknown_; }
    uint32_t count_bin(unsigned b) const
    {
        return (b < kBins) ? counts_[b] : 0u;
    }

private:
    uint32_t counts_[kBins]{};
    uint32_t last_ms_[kBins]{};
    uint32_t total_{0};
    uint32_t unknown_{0};
    uint32_t filter_id_{0xFFFFFFFFu};
    bool paused_{false};
};

} /* namespace aegw */
