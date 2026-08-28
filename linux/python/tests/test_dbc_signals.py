"""Signal pack/unpack tests (no hardware). REQ-LAB-013."""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from aegw_can.signals import BcmStatus, pack_bcm_status, unpack_bcm_status, pack_vcu_torque, unpack_vcu_torque


def main() -> int:
    s = BcmStatus(ignition=1, doors=2, lights=0x0F, speed_kmh=42, alive=3, crc=0xAB)
    raw = pack_bcm_status(s)
    u = unpack_bcm_status(raw)
    assert u.ignition == 1 and u.doors == 2 and u.speed_kmh == 42 and u.alive == 3
    t, b, a = unpack_vcu_torque(pack_vcu_torque(-10, 1, 40.0))
    assert t == -10 and b == 1 and abs(a - 40.0) < 0.5
    print("PASS test_dbc_signals")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
