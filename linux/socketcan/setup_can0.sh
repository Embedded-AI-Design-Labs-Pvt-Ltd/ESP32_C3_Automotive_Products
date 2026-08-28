#!/usr/bin/env bash
# Setup Classical CAN SocketCAN interface (CANable / PCAN / similar).
# Usage: sudo ./setup_can0.sh [iface] [bitrate]
# Example: sudo ./setup_can0.sh can0 500000
set -euo pipefail
IFACE="${1:-can0}"
BITRATE="${2:-500000}"

echo "Bringing up ${IFACE} @ ${BITRATE} (Classical CAN, not CAN-FD)"
ip link set "${IFACE}" down 2>/dev/null || true
ip link set "${IFACE}" type can bitrate "${BITRATE}"
ip link set "${IFACE}" up
ip -details link show "${IFACE}"
echo "OK. Try: candump ${IFACE}"
