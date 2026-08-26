#!/usr/bin/env python3
"""Verify HTML portal files and relative links."""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DOCS = ROOT / "docs"
HREF = re.compile(r"""(?:href|src)=["']([^"'#]+)""")


def main() -> int:
    errors: list[str] = []
    required = [
        DOCS / "index.html",
        DOCS / "presentation.html",
        DOCS / "assets" / "style.css",
        DOCS / "assets" / "nav.js",
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
