# Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
# Muhammad Samiullah — CTO & Founder. All rights reserved.

from __future__ import annotations

import shutil
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
SRC = REPO / "ports" / "arduino" / "AEGW_C3" / "src"

HEADERS = [
    "platform/common/ae_types.h",
    "platform/common/ae_error.h",
    "platform/common/ae_can_ids.h",
    "platform/common/ae_copyright.h",
    "platform/framework/ae_ring.h",
    "platform/hal/hal_can.h",
    "platform/hal/hal_can_adapter.h",
    "platform/hal/hal_misc.h",
    "platform/protocols/can_service.h",
    "platform/protocols/crc_e2e.h",
    "platform/protocols/isotp.h",
    "platform/protocols/uds.h",
    "platform/services/dtc.h",
    "platform/services/ble_auto.h",
    "platform/services/fault_mgr.h",
    "platform/services/ota_agent.h",
    "platform/services/ecu_models.h",
    "products/product_api.h",
    "products/product_dids.h",
    "products/usecases.h",
    "ports/arduino/aegw_runtime.h",
]

UNITS = [
    "platform/common/ae_error.c",
    "platform/framework/ae_ring.c",
    "platform/protocols/can_service.c",
    "platform/protocols/crc_e2e.c",
    "platform/protocols/isotp.c",
    "platform/protocols/uds.c",
    "platform/services/dtc.c",
    "platform/services/ble_auto.c",
    "platform/services/fault_mgr.c",
    "platform/services/ota_agent.c",
    "platform/services/ecu_models.c",
    "products/product_dids.c",
    "products/products_connectivity.c",
    "products/products_diagnostics.c",
    "products/products_validation.c",
    "products/products_security_ota.c",
    "products/products_registry.c",
    "products/usecases.c",
    "ports/arduino/aegw_runtime.c",
]


def main() -> None:
    SRC.mkdir(parents=True, exist_ok=True)
    for repo_rel in HEADERS + UNITS:
        src = REPO / repo_rel
        dst = SRC / Path(repo_rel).name
        shutil.copy2(src, dst)
    print("copied", len(HEADERS) + len(UNITS), "files into", SRC)


if __name__ == "__main__":
    main()
