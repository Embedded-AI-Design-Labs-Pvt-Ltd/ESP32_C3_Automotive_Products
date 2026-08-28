# linux/

Host-side Classical CAN lab tooling for SocketCAN, python-can, DBC, ISO-TP/UDS clients.

| Path | Role |
|---|---|
| `socketcan/` | setup/teardown scripts, CANable/PCAN notes |
| `python/` | `aegw_can` CLI + signal helpers |
| `dbc/` | Points to repo-root `dbc/` |
| `isotp/` / `uds/` | Future Python clients (C stacks stay in `platform/`) |

```bash
sudo linux/socketcan/setup_can0.sh can0 500000
PYTHONPATH=linux/python python linux/python/tests/test_dbc_signals.py
```

Bench / Virtual ECU only. No CAN-FD on the ESP32-C3 path.

---

**Embedded AI Design Labs Pvt Ltd · Muhammad Samiullah · © 2026**
