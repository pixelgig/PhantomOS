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

## v0.3 network checks (physical)
- Confirm standalone default and no unsolicited DNS or upstream association after reboot.
- Join known WPA2 network; verify DHCP address, gateway, DNS and signal against router; repeat with wrong password and confirm timeout.
- Verify phone reconnects after channel changes; station disconnection and reboot reset state.
- Verify dashboard is not reachable at station IP; test routed access to AP IP from station side before claiming isolation. Verify wrong Host and missing command header rejected.
- Verify overlapping subnet disconnects and leaves AP reachable. Verify scan/join conflict, DNS cooldown, DNS failure and cached result behavior.
- Check no station password in responses, LittleFS or serial logs; inspect persistence after reboot. Test heap with repeated joins/scans.
