#!/usr/bin/env python3
# Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
# Muhammad Samiullah — CTO & Founder. All rights reserved.
"""Verify HTML portal files, relative links, and copyright footer."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DOCS = ROOT / "docs"
HREF = re.compile(r"""(?:href|src)=["']([^"'#]+)""")
REQUIRED_FOOTER = (
    "Muhammad Samiullah",
    "CTO",
    "2026",
)


def main() -> int:
    errors: list[str] = []
    required = [
        DOCS / "index.html",
        DOCS / "presentation.html",
        DOCS / "assets" / "style.css",
        DOCS / "assets" / "nav.js",
        DOCS / "assets" / "logo.png",
    ]
    required += sorted((DOCS / "pages").glob("*.html"))
    for path in required:
        if not path.is_file():
            errors.append(f"missing {path}")

    html_files = [DOCS / "index.html", DOCS / "presentation.html"]
    html_files += sorted((DOCS / "pages").glob("*.html"))
    for html in html_files:
        text = html.read_text(encoding="utf-8")
        if "<html" not in text.lower():
            errors.append(f"not html: {html}")
        for needle in REQUIRED_FOOTER:
            if needle not in text:
                errors.append(f"missing footer '{needle}' in {html.name}")
        for match in HREF.findall(text):
            if match.startswith(("http://", "https://", "mailto:", "javascript:")):
                continue
            target = (html.parent / match).resolve()
            if not target.exists():
                errors.append(f"broken {html.name} -> {match}")

    print(f"checked {len(html_files)} html files")
    if errors:
        for err in errors:
            print("FAIL", err)
        return 1
    print("PASS docs links")
    return 0


if __name__ == "__main__":
    sys.exit(main())
