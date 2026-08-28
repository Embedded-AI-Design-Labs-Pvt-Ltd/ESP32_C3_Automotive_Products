/**
 * @file usb_can_adapter.hpp
 * @brief C++ helper for CANable / PCAN-USB / SLCAN / SocketCAN on host.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 */
#pragma once

#include "hal_can.h"
#include "hal_can_adapter.h"

#include <cstring>
#include <string>

namespace aegw {

class UsbCanAdapter {
public:
    enum class Backend { Sim, SocketCan, Pcan, Slcan };

    static Backend parse(const std::string &name)
    {
        if (name == "socketcan" || name == "canable") {
            return Backend::SocketCan;
        }
        if (name == "pcan" || name == "pcan-usb") {
            return Backend::Pcan;
        }
        if (name == "slcan") {
            return Backend::Slcan;
        }
        return Backend::Sim;
    }

    ae_status_t open(Backend be, const char *iface, uint32_t bitrate = 500000u)
    {
        uint8_t b = AE_CAN_BE_SIM;
        if (be == Backend::SocketCan) {
            b = AE_CAN_BE_SOCKETCAN;
        } else if (be == Backend::Pcan) {
            b = AE_CAN_BE_PCAN;
        } else if (be == Backend::Slcan) {
            b = AE_CAN_BE_SLCAN;
        }
        ae_can_cfg_t cfg{};
        cfg.bitrate = bitrate;
        cfg.backend = b;
        if (iface != nullptr) {
            std::strncpy(cfg.iface, iface, sizeof(cfg.iface) - 1u);
        }
        return hal_can_init(&cfg);
    }

    ae_status_t open_from_env() { return hal_can_init_from_env(); }

    ae_status_t send(const ae_can_frame_t &f, uint32_t to_ms = 10u)
    {
        return hal_can_send(&f, to_ms);
    }

    ae_status_t recv(ae_can_frame_t &f) { return hal_can_recv(&f); }

    bool live() const { return hal_can_adapter_is_live() != 0; }

    const char *backend_name() const
    {
        return hal_can_backend_name(hal_can_backend());
    }
};

} /* namespace aegw */
