# AGENT: POSIX BUILD ENGINEER

CMake + GCC/Clang + CTest. Host tests must run without ESP32.

Commands: `cmake -S . -B build/cmake`, `cmake --build build/cmake --config Debug`, `ctest --test-dir build/cmake -C Debug`. Later binaries: virtual_ecu, vehicle_simulator, diagnostic_tester, can_logger.

Windows (this repo) and Linux CI should share the same C sources.
