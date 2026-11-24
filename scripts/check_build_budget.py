"""Check the firmware resource report emitted by PlatformIO."""
import re
import sys
from pathlib import Path

log = Path(sys.argv[1]).read_text(encoding="utf-8")
for resource, budget in (("RAM", 45), ("Flash", 70)):
    match = re.search(
        rf"(?m)^{resource}:.*?used (\d+) bytes from (\d+) bytes",
        log,
    )
    if not match:
        raise SystemExit(f"Missing PlatformIO {resource} report")
    used, available = map(int, match.groups())
    if available <= 0 or used * 100 > available * budget:
        raise SystemExit(f"{resource} budget exceeded: {used}/{available} bytes (limit {budget}%)")
    print(f"{resource}: {used}/{available} bytes, within {budget}% budget")

if Path("dashboard-preview.html").stat().st_size > 32 * 1024:
    raise SystemExit("Dashboard exceeds 32 KiB")
