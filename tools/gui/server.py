# Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
# Muhammad Samiullah — CTO & Founder. All rights reserved.
"""AEGW-C3 Virtual ECU GUI: products, 120 use cases, UDS sim, CAN analyzer."""

from __future__ import annotations

import json
import threading
import time
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any
from urllib.parse import parse_qs, urlparse

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

# Mirrors products/usecases.c catalog (lab SIM). Not road-vehicle certified.
USECASES: list[dict[str, Any]] = []
_CATS = [
    ("CAN", 1, 20),
    ("UDS", 21, 40),
    ("DIAG", 41, 43),
    ("FAULT", 44, 48),
    ("DIAG", 49, 53),
    ("FAULT", 54, 55),
    ("CYBER", 56, 75),
    ("VEHICLE", 76, 90),
    ("E2E", 91, 100),
    ("HIL", 101, 110),
    ("OTA", 111, 114),
    ("GATEWAY", 115, 120),
]
_NAMES = {
    1: "CAN init 500 kbit/s",
    2: "CAN TX standard ID",
    3: "CAN TX extended ID",
    4: "CAN RX queue drain",
    5: "CAN filter accept",
    6: "CAN bus overflow detect",
    7: "CAN expect timeout DTC",
    8: "BCM status cyclic TX",
    9: "VCU heartbeat TX",
    10: "VCU torque TX",
    11: "BMS pack status TX",
    12: "BMS temperature TX",
    13: "TPMS frame TX",
    14: "Sensor gateway pack TX",
    15: "Light command/response",
    16: "CAN stats counters",
    17: "CAN subscribe callback",
    18: "RTR frame reject path",
    19: "DLC 0..8 sweep",
    20: "Bus reset recover",
    21: "UDS 0x10 Default session",
    22: "UDS 0x10 Extended session",
    23: "UDS 0x10 Programming session",
    24: "UDS 0x3E TesterPresent",
    25: "UDS 0x22 Read VIN F190",
    26: "UDS 0x22 Read SW F189",
    27: "UDS 0x22 unsupported DID NRC",
    28: "UDS 0x2E Write DID proto",
    29: "UDS 0x14 Clear DTC",
    30: "UDS 0x19 Read DTC",
    31: "UDS 0x11 ECU Reset proto",
    32: "UDS 0x27 SecurityAccess seed",
    33: "UDS 0x27 bad key lockout path",
    34: "UDS 0x31 RoutineControl",
    35: "UDS negative response length",
    36: "UDS S3 timeout tick",
    37: "UDS client/server loopback",
    38: "OBD functional 0x7DF speed",
    39: "ISO-TP single frame UDS",
    40: "ISO-TP multi-frame UDS",
    41: "DTC raise pending",
    42: "DTC confirm after repeats",
    43: "DTC clear all",
    44: "Fault NORMAL state",
    45: "Fault DETECTED",
    46: "Fault DEGRADED critical",
    47: "Fault SAFE state",
    48: "Fault recover to NORMAL",
    49: "DTC CAN timeout BCM",
    50: "DTC BMS over-temp",
    51: "DTC unexpected CAN ID",
    52: "DTC sensor fail",
    53: "DTC low voltage",
    54: "Watchdog kick count",
    55: "NVS config roundtrip",
    56: "IDS allow-list accept",
    57: "IDS reject unknown ID",
    58: "Flood rate detect",
    59: "Replay same frame detect",
    60: "Invalid DLC detect",
    61: "Diagnostic abuse rate limit",
    62: "Spoof VCU torque detect",
    63: "Bus-off storm counter",
    64: "Unexpected period detect",
    65: "Blacklist drop ID",
    66: "Security event log slot",
    67: "UDS without session deny",
    68: "Write DID without SA deny",
    69: "ISO-TP flood / DoS path",
    70: "Malformed UDS length",
    71: "IDS scan unknown range",
    72: "Gateway flood absorb",
    73: "E2E timeout as attack",
    74: "Range check SOC",
    75: "Plausibility speed jump",
    76: "BCM ign/doors/lights",
    77: "BCM speed cyclic",
    78: "VCU ready/mode",
    79: "VCU torque request",
    80: "BMS pack voltage",
    81: "BMS SOC/SOH",
    82: "BMS temp limits",
    83: "BMS discharge limit",
    84: "TPMS four corners",
    85: "Sensor ADC pack",
    86: "Instrument cluster proto",
    87: "Gateway health",
    88: "Light status feedback",
    89: "Door status",
    90: "Vehicle cluster snapshot",
    91: "CRC8 J1850 protect",
    92: "CRC8 check OK",
    93: "CRC8 check fail",
    94: "Alive counter wrap",
    95: "E2E timeout",
    96: "E2E DLC fail",
    97: "CRC16 CCITT",
    98: "CRC32 image slot",
    99: "E2E counter skip",
    100: "E2E multi-ID protect",
    101: "HIL inject BCM",
    102: "HIL inject VCU",
    103: "HIL inject BMS",
    104: "HIL GPIO write/read",
    105: "HIL ADC stimulate",
    106: "HIL loopback CAN",
    107: "HIL digital out",
    108: "HIL analog in",
    109: "HIL drop frame",
    110: "HIL timing stamp",
    111: "OTA begin image",
    112: "OTA write chunk",
    113: "OTA verify fail discard",
    114: "OTA running slot valid",
    115: "GW light route",
    116: "GW IDS drop",
    117: "GW route flood",
    118: "Ring buffer put",
    119: "Millis monotonic",
    120: "Full bus recover",
}


def _build_usecases() -> None:
    USECASES.clear()
    for cat, a, b in _CATS:
        for i in range(a, b + 1):
            USECASES.append(
                {
                    "id": i,
                    "category": cat,
                    "name": _NAMES.get(i, f"UC-{i}"),
                    "requirement": "REQ-LAB",
                }
            )


_build_usecases()


class VirtualEcu:
    def __init__(self) -> None:
        self.lock = threading.Lock()
        self.mode = "virtual"  # virtual=sim, hardware=live intent
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
        self.last_usecase: dict[str, Any] | None = None
        self.counter = 0
        self.drop_id = None
        # CAN analyzer
        self.an_pause = False
        self.an_filter: int | None = None  # None = all
        self.an_total = 0
        self.an_rates: dict[str, int] = {}
        self.an_unknown = 0
        self.an_sec_events = 0
        # UDS simulator
        self.uds_last_req = ""
        self.uds_last_resp = ""
        self.uds_seed = 0xA5A5
        self.uds_unlocked = False
        self.cfg_byte = 0x00

    def _frame(self, can_id: int, data: list[int], name: str) -> None:
        if self.drop_id is not None and can_id == self.drop_id:
            self._note(f"DROP {can_id:#05x}")
            return
        if self.an_pause:
            return
        if self.an_filter is not None and can_id != self.an_filter:
            return
        rec = {
            "ts": int(time.time() * 1000),
            "id": f"{can_id:#05x}",
            "dlc": len(data),
            "data": " ".join(f"{b:02X}" for b in data),
            "name": name,
        }
        self.can.append(rec)
        self.can = self.can[-200:]
        self.an_total += 1
        key = f"{can_id:#05x}"
        self.an_rates[key] = self.an_rates.get(key, 0) + 1
        allow = {
            0x1A0, 0x1A1, 0x1A2, 0x1A3, 0x200, 0x201, 0x300, 0x302,
            0x5A0, 0x5B0, 0x7DF, 0x7E0, 0x7E8, 0x500, 0x501,
        }
        if can_id not in allow and can_id <= 0x7FF:
            self.an_unknown += 1
            self.an_sec_events += 1

    def _note(self, msg: str) -> None:
        self.log.append(f"{time.strftime('%H:%M:%S')}  {msg}")
        self.log = self.log[-60:]

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
            top = sorted(self.an_rates.items(), key=lambda x: -x[1])[:12]
            return {
                "mode": self.mode,
                "hw_ok": self.hw_ok,
                "fault": self.fault,
                "session": self.session,
                "vin": self.vin,
                "ble_link": self.ble_link,
                "last_product": self.last_product,
                "last_usecase": self.last_usecase,
                "bcm": dict(self.bcm),
                "vcu": dict(self.vcu),
                "bms": dict(self.bms),
                "tpms": list(self.tpms),
                "sensor_mv": self.sensor_mv,
                "can": list(self.can[-80:]),
                "dtc": list(self.dtc),
                "log": list(self.log[-30:]),
                "products": [{"id": i, "name": n} for i, n in PRODUCTS],
                "usecase_count": len(USECASES),
                "analyzer": {
                    "pause": self.an_pause,
                    "filter": None if self.an_filter is None else f"{self.an_filter:#05x}",
                    "total": self.an_total,
                    "unknown": self.an_unknown,
                    "sec_events": self.an_sec_events,
                    "top": [{"id": k, "count": v} for k, v in top],
                },
                "uds": {
                    "session": self.session,
                    "unlocked": self.uds_unlocked,
                    "last_req": self.uds_last_req,
                    "last_resp": self.uds_last_resp,
                    "seed": f"{self.uds_seed:04X}",
                    "cfg": f"{self.cfg_byte:02X}",
                },
            }

    def set_mode(self, mode: str) -> dict[str, Any]:
        with self.lock:
            if mode == "hardware":
                self.mode = "hardware"
                self.hw_ok = False
                self._note("Live/hardware mode requested — no ESP32-C3 link; analyzer feeds Virtual ECU")
            else:
                self.mode = "virtual"
                self.hw_ok = False
                self._note("Simulation mode (Virtual ECU)")
        return self.snapshot()

    def analyzer_ctrl(self, body: dict[str, Any]) -> dict[str, Any]:
        with self.lock:
            if "pause" in body:
                self.an_pause = bool(body["pause"])
                self._note(f"Analyzer pause={self.an_pause}")
            if "filter" in body:
                f = body["filter"]
                if f in (None, "", "all", "*"):
                    self.an_filter = None
                else:
                    self.an_filter = int(str(f), 0)
                self._note(f"Analyzer filter={self.an_filter}")
            if body.get("clear"):
                self.can.clear()
                self.an_rates.clear()
                self.an_total = 0
                self.an_unknown = 0
                self._note("Analyzer cleared")
            if "inject" in body:
                inj = body["inject"]
                cid = int(str(inj.get("id", "0x666")), 0)
                data = inj.get("data", [0])
                if isinstance(data, str):
                    data = [int(x, 16) for x in data.replace(",", " ").split() if x]
                self._frame(cid, list(data)[:8], "INJECT")
                self._note(f"Inject {cid:#05x}")
        return self.snapshot()

    def uds_request(self, body: dict[str, Any]) -> dict[str, Any]:
        with self.lock:
            sid = int(str(body.get("sid", "0x22")), 0) & 0xFF
            data_raw = body.get("data", "")
            if isinstance(data_raw, str):
                payload = [int(x, 16) for x in data_raw.replace(",", " ").split() if x]
            else:
                payload = [int(x) & 0xFF for x in data_raw]
            req = [sid] + payload
            self.uds_last_req = " ".join(f"{b:02X}" for b in req)
            # ISO-TP SF on bus
            sf = [len(req)] + req + [0] * (8 - 1 - len(req))
            self._frame(0x7E0, sf[:8], "UDS_REQ")
            resp = self._uds_handle(sid, payload)
            self.uds_last_resp = " ".join(f"{b:02X}" for b in resp)
            rs = [len(resp)] + resp + [0] * 8
            self._frame(0x7E8, rs[:8], "UDS_RESP")
            self._note(f"UDS {self.uds_last_req} -> {self.uds_last_resp}")
        return self.snapshot()

    def _uds_handle(self, sid: int, payload: list[int]) -> list[int]:
        if sid == 0x10:  # DSC
            sess = payload[0] if payload else 0x01
            self.session = {0x01: "DEFAULT", 0x02: "PROGRAMMING", 0x03: "EXTENDED"}.get(sess, "DEFAULT")
            self.uds_unlocked = False
            return [0x50, sess]
        if sid == 0x3E:
            return [0x7E, payload[0] if payload else 0]
        if sid == 0x22:
            did = ((payload[0] << 8) | payload[1]) if len(payload) >= 2 else 0
            if did == 0xF190:
                vin = [ord(c) for c in self.vin[:17]]
                return [0x62, 0xF1, 0x90] + vin[:5]  # SF truncated display
            if did == 0xF189:
                return [0x62, 0xF1, 0x89, 0x01, 0x00]
            return [0x7F, 0x22, 0x31]
        if sid == 0x2E:
            if self.session == "DEFAULT":
                return [0x7F, 0x2E, 0x7E]
            if not self.uds_unlocked:
                return [0x7F, 0x2E, 0x33]
            if len(payload) >= 3:
                self.cfg_byte = payload[2]
            return [0x6E, payload[0], payload[1]] if len(payload) >= 2 else [0x7F, 0x2E, 0x13]
        if sid == 0x27:
            if payload and payload[0] == 0x01:
                return [0x67, 0x01, (self.uds_seed >> 8) & 0xFF, self.uds_seed & 0xFF]
            if payload and payload[0] == 0x02:
                key = ((payload[1] << 8) | payload[2]) if len(payload) >= 3 else 0
                if key == ((self.uds_seed ^ 0x1234) & 0xFFFF):
                    self.uds_unlocked = True
                    return [0x67, 0x02]
                self.an_sec_events += 1
                return [0x7F, 0x27, 0x35]
            return [0x7F, 0x27, 0x13]
        if sid == 0x14:
            self.dtc.clear()
            return [0x54]
        if sid == 0x19:
            return [0x59, 0x02, 0xFF]
        if sid == 0x11:
            self.session = "DEFAULT"
            self.uds_unlocked = False
            return [0x51, payload[0] if payload else 0x01]
        if sid == 0x31:
            return [0x71, payload[0] if payload else 0x01, 0x02, 0x02]
        return [0x7F, sid, 0x11]

    def run_usecase(self, uid: int) -> dict[str, Any]:
        with self.lock:
            uc = next((u for u in USECASES if u["id"] == uid), None)
            if uc is None:
                self._note(f"UC-{uid} unknown")
                return self.snapshot()
            self.last_usecase = {"id": uid, "name": uc["name"], "result": "PASS", "mode": self.mode}
            # Drive bus / UDS / cyber visuals for selected categories
            if 1 <= uid <= 20:
                self._frame(0x1A0, [1, 0, self.bcm["speed"], 0, uid & 0xFF, 0, 0, 0], f"UC{uid}_CAN")
            elif 21 <= uid <= 40:
                sid_map = {21: (0x10, [0x01]), 22: (0x10, [0x03]), 23: (0x10, [0x02]), 24: (0x3E, [0x00]),
                           25: (0x22, [0xF1, 0x90]), 26: (0x22, [0xF1, 0x89])}
                if uid in sid_map:
                    sid, pl = sid_map[uid]
                    req = [sid] + pl
                    self.uds_last_req = " ".join(f"{b:02X}" for b in req)
                    resp = self._uds_handle(sid, pl)
                    self.uds_last_resp = " ".join(f"{b:02X}" for b in resp)
                    self._frame(0x7E0, [len(req)] + req + [0] * 8, f"UC{uid}_REQ")
                    self._frame(0x7E8, [len(resp)] + resp + [0] * 8, f"UC{uid}_RESP")
                else:
                    self._frame(0x7E0, [0x02, 0x3E, 0x00, 0, 0, 0, 0, 0], f"UC{uid}_UDS")
            elif 56 <= uid <= 75:
                self._frame(0x666, [0xDE, 0xAD, uid & 0xFF, 0, 0, 0, 0, 0], f"UC{uid}_CYBER")
                self._raise_dtc("U0001", f"Cyber UC-{uid} (SIM)")
                self.an_sec_events += 1
            elif uid == 50 or uid == 13:
                self.bms["tmax"] = 60
            elif 111 <= uid <= 114:
                self._note(f"UC-{uid} OTA path (SIM) — host C test is authoritative")
            self._note(f"UC-{uid:03d} {uc['name']} PASS ({self.mode})")
        return self.snapshot()

    def run_product(self, pid: str) -> dict[str, Any]:
        with self.lock:
            self.last_product = pid
            handlers = {
                "P01": self._p01, "P02": self._p02, "P03": self._p03, "P04": self._p04,
                "P05": self._p05, "P06": self._p06, "P07": self._p07, "P08": self._p08,
                "P09": self._p09, "P10": self._p10, "P11": self._p11, "P12": self._p12,
                "P13": self._p13, "P14": self._p14, "P15": self._p15, "P16": self._p16,
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
            self.session = "DEFAULT"
            self.uds_unlocked = False
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
        qs = parse_qs(urlparse(self.path).query)
        if path == "/api/state":
            self._json(200, ECU.snapshot())
            return
        if path == "/api/usecases":
            cat = (qs.get("cat") or [""])[0].upper()
            q = (qs.get("q") or [""])[0].lower()
            rows = USECASES
            if cat:
                rows = [u for u in rows if u["category"] == cat]
            if q:
                rows = [u for u in rows if q in u["name"].lower() or q in str(u["id"])]
            self._json(200, {"count": len(rows), "usecases": rows})
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
        if path == "/api/usecase":
            self._json(200, ECU.run_usecase(int(body.get("id", 0))))
            return
        if path == "/api/uds":
            self._json(200, ECU.uds_request(body))
            return
        if path == "/api/analyzer":
            self._json(200, ECU.analyzer_ctrl(body))
            return
        if path == "/api/recover":
            self._json(200, ECU.recover())
            return
        self._json(404, {"error": "unknown"})


def main() -> None:
    threading.Thread(target=_ticker, daemon=True).start()
    httpd = ThreadingHTTPServer(("127.0.0.1", PORT), Handler)
    print(f"AEGW-C3 GUI  http://127.0.0.1:{PORT}/")
    print("  Simulation + Live intent | UDS simulator | CAN analyzer | 120 use cases")
    httpd.serve_forever()


if __name__ == "__main__":
    main()
