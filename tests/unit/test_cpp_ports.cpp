/**
 * @file test_cpp_ports.cpp
 * @brief C++ Virtual ECU, Arduino, and Raspberry Pi 5 port shims.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */

#include <cstdio>

#include "ae_error.h"
#include "arduino_port.hpp"
#include "rpi5_port.hpp"
#include "virtual_ecu.hpp"

int main(void)
{
    int fail = 0;

    if (virtual_ecu_run_demo() != AE_OK) {
        std::printf("FAIL virtual_ecu\n");
        fail = 1;
    } else {
        std::printf("PASS virtual_ecu\n");
    }
    if (arduino_run_product(0) != AE_OK) {
        std::printf("FAIL arduino_p01\n");
        fail = 1;
    } else {
        std::printf("PASS arduino_p01\n");
    }
    if (ae_err_is_ok(arduino_run_product(99))) {
        std::printf("FAIL arduino_bad_idx\n");
        fail = 1;
    } else {
        std::printf("PASS arduino_bad_idx\n");
    }
    if (rpi5_run_product(6) != AE_OK) {
        std::printf("FAIL rpi5_p07\n");
        fail = 1;
    } else {
        std::printf("PASS rpi5_p07\n");
    }
    std::printf(fail ? "RESULT FAIL\n" : "RESULT PASS\n");
    return fail;
}
