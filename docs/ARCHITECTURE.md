# Architecture

## Current implementation
Phantom OS is an offline-first toolkit targeting a 4 MB ESP8266 NodeMCU (PlatformIO nodemcuv2). A phone browser provides the interface. Firmware uses Arduino C++ with ESP8266WiFi, ESP8266WebServer and LittleFS.

    Phone browser (private WPA2 AP, HTTP)
          |
    ESP8266WebServer --> JSON routes /api/status, /api/scan
          |                  |
    Embedded dashboard      Wi-Fi SDK asynchronous scan
    (program flash)         bounded JSON cache in RAM
          |
    LittleFS /ap.key (credential persistence only)

AP+STA mode permits scanning; there is no configured station uplink. setup() starts serial, loads or creates an AP key and registers routes. loop() services requests, polls scan completion and samples heap. Scan results are capped at the strongest 32 entries; channel totals use up to 255 SDK results. Cooldown is 15 seconds, and timeout reports an error without forcibly cancelling the SDK scan.

## Storage and boundaries
- Program flash: firmware and embedded HTML.
- RAM: scan results, counters and system state; lost on restart.
- LittleFS: AP key, provided filesystem is initialized; no automatic formatting.
- Phone localStorage: optional diagnostic snapshots, not synced or uploaded.
- Trust boundary: private AP clients can access the HTTP API. There is no application login or TLS. No Internet routing or external telemetry is implemented.

## Extension principles
Keep platform-specific hardware behind narrow interfaces as features grow. Separate tool logic from HTTP handlers, validate every request, cap response sizes and allocations, and prefer cooperative state machines over blocking waits. Keep shared UI and data contracts portable to future ESP32 builds. New modules should include capability checks, error states, tests, docs and clear hardware prerequisites. This modular structure is a design goal, not yet fully implemented.
