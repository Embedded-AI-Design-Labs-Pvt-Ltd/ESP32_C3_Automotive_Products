# OSAL — not implemented yet

Portable `os_*` APIs will live here. See `docs/architecture/runtime_architecture.md` and `.cursor/agents/osal_engineer.md`.

| Path | Role |
|---|---|
| `include/` (later) | Public headers used by services |
| `posix/` | pthread backend |
| `freertos/` | FreeRTOS backend |

Do not put pthread or FreeRTOS includes in `products/` or `platform/protocols/`.

No dummy `.c` files until phase A2.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
