# Security policy

Phantom OS is an alpha prototype, not a hardened security appliance. Only the latest development branch is considered for fixes; there is no supported stable version yet.

## Reporting
Prefer the repository's GitHub Security > Report a vulnerability option if private vulnerability reporting is enabled. If unavailable, ask the maintainer in a public issue to establish a private channel without revealing exploit details, credentials or personal information. Include affected commit, board, impact, minimal reproduction on an authorized device and suggested mitigation. Do not test third-party networks without permission. No bug bounty or response-time guarantee is offered.

## Security model
The device currently relies on its private WPA2 access point; HTTP has no independent authentication or TLS. Do not expose the dashboard to the Internet or an untrusted network. Keep serial output and AP keys private. Initialize LittleFS only after backing up old data. Browser snapshots may contain nearby network names; redact reports before sharing. See docs/ARCHITECTURE.md for boundaries.

## v0.3 station mode
Upstream association is disabled in the default build. Experimental firmware requires controlled hardware isolation testing before any broader use. Server binds to AP address and validates Host/destination; physical isolation tests are outstanding. Do not join untrusted upstream networks. Station credentials are sent via local HTTP inside WPA2 and used only for the current session; no application persistence. No independent app authentication or TLS. DNS is explicitly user-triggered and may reach the upstream resolver.
