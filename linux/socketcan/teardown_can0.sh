#!/usr/bin/env bash
# Tear down SocketCAN interface.
set -euo pipefail
IFACE="${1:-can0}"
ip link set "${IFACE}" down
echo "Down ${IFACE}"
