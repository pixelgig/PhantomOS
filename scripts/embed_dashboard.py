"""Regenerate the ESP8266 dashboard header from the offline preview."""
from pathlib import Path

root = Path(__file__).resolve().parents[1]
html = (root / "dashboard-preview.html").read_text(encoding="utf-8").rstrip() + "\n"
if ')PHANTOM"' in html:
    raise SystemExit("Dashboard contains the C++ raw string closing delimiter")
header = '#pragma once\n#include <Arduino.h>\nstatic const char DASHBOARD[] PROGMEM = R"PHANTOM(\n' + html + ')PHANTOM";\n'
(root / "include/dashboard.h").write_text(header, encoding="utf-8", newline="\n")
