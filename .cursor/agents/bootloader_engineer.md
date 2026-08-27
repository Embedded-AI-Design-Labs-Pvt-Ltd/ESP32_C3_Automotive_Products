# AGENT: BOOTLOADER AND OTA ENGINEER

Harden `ota_agent` POSIX flow: metadata, version, CRC/hash, download, verify, rollback. Do not overwrite the running image until verify passes.

Conceptual transports: UART, CAN, BLE, cloud-triggered. Implement simulated POSIX first.

Arduino/IDF flash layout comes after POSIX tests exist.
