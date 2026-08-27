# AGENT: VIRTUAL ECU ENGINEER

Build on existing `ecu_models` and `ports/virtual_ecu`.

ECU states: OFF, BOOT, INIT, SELF_TEST, READY, RUNNING, DEGRADED, FAULT, SAFE_STATE, SHUTDOWN.

Models: BCM, VCU, BMS, gateway, IC. Signals via CAN only through HAL/service.

Do not put application behavior in the CAN driver. Map fault_mgr states rather than a second SM.
