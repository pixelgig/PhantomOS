# Local HTTP API (v0.3)

Base URL is normally http://192.168.4.1, bound to the device AP address; station isolation requires hardware validation. All JSON routes use no-store caching. There is no external service or stable public API guarantee before v1.

| Method | Route | Description |
| --- | --- | --- |
| GET | / | Embedded HTML dashboard |
| GET | /api/status | Device telemetry |
| GET | /api/capabilities | Schema version and compiled feature flags |
| POST | /api/scan | Start asynchronous Wi-Fi scan |
| GET | /api/scan | Read scan status and cached results |

POST /api/scan requires the header X-Phantom-Request: scan. It is a browser cross-origin safeguard, not authentication. A 202 response contains state=scanning; 403 means missing/incorrect header, 429 means cooldown, 503 means low heap. Unknown routes return 404.

Status fields: version, board, cpuMHz, flashBytes, freeHeap, largestFreeBlock, minHeap, maxHeap, scanCount, scanErrors, lastScanMs, uptimeSeconds, clients, ap, ip, persistentKey. Heap and flash values are bytes; durations are milliseconds unless named Seconds.

Scan fields: state (idle, scanning, ready, error), total, ageSeconds (nullable), channelCounts (14 positions, channels 1–14), counted, networks (array of ssid, rssi in dBm, channel, security). A new scan clears the previous cache. Use results only when state is ready. The pinned SDK may report a radio completion failure as zero results. The networks JSON array is capped at 8 KiB and 32 entries. No BSSID, packet content or passwords are returned. Values are observations, not proof of network security. Keep responses local unless the user deliberately exports them.

Use the dashboard or a local HTTP client on an owned device. Do not poll aggressively. Dashboard requests time out after four seconds; scan polling uses a monotonic deadline; scan cooldown is enforced by firmware.

Capability schema 1 reports `wifiScan`, `gpioRead` and `experimentalStation` as booleans. Feature flags indicate what was compiled; board validation is tracked separately in VALIDATION.md.

## Experimental network toolkit

The default build returns `state: "disabled"` for link status and HTTP 403 for link mutations. The following behavior applies to experimental builds only.
| Method | Route | Required X-Phantom-Request | Purpose |
| --- | --- | --- | --- |
| GET | /api/link | — | Station state and connected-only telemetry |
| POST | /api/link/join | join | URL-encoded ssid and password; explicit user action |
| POST | /api/link/leave | leave | Disconnect station |
| POST | /api/link/dns | dns | Resolve example.com, max 1.5-second SDK timeout |

Link states: disconnected, connecting, connected, timeout, lost, subnet_conflict. Connected fields: ssid, ip, gateway, subnet, dns, rssi, channel. DNS fields: hostname, resolved, elapsedMs, address (nullable). Join returns 202; leave and DNS return 200. Errors include 400 invalid parameters, 403 invalid header/host, 409 conflicting radio operation or disconnected, 429 cooldown, 503 low memory. All routes require destination/Host 192.168.4.1 (optional :80). No CORS. This is not authentication. No credentials are returned.

## Read-only electronics prototype

`GET /api/electronics/status` samples the NodeMCU profile's five allowed
digital input levels. The response contains `schema: 1` and a `pins` array of
`label`, `gpio` and `level` (0 or 1). Use only when `gpioRead` is true in
capabilities. Physical validation is pending.
