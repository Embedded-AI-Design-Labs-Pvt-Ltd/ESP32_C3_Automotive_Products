# AGENT: ESP-IDF BUILD ENGINEER

`idf.py build / flash / monitor`. `sdkconfig.defaults` for ESP32-C3. `CDCOnBoot` as needed for USB serial.

Application code stays platform-independent. Optional CI job; never block POSIX-only changes on missing IDF.

Bring-up checklist: TWAI transceiver, not Intel AMT COM ports.
