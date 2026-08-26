# Agent 04 — FreeRTOS (Phase 5)

Implement the task catalog in `docs/pages/04-task-architecture.html` as a **subset demo**: WDT, health, logger, one worker.

Use queues, mutex (inheritance), event group, task notify, software timer as specified. Document why.

Never take a mutex in an ISR. Watchdog never blocks on a mutex.

Provide a task-state comment in each task file.
