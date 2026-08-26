CC = gcc
CXX = g++
CFLAGS = -std=c11 -Wall -Wextra -Werror -DAE_HOST=1
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -DAE_HOST=1
INCLUDES = -Iplatform/common -Iplatform/hal -Iplatform/framework \
	-Iplatform/middleware -Iplatform/protocols -Iplatform/services \
	-Iproducts -Iports/virtual_ecu -Iports/esp32_c3 -Iports/arduino \
	-Iports/raspberry_pi_5

SRC_C = \
	platform/common/ae_error.c \
	platform/framework/ae_ring.c \
	platform/hal/hal_host.c \
	platform/protocols/can_service.c \
	platform/protocols/isotp.c \
	platform/protocols/uds.c \
	platform/services/dtc.c \
	platform/services/ble_auto.c \
	platform/services/fault_mgr.c \
	platform/services/ota_agent.c \
	platform/services/ecu_models.c \
	products/product_dids.c \
	products/products_connectivity.c \
	products/products_diagnostics.c \
	products/products_validation.c \
	products/products_security_ota.c \
	products/products_registry.c

SRC_CXX = \
	ports/virtual_ecu/virtual_ecu.cpp \
	ports/arduino/arduino_port.cpp \
	ports/raspberry_pi_5/rpi5_port.cpp

OUT = build/host

.PHONY: all test clean

all: test

$(OUT):
	mkdir -p $(OUT)

test: $(OUT)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC_C) tests/unit/test_host.c -o $(OUT)/test_host.exe
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC_C) tests/unit/test_all_products.c -o $(OUT)/test_all_products.exe
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC_C) $(SRC_CXX) tests/unit/test_cpp_ports.cpp -o $(OUT)/test_cpp_ports.exe
	$(OUT)/test_host.exe
	$(OUT)/test_all_products.exe
	$(OUT)/test_cpp_ports.exe

clean:
	rm -rf $(OUT)
