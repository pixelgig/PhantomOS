# Testing and acceptance

## Automated checks
Run node tests/dashboard.test.cjs, pio run -e nodemcuv2 and pio run -e nodemcuv2 -t buildfs. CI runs these commands and publishes build artifacts only if they pass. Compile success is not equivalent to hardware validation.

## Physical checklist
Record firmware commit, board marking, flash size, phone/browser, date, power source and results.
- Boot, serial startup and AP connection without Internet.
- Verify key persistence after restart with initialized LittleFS; verify temporary key behavior without it.
- Scan known owned APs, compare channel/security/RSSI, and confirm reconnect after scan.
- Repeat at least ten scans with cooldown; look for heap degradation or reset.
- Test scan requests during active scan, cooldown, failure and timeout handling.
- Verify unusual SSIDs display as text and CSV export protects against formulas.
- Check JSON/CSV exports, optional snapshots, clearing storage and foreground monitoring.
- Update firmware without reinitializing LittleFS and confirm settings preserved.
- Verify mobile layout and no requests to external services.

Report PASS, FAIL or NOT RUN for each. Never publish raw passwords, nearby SSIDs or private logs. Use VALIDATION.md for current evidence and attach redacted logs to issues.
