# Agent 03 — C/C++ firmware (Phase 4)

MISRA-inspired C11 on the MCU. Host tools may use restricted C++17.

Demonstrate in `platform/framework/`: volatile variable, volatile pointer, const pointer, function pointer callbacks, static ring buffer, static pool list, clear ownership.

No heap on data paths. No C++ on the MCU without a written justification.

Each public function: banner + block comment. Line comments on non-obvious lines only.

Pair with Agent 15 for unit tests.
