# platform/framework

C patterns used everywhere:

- Static ring buffer (ISR-safe put)
- Hierarchical state machine with entry/exit/tick
- Callback table (function pointers, no heap)
- Static object pools
- Const tables for CAN DB and DID maps

C++ equivalents, if any, live in host tools only.
