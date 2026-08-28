# linux/python — AEGW-C3 CAN automation

```bash
# Signal unit tests (no adapter):
python linux/python/tests/test_dbc_signals.py

# Optional on Linux with adapter:
pip install python-can
python -m aegw_can.cli --iface can0 dump
```

Run from `linux/python` or set `PYTHONPATH=linux/python`.

Classical CAN only. Lab/bench use only.
