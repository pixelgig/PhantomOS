# Developer setup

Clone https://github.com/pixelgig/PhantomOS.git and work from its root. Install Python 3, Node.js (CI uses 22), and PlatformIO 6.1.18. Use a virtual environment if preferred. Initial dependency installation requires Internet.

    python -m pip install platformio==6.1.18
    node tests/dashboard.test.cjs
    pio run -e nodemcuv2
    pio run -e nodemcuv2 -t buildfs

On Windows use py -m platformio instead of pio if necessary. Build products reside in .pio/build/nodemcuv2/. Do not commit .pio, credentials or private reports. See README for physical flashing instructions.

## File map
- src/main.cpp: firmware initialization, HTTP API, scan state and telemetry.
- include/dashboard.h: embedded HTML stored in program flash.
- dashboard-preview.html: standalone UI with sample data in file mode.
- data/phantom.txt: marker for initial LittleFS image.
- tests/dashboard.test.cjs: Node smoke tests using a simulated browser.
- platformio.ini: target board and pinned platform.
- .github/workflows/build.yml: tests, compile, filesystem build and artifact upload.

## Editing the dashboard
Edit dashboard-preview.html first, then regenerate include/dashboard.h using:

    python scripts/embed_dashboard.py
    node tests/dashboard.test.cjs

The tests check byte-equivalent dashboard text (ignoring outer whitespace) and representative behavior; they are not full browser or board tests. Never insert untrusted SSIDs into HTML markup. Keep the UI dependency-free and verify narrow mobile layouts.

## Extending firmware
Use small testable functions; add API docs for route changes. Check free heap, flash usage and worst-case response sizes. Do not change flash layouts or persist additional sensitive data without documenting migration and privacy. Do not add hardware-specific capabilities to the supported list until tested on that hardware.
