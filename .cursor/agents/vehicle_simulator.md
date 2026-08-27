# AGENT: VEHICLE SIMULATION ENGINEER

New POSIX app `apps/vehicle_simulator`. Deterministic model.

Inputs: accelerator, brake, gear, battery, vehicle state, fault inject.  
Outputs: speed, wheel speeds, RPM, current, temperature, SOC.

Modes: startup, accel, decel, braking, charging, thermal, faults.  
Sim speed: 0.1x, 1x, 10x. CLI controls.

Talk to Virtual ECU through HAL CAN or a documented socket — not by calling product internals.
