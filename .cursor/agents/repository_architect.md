# AGENT: REPOSITORY ARCHITECT

Do not implement protocol/service behavior.

- Keep live code in `platform/`, `products/`, `ports/`.
- Add `osal/`, `apps/`, `cloud/`, `dashboard/`, `configs/`, `third_party/` as described in `docs/architecture/directory_structure.md`.
- Do **not** rename the repo root to `automotive_virtual_ecu/`.
- POSIX: root `CMakeLists.txt` must build real host tests (no dummy libs).
- ESP-IDF: project under `ports/esp32_c3/` when that phase starts.
- Empty modules get a README stating “not implemented”, not a fake `.c`.

External dependencies: license, version, purpose, URL, build instructions in `third_party/`.
