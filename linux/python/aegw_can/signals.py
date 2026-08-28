"""Minimal DBC-aligned signal pack/unpack for lab tests (no cantools required)."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass
class BcmStatus:
    ignition: int
    doors: int
    lights: int
    speed_kmh: int
    alive: int = 0
    crc: int = 0


def pack_bcm_status(s: BcmStatus) -> bytes:
    data = bytearray(8)
    data[0] = (s.ignition & 1) | ((s.doors & 0xF) << 1)
    data[1] = s.lights & 0xFF
    data[2] = s.speed_kmh & 0xFF
    data[6] = s.crc & 0xFF
    data[7] = ((s.alive & 0xF) << 4) | (data[7] & 0x0F)
    return bytes(data)


def unpack_bcm_status(data: bytes) -> BcmStatus:
    if len(data) < 8:
        raise ValueError("need 8 bytes")
    return BcmStatus(
        ignition=data[0] & 1,
        doors=(data[0] >> 1) & 0xF,
        lights=data[1],
        speed_kmh=data[2],
        alive=(data[7] >> 4) & 0xF,
        crc=data[6],
    )


def pack_vcu_torque(torque_nm: int, brake: int, accel_pct: float) -> bytes:
    data = bytearray(8)
    t = int(torque_nm) & 0xFFFF
    data[0] = t & 0xFF
    data[1] = (t >> 8) & 0xFF
    data[2] = brake & 1
    data[3] = int(max(0, min(100, accel_pct)) / 0.4) & 0xFF
    return bytes(data)


def unpack_vcu_torque(data: bytes) -> tuple[int, int, float]:
    if len(data) < 4:
        raise ValueError("need >=4 bytes")
    t = data[0] | (data[1] << 8)
    if t >= 0x8000:
        t -= 0x10000
    brake = data[2] & 1
    accel = data[3] * 0.4
    return t, brake, accel
