# Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
# Muhammad Samiullah — CTO & Founder. All rights reserved.
"""AEGW-C3 Virtual ECU GUI server. No ESP32 hardware required."""

from __future__ import annotations

import json
import threading
import time
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any
from urllib.parse import urlparse

ROOT = Path(__file__).resolve().parent
REPO = ROOT.parent.parent
GUI_DIR = ROOT
DOCS_DIR = REPO / "docs"
PORT = 8765

PRODUCTS = [
    ("P01", "BLE-CAN Gateway"),
    ("P02", "BLE OBD-II Adapter"),
    ("P03", "UDS Diagnostic Tester"),
    ("P04", "CAN Data Logger"),
    ("P05", "ECU Communication Analyzer"),
    ("P06", "ECU Test Box"),
    ("P07", "ECU Simulator"),
    ("P08", "ECU Fault Injection Box"),
    ("P09", "ECU End-of-Line Tester"),
    ("P10", "ECU Config/Calibration"),
    ("P11", "Automotive Sensor Gateway"),
    ("P12", "TPMS BLE Gateway"),
    ("P13", "EV Battery Diagnostic Gateway"),
    ("P14", "CAN Intrusion/Anomaly Monitor"),
    ("P15", "HIL/RCP Interface"),
    ("P16", "Automotive Training Kit"),
    ("P17", "ECU Bootloader/OTA Prototype"),
]


class VirtualEcu:
    def __init__(self) -> None:
        self.lock = threading.Lock()
        self.mode = "virtual"
        self.hw_ok = False
        self.running = True
        self.can: list[dict[str, Any]] = []
        self.dtc: list[dict[str, Any]] = []
        self.log: list[str] = []
        self.vin = "AEGWC3PROTO00001"
        self.session = "DEFAULT"
        self.fault = "NORMAL"
        self.bcm = {"ign": 1, "doors": 0, "lights": 0, "speed": 42}
        self.vcu = {"ready": 1, "mode": 1, "torque": 80}
        self.bms = {"pack_v": 380.0, "soc": 80, "soh": 95, "tmin": 25, "tmax": 28, "lim_dch": 200}
        self.tpms = [220, 218, 221, 219]
        self.sensor_mv = 3300
        self.ble_link = True
        self.last_product = None
        self.counter = 0
        self.drop_id = None

    def _frame(self, can_id: int, data: list[int], name: str) -> None:
        if self.drop_id is not None and can_id == self.drop_id:
            self._note(f"DROP {can_id:#05x}")
            return
        rec = {
            "ts": int(time.time() * 1000),
            "id": f"{can_id:#05x}",
            "dlc": len(data),
            "data": " ".join(f"{b:02X}" for b in data),
            "name": name,
        }
        self.can.append(rec)
        self.can = self.can[-80:]

    def _note(self, msg: str) -> None:
        self.log.append(f"{time.strftime('%H:%M:%S')}  {msg}")
        self.log = self.log[-40:]

    def _raise_dtc(self, code: str, text: str) -> None:
        for d in self.dtc:
            if d["code"] == code:
                d["count"] = min(3, d["count"] + 1)
                d["status"] = "CONFIRMED" if d["count"] >= 3 else "PENDING"
                return
        self.dtc.append({"code": code, "text": text, "count": 1, "status": "PENDING"})

    def tick(self) -> None:
        with self.lock:
            self.counter += 1
            ck = (0xFF - (self.bcm["speed"] & 0xFF)) & 0xFF
            self._frame(
                0x1A0,
                [self.bcm["ign"] | (self.bcm["doors"] << 2), self.bcm["lights"], self.bcm["speed"], 0, self.counter & 0xFF, ck, 0, 0],
                "BCM_Status",
            )
            if self.counter % 2 == 0:
                self._frame(0x200, [self.vcu["ready"], self.vcu["mode"], 0, 0, 0, 0, 0, 0], "VCU_Heartbeat")
                pv = int(self.bms["pack_v"] * 10)
                self._frame(0x300, [(pv >> 8) & 0xFF, pv & 0xFF, self.bms["soc"], self.bms["soh"], 0, 0, 0, 0], "BMS_Status")
            if self.counter % 5 == 0:
                self._frame(0x302, [self.bms["tmin"] & 0xFF, self.bms["tmax"] & 0xFF, 0, 0, 0, 0, 0, 0], "BMS_Temp")
            if self.bms["tmax"] > 55:
                self._raise_dtc("P1A02", "BMS over-temperature (SIM)")
                self.bms["lim_dch"] = 0
                self.vcu["torque"] = 0
                self.fault = "DEGRADED"

    def snapshot(self) -> dict[str, Any]:
        with self.lock:
            return {
                "mode": self.mode,
                "hw_ok": self.hw_ok,
                "fault": self.fault,
                "session": self.session,
                "vin": self.vin,
                "ble_link": self.ble_link,
                "last_product": self.last_product,
                "bcm": dict(self.bcm),
                "vcu": dict(self.vcu),
                "bms": dict(self.bms),
                "tpms": list(self.tpms),
                "sensor_mv": self.sensor_mv,
                "can": list(self.can[-40:]),
                "dtc": list(self.dtc),
                "log": list(self.log[-20:]),
                "products": [{"id": i, "name": n} for i, n in PRODUCTS],
            }

    def set_mode(self, mode: str) -> dict[str, Any]:
        with self.lock:
            if mode == "hardware":
                self.mode = "hardware"
                self.hw_ok = False
                self._note("Hardware mode: no ESP32-C3 on UART/TCP — staying on Virtual ECU bus")
            else:
                self.mode = "virtual"
                self.hw_ok = False
                self._note("Virtual ECU mode (no hardware)")
        return self.snapshot()

    def run_product(self, pid: str) -> dict[str, Any]:
        with self.lock:
            self.last_product = pid
            handlers = {
                "P01": self._p01,
                "P02": self._p02,
                "P03": self._p03,
                "P04": self._p04,
                "P05": self._p05,
                "P06": self._p06,
                "P07": self._p07,
                "P08": self._p08,
                "P09": self._p09,
                "P10": self._p10,
                "P11": self._p11,
                "P12": self._p12,
                "P13": self._p13,
                "P14": self._p14,
                "P15": self._p15,
                "P16": self._p16,
                "P17": self._p17,
            }
            fn = handlers.get(pid)
            if fn is None:
                self._note(f"{pid} unknown")
            else:
                fn()
        return self.snapshot()

    def _p01(self) -> None:
        self._frame(0x1A2, [0x07], "BCM_Light_Cmd")
        self.bcm["lights"] = 0x07
        self._frame(0x1A3, [0x07, 0, 0, 0, 0, 0, 0, 0], "BCM_Light_Status")
        self._note("P01 BLE-CAN: cmd 0x1A2 bridged, status 0x1A3 notified")

    def _p02(self) -> None:
        self._frame(0x7DF, [0x02, 0x01, 0x0D, 0, 0, 0, 0, 0], "OBD_Req")
        self._frame(0x7E8, [0x03, 0x41, 0x0D, self.bcm["speed"], 0, 0, 0, 0], "OBD_Resp")
        self._note("P02 OBD Mode 01 PID 0D speed")

    def _p03(self) -> None:
        self.session = "EXTENDED"
        self._frame(0x7E0, [0x03, 0x22, 0xF1, 0x90, 0, 0, 0, 0], "UDS_RDBI")
        self._note(f"P03 UDS 0x22 F190 VIN={self.vin}")

    def _p04(self) -> None:
        self._note(f"P04 logger snapshot {len(self.can)} frames")

    def _p05(self) -> None:
        self._raise_dtc("U1A00", "BCM 0x1A0 timeout (SIM)")
        self._note("P05 analyzer: missing cyclic 0x1A0 armed")

    def _p06(self) -> None:
        self._p01()
        self._note("P06 test box GPIO expect OK")

    def _p07(self) -> None:
        self._frame(0x201, [0x00, 0x50, 0, self.counter & 0xFF, 0, 0, 0, 0], "VCU_Torque")
        self._note("P07 simulator BCM/VCU/BMS cyclic")

    def _p08(self) -> None:
        self.drop_id = 0x201
        self.fault = "RECOVERY"
        self._note("P08 inject: drop 0x201 VCU_Torque")

    def _p09(self) -> None:
        self.session = "EXTENDED"
        self._note(f"P09 EOL PASS VIN={self.vin} routine 0x0202")

    def _p10(self) -> None:
        self._note("P10 calibration 0xAA55 written to NVS")

    def _p11(self) -> None:
        mv = self.sensor_mv
        self._frame(0x5B0, [(mv >> 8) & 0xFF, mv & 0xFF, 0x12, 0x34, 0, 0, 0, 0], "Sensor_Pack")
        self._note("P11 ADC 3300 mV + I2C packed")

    def _p12(self) -> None:
        self._frame(0x5A0, self.tpms + [0, 0, 0, 0], "TPMS_Status")
        self._note("P12 TPMS BLE advert -> CAN 0x5A0")

    def _p13(self) -> None:
        self.bms["tmax"] = 60
        self._raise_dtc("P1A02", "BMS over-temperature (SIM)")
        self._raise_dtc("P1A02", "BMS over-temperature (SIM)")
        self._raise_dtc("P1A02", "BMS over-temperature (SIM)")
        self.fault = "DEGRADED"
        self.vcu["torque"] = 0
        self.bms["lim_dch"] = 0
        self._note("P13 EV battery over-temp DTC confirmed")

    def _p14(self) -> None:
        self._frame(0x666, [0xDE, 0xAD, 0, 0, 0, 0, 0, 0], "UNEXPECTED")
        self._raise_dtc("U0001", "Unexpected CAN ID (SIM)")
        self._note("P14 IDS: 0x666 not on allow-list")

    def _p15(self) -> None:
        self._frame(0x1A0, [0x01, 0, 0x2A, 0, 0, 0, 0, 0], "HIL_Inject")
        self._note("P15 HIL inject 0x1A0 + GPIO measure")

    def _p16(self) -> None:
        self._note("P16 training kit: BCM+VCU+BMS lab running")

    def _p17(self) -> None:
        self._note("P17 OTA verify CRC OK; failed image discarded, running slot valid")

    def recover(self) -> dict[str, Any]:
        with self.lock:
            self.drop_id = None
            self.fault = "NORMAL"
            self.bms["tmax"] = 28
            self.bms["lim_dch"] = 200
            self.vcu["torque"] = 80
            self.dtc.clear()
            self._note("Recovered to NORMAL, DTCs cleared")
        return self.snapshot()


ECU = VirtualEcu()


def _ticker() -> None:
    while ECU.running:
        ECU.tick()
        time.sleep(0.1)


class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(GUI_DIR), **kwargs)

    def log_message(self, fmt: str, *args: Any) -> None:
        return

    def _json(self, code: int, payload: Any) -> None:
        body = json.dumps(payload).encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _read_json(self) -> dict[str, Any]:
        n = int(self.headers.get("Content-Length", "0") or "0")
        if n <= 0:
            return {}
        raw = self.rfile.read(n)
        try:
            data = json.loads(raw.decode("utf-8"))
        except json.JSONDecodeError:
            return {}
        return data if isinstance(data, dict) else {}

    def do_GET(self) -> None:  # noqa: N802
        path = urlparse(self.path).path
        if path == "/api/state":
            self._json(200, ECU.snapshot())
            return
        if path == "/docs" or path == "/docs/":
            target = DOCS_DIR / "index.html"
            return self._send_file(target)
        if path.startswith("/docs/"):
            rel = path[len("/docs/") :]
            target = (DOCS_DIR / rel).resolve()
            if str(target).startswith(str(DOCS_DIR.resolve())) and target.is_file():
                return self._send_file(target)
            self.send_error(404)
            return
        if path in ("/", "/index.html"):
            self.path = "/index.html"
        super().do_GET()

    def _send_file(self, target: Path) -> None:
        data = target.read_bytes()
        ctype = "text/html"
        if target.suffix == ".css":
            ctype = "text/css"
        elif target.suffix == ".js":
            ctype = "application/javascript"
        self.send_response(200)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)

    def do_POST(self) -> None:  # noqa: N802
        path = urlparse(self.path).path
        body = self._read_json()
        if path == "/api/mode":
            self._json(200, ECU.set_mode(str(body.get("mode", "virtual"))))
            return
        if path == "/api/product":
            self._json(200, ECU.run_product(str(body.get("id", ""))))
            return
        if path == "/api/recover":
            self._json(200, ECU.recover())
            return
        self._json(404, {"error": "unknown"})


def main() -> None:
    threading.Thread(target=_ticker, daemon=True).start()
    httpd = ThreadingHTTPServer(("127.0.0.1", PORT), Handler)
    print(f"AEGW-C3 GUI  http://127.0.0.1:{PORT}/")
    httpd.serve_forever()


if __name__ == "__main__":
    main()
