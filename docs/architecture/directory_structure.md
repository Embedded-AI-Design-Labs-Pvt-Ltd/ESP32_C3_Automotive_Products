# Directory structure

The playbook’s `automotive_virtual_ecu/` tree is the **logical** layout. This repository **keeps** `platform/`, `products/`, and `ports/` as the live code so Arduino generation and host tests do not break. New top-level folders are added beside them.

## 1. Live tree (do not big-bang rename)

```text
ESP32_C3_Automotive_Products/
├── apps/                      CLI entrypoints (thin; link platform)
├── osal/                      OSAL API + posix/ + freertos/   (new)
├── cloud/                     gateway / telemetry (new, no secrets)
├── dashboard/                 fleet UI later; lab GUI stays in tools/gui
├── configs/                   board and sim JSON/Kconfig snippets
├── third_party/               documented dependencies only
├── platform/                  reusable C (existing)
│   ├── common/
│   ├── framework/
│   ├── hal/                   public HAL headers + current host impl
│   ├── bsp/                   empty → ESP32 pin map
│   ├── drivers/               empty → posix/ and esp32/
│   ├── middleware/            empty → logger, bus (OSAL-based)
│   ├── protocols/
│   └── services/
├── products/                  P01–P17 compositions
├── ports/
│   ├── esp32_c3/              IDF project (to complete)
│   ├── virtual_ecu/
│   ├── arduino/
│   └── raspberry_pi_5/
├── tests/
├── tools/                     GUI, scripts
├── docs/                      HTML portal + docs/architecture
├── infra/                     docker / jenkins / terraform (planned)
└── .cursor/                   agents + rules
```

## 2. Mapping from playbook names

| Playbook | This repo |
|---|---|
| `apps/virtual_ecu` | `apps/virtual_ecu` + `ports/virtual_ecu` + `tools/gui` |
| `apps/vehicle_simulator` | `apps/vehicle_simulator` (new) |
| `apps/diagnostic_tester` | product P03 + `apps/diagnostic_tester` |
| `core/*` | `platform/services` + `platform/framework` |
| `protocols/*` | `platform/protocols` |
| `middleware/*` | `platform/middleware` + `osal` |
| `hal/*` | `platform/hal` |
| `drivers/posix` | `platform/drivers/posix` (move `hal_host.c` here when OSAL lands) |
| `drivers/esp32` | `platform/drivers/esp32` |
| `platforms/posix` | CMake host + `osal/posix` |
| `platforms/esp32c3` | `ports/esp32_c3` |
| `cloud/`, `dashboard/` | top-level folders |

## 3. CMake (POSIX)

Root `CMakeLists.txt` builds **existing** host tests. It does not add dummy libraries.

ESP-IDF uses its own CMake under `ports/esp32_c3/` when that project is created.

## 4. Include rules (enforced in review)

See [module_dependency.md](module_dependency.md). CI later: a script that greps IDF/pthread includes under `products/` and `platform/protocols`.

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
