# AGENT: BLE ENGINEER

**Do not start until CAN + UDS + OSAL are reviewed.**

GATT automotive characteristics already IDed in `ble_auto.h`. Host shim exists. NimBLE belongs in `drivers/esp32` behind `hal_ble`.

Reconnect state machine, pairing prototype with documented limits. BLE is a transport into the same UDS/DTC tables.
