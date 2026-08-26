/**
 * @file ae_can_ids.h
 * @brief Prototype / simulated CAN identifiers. Not OEM production IDs.
 */
#ifndef AE_CAN_IDS_H
#define AE_CAN_IDS_H

#define AE_CAN_BCM_STATUS      0x1A0u
#define AE_CAN_BCM_DOOR        0x1A1u
#define AE_CAN_BCM_LIGHT_CMD   0x1A2u
#define AE_CAN_BCM_LIGHT_ST    0x1A3u
#define AE_CAN_VCU_HB          0x200u
#define AE_CAN_VCU_TORQUE      0x201u
#define AE_CAN_VCU_STATE       0x202u
#define AE_CAN_BMS_STATUS      0x300u
#define AE_CAN_BMS_CELL        0x301u
#define AE_CAN_BMS_TEMP        0x302u
#define AE_CAN_BMS_LIMITS      0x303u
#define AE_CAN_IC_DISPLAY      0x400u
#define AE_CAN_IC_TELLTALE     0x401u
#define AE_CAN_GW_HEALTH       0x500u
#define AE_CAN_GW_ROUTE        0x501u
#define AE_CAN_TPMS            0x5A0u
#define AE_CAN_SENSOR          0x5B0u
#define AE_CAN_OBD_FUNC        0x7DFu
#define AE_CAN_UDS_REQ         0x7E0u
#define AE_CAN_UDS_RESP        0x7E8u

#define AE_DTC_CAN_TIMEOUT_BCM 0xC1A000u
#define AE_DTC_BMS_COMM        0x51A000u
#define AE_DTC_BMS_OTEMP       0x51A002u
#define AE_DTC_TPMS_LOST       0x81A000u
#define AE_DTC_LOW_VOLTAGE     0x51A010u
#define AE_DTC_SENSOR_FAIL     0xC1A010u
#define AE_DTC_COMMS           0xC10000u

#define AE_DID_VIN             0xF190u
#define AE_DID_SW              0xF189u
#define AE_DID_HW              0xF191u

#define AE_VIN_PROTO           "AEGWC3PROTO00001"

#endif /* AE_CAN_IDS_H */
