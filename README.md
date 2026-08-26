# Automotive ESP32-C3 Edge Gateway & ECU Engineering Platform

One reusable embedded platform. Seventeen product applications assembled from the same HAL, drivers, protocols, and services.

**Embedded AI Design Labs Pvt Ltd**  
Author: Muhammad Samiullah (CTO) — muhammadsami@embedailabs.com  
GitHub: [Embedded-AI-Design-Labs-Pvt-Ltd](https://github.com/Embedded-AI-Design-Labs-Pvt-Ltd/)

**Primary target:** ESP32-C3 · ESP-IDF · FreeRTOS · C (firmware) · C++ (host tools)

This repository is **not** seventeen separate products. It is one core platform plus thin product compositions.

## Run locally

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\scripts\run_all.ps1
python tools\gui\server.py
```

GUI (Virtual ECU, no hardware): http://127.0.0.1:8765/

Administrator / toolchain helper (UAC prompt):

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\scripts\setup_admin.ps1
```

## Read this first

- [docs/index.html](docs/index.html) — architecture portal
- [docs/presentation.html](docs/presentation.html) — slide deck
- [docs/pages/01-architecture.html](docs/pages/01-architecture.html) — system architecture

## Layered model

```
Application (product composition)
        ↓
Automotive Services
        ↓
Protocols (CAN · ISO-TP · UDS · OBD-II)
        ↓
Middleware (IPC · logger · config · health)
        ↓
HAL
        ↓
Drivers
        ↓
ESP32-C3 hardware
```

## Repository map

| Path | Role |
|---|---|
| `platform/` | Reusable modules only |
| `products/` | Thin product applications (P01–P17) |
| `ports/` | ESP32-C3, Arduino, Raspberry Pi 5, Virtual ECU |
| `tools/` | PC GUI, Virtual ECU, scripts |
| `tests/` | Unit, integration, HIL, regression |
| `infra/` | Docker, Jenkins, Terraform (AWS/GCP/Azure) |
| `docs/` | Architecture HTML portal and presentation |
