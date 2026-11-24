# Local HTTP API (v0.3)

Base URL is normally http://192.168.4.1, bound to the device AP address; station isolation requires hardware validation. All JSON routes use no-store caching. There is no external service or stable public API guarantee before v1.

| Method | Route | Description |
| --- | --- | --- |
| GET | / | Embedded HTML dashboard |
| GET | /api/status | Device telemetry |
| POST | /api/scan | Start asynchronous Wi-Fi scan |
| GET | /api/scan | Read scan status and cached results |

POST /api/scan requires the header X-Phantom-Request: scan. It is a browser cross-origin safeguard, not authentication. A 202 response contains state=scanning; 403 means missing/incorrect header, 429 means cooldown, 503 means low heap. Unknown routes return 404.

Status fields: version, board, cpuMHz, flashBytes, freeHeap, minHeap, maxHeap, scanCount, scanErrors, lastScanMs, uptimeSeconds, clients, ap, ip, persistentKey. Heap and flash values are bytes; durations are milliseconds unless named Seconds.

Scan fields: state (idle, scanning, ready, error), total, ageSeconds (nullable), channelCounts (14 positions, channels 1–14), counted, networks (array of ssid, rssi in dBm, channel, security). A failed or running scan can retain an earlier cache: clients must inspect state. No BSSID, packet content or passwords are returned. Values are observations, not proof of network security. Keep responses local unless the user deliberately exports them.

Use the dashboard or a local HTTP client on an owned device. Do not poll aggressively. Current dashboard retries for approximately 30 seconds; scan cooldown is enforced by firmware.

## Optional network toolkit
| Method | Route | Required X-Phantom-Request | Purpose |
| --- | --- | --- | --- |
| GET | /api/link | — | Station state and connected-only telemetry |
| POST | /api/link/join | join | URL-encoded ssid and password; explicit user action |
| POST | /api/link/leave | leave | Disconnect station |
| POST | /api/link/dns | dns | Resolve example.com, max 1.5-second SDK timeout |

Link states: disconnected, connecting, connected, timeout, lost, subnet_conflict. Connected fields: ssid, ip, gateway, subnet, dns, rssi, channel. DNS fields: hostname, resolved, elapsedMs, address (nullable). Join returns 202; leave and DNS return 200. Errors include 400 invalid parameters, 403 invalid header/host, 409 conflicting radio operation or disconnected, 429 cooldown, 503 low memory. All routes require destination/Host 192.168.4.1 (optional :80). No CORS. This is not authentication. No credentials are returned.
