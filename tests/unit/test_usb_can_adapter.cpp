/**
 * @file test_usb_can_adapter.cpp
 * @brief C++ UsbCanAdapter wrapper smoke test (sim).
 */
#include "usb_can_adapter.hpp"

#include <cstdio>
#include <cstring>

int main()
{
    aegw::UsbCanAdapter a;
#ifdef _WIN32
    _putenv("AE_CAN_BACKEND=");
#else
    unsetenv("AE_CAN_BACKEND");
#endif
    if (a.open_from_env() != AE_OK) {
        std::printf("FAIL open\n");
        return 1;
    }
    if (a.live()) {
        std::printf("FAIL unexpected live\n");
        return 1;
    }
    if (std::strcmp(a.backend_name(), "sim") != 0) {
        std::printf("FAIL backend\n");
        return 1;
    }
    ae_can_frame_t f{};
    f.id = 0x200;
    f.dlc = 2;
    f.data[0] = 1;
    if (a.send(f) != AE_OK) {
        std::printf("FAIL send\n");
        return 1;
    }
    ae_can_frame_t r{};
    if (a.recv(r) != AE_OK || r.id != 0x200u) {
        std::printf("FAIL recv\n");
        return 1;
    }
    std::printf("PASS usb_can_adapter\nRESULT PASS\n");
    return 0;
}
