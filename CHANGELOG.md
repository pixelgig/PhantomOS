# Changelog

## Unreleased — standalone hardening

- Compile station association and DNS out of the default profile and report feature availability through `/api/capabilities`.
- Bound scan serialization, clear results on scan start, reject failed starts, and discard results from expired scans.
- Add dashboard deadlines and mock live-response coverage.
- Add a read-only GPIO prototype for five NodeMCU inputs.
- Limit foreground monitoring to 15-second samples and coalesce overlapping status refreshes.
- Pin CI actions and enforce flash, RAM and dashboard size limits.
- Stage and read back AP key changes before replacing the active key.
- Hold CI binary uploads while linked dependency distribution obligations are verified.


Changes are recorded by version. Versions below 1.0 are development releases; interfaces may change.

## 0.3.0 — 2026-11-24
- Optional station join/disconnect, DHCP network details and on-demand DNS resolution.
- AP-address binding and Host/destination checks, subnet overlap rejection, scan/join guard.
- Network dashboard and demo tests, network toolkit documentation. No routing or ping.
- Corrected v0.2 CI passed; v0.3 build pending at commit time.

## Unreleased
- Public contribution, architecture, API, testing, hardware, security and release documentation.
- MIT license for original project files; dependency licenses remain separate.
- Correct ESP8266WebServer header registration for the pinned ESP8266 core (CI verification pending at time of change).

## 0.2.0 — 2026-11-24
- Scan duration, error counters, heap sampling, channel totals and UTF-8 handling.
- Dashboard filtering, sorting, optional local snapshots, JSON export and response-time monitor.
- Dashboard tests passed in GitHub Actions; initial firmware compilation failed at header registration. Physical validation pending.

## 0.1.0 — 2026-11-24
- Initial firmware, local access point, Wi-Fi discovery, device status and CSV export.
- Embedded dashboard, offline preview, PlatformIO setup and build workflow.
