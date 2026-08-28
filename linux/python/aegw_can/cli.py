#!/usr/bin/env python3
"""CLI for lab CAN TX/RX. Uses python-can when installed; otherwise prints guidance."""

from __future__ import annotations

import argparse
import sys
import time


def main() -> int:
    p = argparse.ArgumentParser(description="AEGW-C3 lab CAN CLI")
    p.add_argument("--iface", default="can0")
    p.add_argument("--bitrate", type=int, default=500000)
    p.add_argument("--bustype", default="socketcan", help="socketcan|virtual|pcan")
    sub = p.add_subparsers(dest="cmd", required=True)

    dump = sub.add_parser("dump", help="Receive and print frames")
    dump.add_argument("--count", type=int, default=20)

    send = sub.add_parser("send", help="Send one Classical CAN frame")
    send.add_argument("--id", type=lambda x: int(x, 0), required=True)
    send.add_argument("--data", default="00", help="hex bytes e.g. 01AABB")

    args = p.parse_args()
    try:
        import can  # type: ignore
    except ImportError:
        print("python-can not installed. On Linux: pip install python-can", file=sys.stderr)
        print("Host unit signal tests do not need the adapter.", file=sys.stderr)
        return 2

    bus = can.Bus(channel=args.iface, bustype=args.bustype, bitrate=args.bitrate)
    try:
        if args.cmd == "dump":
            n = 0
            while n < args.count:
                msg = bus.recv(timeout=1.0)
                if msg is None:
                    continue
                print(f"{msg.arbitration_id:03X}  {msg.data.hex()}")
                n += 1
        elif args.cmd == "send":
            raw = bytes.fromhex(args.data)
            if len(raw) > 8:
                print("Classical CAN DLC max 8", file=sys.stderr)
                return 1
            msg = can.Message(arbitration_id=args.id, data=raw, is_extended_id=False)
            bus.send(msg)
            print(f"sent {args.id:03X} {raw.hex()}")
            time.sleep(0.05)
    finally:
        bus.shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
