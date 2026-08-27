# Architecture package — POSIX Virtual ECU · ESP32-C3 · Cloud

This folder is the **STEP 0 / STEP 1** baseline for the three-target platform. It describes what exists in this repository today, what is missing, and the order of work. It does not replace the HTML portal; it extends it.

| Document | Purpose |
|---|---|
| [gap_analysis.md](gap_analysis.md) | Honest inventory vs the master prompt |
| [system_architecture.md](system_architecture.md) | Nodes, buses, cloud, dashboards |
| [software_architecture.md](software_architecture.md) | Layers, portability, C vs C++ |
| [runtime_architecture.md](runtime_architecture.md) | OSAL, tasks, POSIX vs FreeRTOS |
| [data_flow.md](data_flow.md) | CAN, UDS, telemetry paths |
| [control_flow.md](control_flow.md) | Boot, ECU, fault, OTA, session |
| [module_dependency.md](module_dependency.md) | Allowed includes and graph |
| [deployment_architecture.md](deployment_architecture.md) | How each target is built and run |
| [directory_structure.md](directory_structure.md) | Live tree vs target tree (no big-bang rename) |
| [development_phases.md](development_phases.md) | Ordered phases; POSIX first |
| [test_strategy.md](test_strategy.md) | Unit through HIL and fault injection |
| [agent_breakdown.md](agent_breakdown.md) | Cursor agents and rules |

**Governing rules**

1. One core platform. Products are thin compositions.
2. Application and services must not call `pthread`, FreeRTOS, or ESP-IDF. Those live in OSAL / HAL / drivers / platform ports.
3. CAN identifiers in `platform/common/ae_can_ids.h` are **prototype / simulated**. They are not OEM IDs.
4. This is **not** ISO 26262 certified and **not** UNECE WP.29 / R155 certified.
5. Do not download random third-party source. External dependencies must be documented (license, version, purpose, URL, build).

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.
