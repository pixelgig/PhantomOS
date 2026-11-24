# v0.2.0 validation status

- Node.js dashboard smoke test: PASS. Embedded HTML matches preview; sample status, four network rows, 14 channel bars, CSV escaping, filtering/sorting, scan telemetry, snapshot schema and simulated browser storage round-trip.
- Firmware: source changes reviewed; PlatformIO is not installed in this environment. No firmware binary or successful compilation claimed.
- Hardware: physical board is unavailable; all radio, memory stability, persistence and real browser-to-device tests remain pending.
- CI: workflow included; not executed here.

## Deferred acceptance tests
- Compile for nodemcuv2 and inspect RAM and flash usage.
- Flash after confirming chip and capacity; check AP key persistence and mobile connectivity.
- Repeated scans, low-heap guard, timeout recovery and valid UTF-8 SSIDs.
- Monitor actual local HTTP latency, reconnect after scan, verify heap range and scan count; run at least ten scans.
- Verify snapshots, JSON/CSV downloads and clear behavior in the phone browser.
