# Agent instructions and change log

Read BRAIN.md before work. Keep hardware facts separate from assumptions. Never claim compilation, deployment or physical testing unless actually performed. Preserve private AP credentials and avoid cloud dependencies. Update this file after changes with version, model, modifications and validation.

## 2026-11-24 — v0.1.0 — ChatGPT (GPT-6.2)
- Created original ESP8266 firmware, local access point, read-only wireless diagnostics, status endpoint, CSV export, responsive dashboard and offline sample preview.
- Added PlatformIO project with pinned platform and CI workflow, installation instructions, and dashboard smoke test.
- Build attempt: local environment lacked PlatformIO and pip could not fetch it. Firmware compilation and actual flash/run validation are pending. Dashboard test results recorded in VALIDATION.md.
- Next action: confirm actual chip/flash size, compile, flash using user's Windows PC, validate on real board.

## 2026-11-24 — v0.2.0 — ChatGPT (GPT-6.2)
- Added scan counters, duration and timeout reporting, sampled heap range, separate channel totals, valid UTF-8 preservation, and AP startup guard.
- Added Wi-Fi filters/sorting, JSON export, optional browser-local snapshots, and local HTTP response monitoring. No uplink, disruptive radio capabilities or additional hardware required.
- User prefers to defer physical testing until PC access is available. Firmware compile and hardware testing remain pending. See VALIDATION.md.

## 2026-11-24 — Repository migration — ChatGPT (GPT-6.2)
- Imported the complete v0.2.0 source into pixelgig/PhantomOS on main. Verified all 12 original Git blob hashes against the source archive.
- Updated README to identify the canonical repository. No firmware behavior changed. GitHub Actions build not yet verified; physical validation remains pending.

## 2026-11-24 — Public project preparation — ChatGPT (GPT-6.2)
- Added MIT license, public contribution and security policies, issue/PR templates, architecture, API, hardware, development, testing and release docs, roadmap, changelog and dashboard generator.
- Public source is for everyone; temporary BRAIN.md and AGENTS.md must be removed before stable v1.0 after migration into public docs. Git history is retained.
- Initial CI dashboard test passed; firmware compilation failed because the variadic header registration overload misinterpreted the array/count arguments. Changed to single-header overload; new CI result pending. Hardware remains untested.

## 2026-11-24 — v0.3.0 — ChatGPT (GPT-6.2)
- Verified v0.2 corrected GitHub build passed (45348786422).
- Added optional station association, disconnect, DHCP telemetry, DNS check with timeout/cooldown, AP-bound HTTP server and Host/destination guard, subnet conflict rejection, dashboard/demo and automated assertions. No intentional station credential persistence, NAT, ping or speed test.
- Updated public docs, roadmap, changelog, validation. v0.3 CI and physical tests pending. Next: hardware isolation/connection testing; v0.4 electronics toolkit.
