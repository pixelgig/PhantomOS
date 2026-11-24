# Phantom OS project direction

Goal: a portable, zero-recurring-cost, local-first electronics/network diagnostic toolkit for anyone using a compatible ESP8266 NodeMCU and Android phone. No permanent computer or cloud is required. Initial computer access is only for compiling/flashing. Make a modular design that can later be ported to ESP32-S3.

## Architecture
- ESP8266 Arduino core via PlatformIO, AP+STA mode for scanning with standalone AP and no configured uplink.
- Embedded HTML/CSS/JS in program flash; small JSON HTTP API, local scan cache. LittleFS for access point key only. No network traffic capture or credential collection.
- Companion phone is the display. Future native Android app can add device-local BLE/NFC capabilities when supported by the phone and user permissions.
- Future GPIO/UART diagnostics require voltage verification, appropriate wiring and an authorized target. External radios require hardware and firmware validation. Do not promise Flipper-equivalent functionality.

## Milestones
1. v0.1: source, preview, build and first hardware smoke test.
2. v0.2: source implemented for scan/heap telemetry, UTF-8 SSID handling, phone-local snapshots, JSON export and opt-in dashboard monitor. Hardware validation pending; settings and uplink diagnostics deferred.
3. v0.3: optional owner-network station mode, DHCP telemetry and explicit DNS lookup implemented; no routing, ping or speed test; hardware validation pending. Native Android companion planned for v0.5.
4. v0.4: GPIO and UART tools for owner-controlled hardware, with safe pin selection.
5. Later: optional ESP32-S3 port, hardware-dependent extensions, browser flasher only after verified binaries.

## Rules
Use free/open tools; prefer no external services; do not expose device publicly; avoid disruptive features; keep flash/RAM limits in view. Every change must include test status, known limitations, and migration notes. Read this file and AGENTS.md before editing.

## Public project policy
The project is open to everyone under the MIT license for original code. ROADMAP.md and docs/ are the permanent public project records. Keep this file and AGENTS.md only during development; before v1.0 migrate remaining useful information and delete both from the active repository as required by docs/RELEASING.md. Do not rewrite Git history.

## Standalone development checkpoint

Gate A defaults to standalone Wi-Fi scanning. The schema-versioned capabilities route reports compiled support. Keep experimental station tooling gated pending request-parser and isolation review. License provenance and actual ESP8266 validation remain release gates.
