# Local HTTP API (v0.2)

Base URL is normally http://192.168.4.1, accessible only via the device AP in its current configuration. All JSON routes use no-store caching. There is no external service or stable public API guarantee before v1.

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
