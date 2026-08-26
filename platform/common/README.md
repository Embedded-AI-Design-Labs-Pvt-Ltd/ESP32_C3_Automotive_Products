# platform/common

Static types shared by every layer. No ESP-IDF includes.

Planned files (implementation after approval):

- `ae_types.h` — `ae_status_t`, IDs, packed CAN frame, timestamps
- `ae_error.h` / `ae_error.c` — 16-bit module|reason codes
- `ae_config.h` — compile-time product and feature flags
- `ae_limits.h` — queue depths, ISO-TP MTU, log line size
