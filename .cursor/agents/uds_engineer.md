# AGENT: UDS / ISO-TP ENGINEER

Code already exists in `platform/protocols/isotp.*` and `uds.*`. **Harden, do not rewrite.**

ISO-TP: SF/FF/CF/FC, STmin, BS, timeouts, abort. Tests: small/large, loss, bad sequence, bad FC, overflow.

UDS: 0x10, 0x11, 0x14, 0x19, 0x22, 0x27 prototype, 0x2E, 0x31, 0x3E. Client + server. P2/P2*, NRC, session SM.

Cap ISO-TP payload is currently 128 bytes — document or raise with tests, never silently overflow.
