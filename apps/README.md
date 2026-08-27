# Apps — thin POSIX executables

Product logic stays in `products/` and `platform/`. These folders are process entrypoints once OSAL exists.

| App | Status | Maps to |
|---|---|---|
| `virtual_ecu/` | Planned | `ports/virtual_ecu` + GUI |
| `vehicle_simulator/` | Planned | New physics model |
| `diagnostic_tester/` | Planned | P03 |
| `can_gateway/` | Planned | P01 |
| `can_logger/` | Planned | P04 |
| `ecu_test_tool/` | Planned | P06 |
| `cloud_gateway/` | Planned | `cloud/` |

Do not duplicate protocol stacks here.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
