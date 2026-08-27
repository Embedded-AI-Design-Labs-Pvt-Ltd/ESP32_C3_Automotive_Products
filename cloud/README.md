# Cloud — lab prototype (not implemented)

Intended path: ECU or Virtual ECU → MQTT/TLS → gateway → telemetry → database → dashboard.

- Device registration, telemetry, commands, DTC upload, OTA metadata.
- **Never commit broker passwords, client keys, or `.pem` files.** Use environment variables or gitignored files (`.env` is ignored).
- Do not block the CAN RX path on network I/O.
- Not a production fleet backend. Not UNECE R155 certified.

See `.cursor/agents/cloud_engineer.md` (phase A13).

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
