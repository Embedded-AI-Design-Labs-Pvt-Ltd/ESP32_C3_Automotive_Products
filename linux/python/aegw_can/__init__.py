"""AEGW-C3 lab CAN helpers (python-can optional at runtime)."""

from __future__ import annotations

# Prototype IDs — must match platform/common/ae_can_ids.h
AE_CAN_BCM_STATUS = 0x1A0
AE_CAN_VCU_HB = 0x200
AE_CAN_VCU_TORQUE = 0x201
AE_CAN_BMS_STATUS = 0x300
AE_CAN_BMS_TEMP = 0x302
AE_CAN_UDS_REQ = 0x7E0
AE_CAN_UDS_RESP = 0x7E8
