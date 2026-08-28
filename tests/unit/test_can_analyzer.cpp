/**
 * @file test_can_analyzer.cpp
 * @brief C++ CAN analyzer unit smoke test.
 */
#include "can_analyzer.hpp"

#include <cstdio>

int main()
{
    aegw::CanAnalyzer a;
    ae_can_frame_t f{};
    a.reset();
    f.id = 0x1A0;
    f.dlc = 8;
    a.on_frame(f, 10);
    a.on_frame(f, 20);
    if (a.total() != 2u) {
        std::printf("FAIL total\n");
        return 1;
    }
    a.set_filter(0x200);
    f.id = 0x1A0;
    a.on_frame(f, 30);
    if (a.total() != 2u) {
        std::printf("FAIL filter\n");
        return 1;
    }
    a.set_paused(true);
    f.id = 0x200;
    a.on_frame(f, 40);
    if (a.total() != 2u) {
        std::printf("FAIL pause\n");
        return 1;
    }
    std::printf("PASS can_analyzer\nRESULT PASS\n");
    return 0;
}
