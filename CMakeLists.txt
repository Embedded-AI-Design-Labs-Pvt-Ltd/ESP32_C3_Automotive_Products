# Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
# Muhammad Samiullah — CTO & Founder. All rights reserved.
#
# POSIX / host tests. Does not require ESP32 hardware.
#   cmake -S . -B build/cmake
#   cmake --build build/cmake --config Debug
#   ctest --test-dir build/cmake -C Debug --output-on-failure
#
# Visual Studio is a multi-config generator; always pass -C Debug (or Release).

cmake_minimum_required(VERSION 3.16)
project(aegw_c3_host C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(AE_HOST=1)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Werror)
endif()

set(AE_INC
  ${CMAKE_SOURCE_DIR}/platform/common
  ${CMAKE_SOURCE_DIR}/platform/hal
  ${CMAKE_SOURCE_DIR}/platform/framework
  ${CMAKE_SOURCE_DIR}/platform/protocols
  ${CMAKE_SOURCE_DIR}/platform/services
  ${CMAKE_SOURCE_DIR}/products
  ${CMAKE_SOURCE_DIR}/ports/virtual_ecu
  ${CMAKE_SOURCE_DIR}/ports/arduino
  ${CMAKE_SOURCE_DIR}/ports/raspberry_pi_5
)

set(AE_PLATFORM_SRC
  platform/common/ae_error.c
  platform/framework/ae_ring.c
  platform/hal/hal_host.c
  platform/protocols/can_service.c
  platform/protocols/isotp.c
  platform/protocols/uds.c
  platform/services/dtc.c
  platform/services/ble_auto.c
  platform/services/fault_mgr.c
  platform/services/ota_agent.c
  platform/services/ecu_models.c
  products/product_dids.c
  products/products_connectivity.c
  products/products_diagnostics.c
  products/products_validation.c
  products/products_security_ota.c
  products/products_registry.c
  ports/arduino/aegw_runtime.c
)

add_library(ae_platform STATIC ${AE_PLATFORM_SRC})
target_include_directories(ae_platform PUBLIC ${AE_INC})

add_executable(test_host
  tests/unit/test_host.c
  platform/common/ae_error.c
  platform/framework/ae_ring.c
)
target_include_directories(test_host PRIVATE ${AE_INC})

add_executable(test_all_products tests/unit/test_all_products.c)
target_link_libraries(test_all_products PRIVATE ae_platform)

add_executable(test_coverage tests/unit/test_coverage.c)
target_link_libraries(test_coverage PRIVATE ae_platform)

add_executable(test_cpp_ports
  tests/unit/test_cpp_ports.cpp
  ports/virtual_ecu/virtual_ecu.cpp
  ports/arduino/arduino_port.cpp
  ports/raspberry_pi_5/rpi5_port.cpp
)
target_link_libraries(test_cpp_ports PRIVATE ae_platform)
target_include_directories(test_cpp_ports PRIVATE ${AE_INC})

enable_testing()
add_test(NAME test_host COMMAND test_host)
add_test(NAME test_all_products COMMAND test_all_products)
add_test(NAME test_coverage COMMAND test_coverage)
add_test(NAME test_cpp_ports COMMAND test_cpp_ports)
