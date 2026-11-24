# Phantom OS — ESP8266 edition v0.3.0

An open-source, community-driven, original local-first Wi-Fi diagnostic toolkit for the NodeMCU ESP8266 (ESP-12E/F, typically 4 MB flash). This is an **alpha development release**, not a hardware-validated firmware. It is not Bruce, and does not emulate missing radio hardware.

## What is included
- Embedded responsive dashboard, served from program flash, no Internet required after installation.
- WPA2-protected local access point, randomly generated 16-character key shown on USB serial. LittleFS stores the key if initialized.
- On-demand 2.4 GHz Wi-Fi discovery, signal strength in dBm, advertised security, channel distribution, strongest 32 visible access points, local CSV export.
- Hardware status (CPU, flash, heap, uptime, clients).
- No persistent router credentials, traffic capture, disruptive radio functions, remote server, analytics, or external dependencies at runtime. Network names remain on the board/phone unless the user exports them.
- Browser-only optional snapshots (up to 10), JSON export, filtering and sorting, scan success/error counters, sampled heap range, and opt-in five-second local HTTP response monitoring. Optional upstream connectivity diagnostics are now included.
- A separate offline browser preview with labeled example data, and a GitHub Actions build workflow.

## Before flashing
1. Check that the chip/module marking is ESP8266 / ESP-12E or ESP-12F and that flash capacity is 4 MB. This project targets PlatformIO `nodemcuv2`; do not flash onto an ESP32 or a different flash layout. If unsure, use esptool's read-only chip identification / flash ID first.
2. Flashing replaces existing firmware. Back up anything important first. Filesystem upload **overwrites the existing flash filesystem**; only do this on a board you own and have backed up or no longer need the old data on.
3. Use a USB **data** cable and a reliable USB port. No external components or GPIO wiring are needed. Use a suitable USB serial driver only if Windows fails to detect the board. CP210x and CH340 are common, but verify the actual chip.

## Build and install on Windows
Install Python 3 if missing. Extract the project ZIP, then open PowerShell in the folder containing `platformio.ini`. The initial build needs Internet to obtain PlatformIO, compiler, and SDK.

```powershell
py -m pip install platformio==6.1.18
py -m platformio device list
py -m platformio run -e nodemcuv2
```

After confirming the board and COM port, replace `COM5` below with its actual port. Upload firmware. For persistent access point credentials, also initialize the bundled LittleFS filesystem **once**, if old filesystem contents can be overwritten.

```powershell
py -m platformio run -e nodemcuv2 -t upload --upload-port COM5
py -m platformio run -e nodemcuv2 -t uploadfs --upload-port COM5
py -m platformio device monitor --port COM5 --baud 115200
```

Press the board's RST button after opening serial monitor. The board prints its network name, password and dashboard URL. Keep the password private. If LittleFS cannot mount (e.g. you skipped initialization), firmware uses a new temporary password every restart. No automatic formatting is done. For subsequent updates, upload firmware only to retain the key. Re-uploading the filesystem resets it.

On Android, connect to `Phantom-...`, enter the printed password, choose to stay connected even if Android warns that the network has no Internet, and open **http://192.168.4.1** in the browser. If the serial monitor shows a different IP, use that instead. USB power may come from a charger or suitable power bank after flashing; USB OTG is not required.

## Expected limits
- ESP8266 is single-radio and cannot scan other channels while continuously serving its access point on its home channel. Scan may briefly interrupt the dashboard connection. The dashboard retries results for 30 seconds. Reconnect manually if needed. Cooldown: 15 seconds between scans.
- Visible networks only. The 32 strongest reported entries are displayed. Channel counts cover up to 255 SDK entries, independently of the strongest-32 display cap; they are not RF spectrum measurements. Security labels are advertised by access points, not proof of security. Valid UTF-8 SSIDs are preserved; invalid bytes are escaped individually.
- Browser communicates over HTTP on the private WPA2 AP; there is no independent application login or TLS. Only share the key with trusted devices. No router uplink is configured by default; optional user-initiated station mode is available. Two clients maximum. This is a development prototype, not a security appliance.
- BLE/NFC/IR/Sub-GHz/125 kHz RFID/USB HID are not implemented. The ESP8266 does not supply those physical interfaces by itself.

## Validation checklist (physical device, not yet performed)
- Build completes and reports flash/RAM use; firmware and filesystem images exist.
- Serial shows correct AP name/key/IP; phone joins and page loads without Internet.
- Scan completes repeatedly with known router shown; channel and RSSI plausible; page reconnects if interrupted.
- CSV contains expected rows; strange SSIDs are treated as text, not markup or spreadsheet formulas.
- Restart after filesystem initialization preserves password; no external network joined.
- Run 10 scans with cooldown; confirm heap does not progressively fall or device reset.

## GitHub build
The canonical project repository is https://github.com/pixelgig/PhantomOS (main). The Actions workflow is configured to run the dashboard smoke test and produce firmware and LittleFS binaries. The corrected v0.2 build passed; v0.3 build and hardware status are tracked in VALIDATION.md.

## References
- https://docs.platformio.org/en/latest/boards/espressif8266/nodemcuv2.html
- https://arduino-esp8266.readthedocs.io/en/stable/esp8266wifi/scan-class.html
- https://arduino-esp8266.readthedocs.io/en/stable/filesystem.html

## Upgrade from v0.1
Upload firmware only if LittleFS is already initialized; no filesystem migration is required. Do not use uploadfs for routine updates. Browser snapshots are stored on the phone, not in LittleFS, and may be shared by different devices using the same dashboard origin. Clear snapshots before handing over the phone or board. Monitoring is foreground-only; Android may throttle background tabs. Scan timeout reports an error but does not cancel the SDK operation; if it never completes, reboot the board.

## Open-source community

Anyone can use, fork, modify and redistribute the original project under the [MIT license](LICENSE). Dependencies have their own terms; see [third-party notices](THIRD_PARTY_NOTICES.md). Contributions, bug reports and documentation improvements are welcome.

- [Contributing](CONTRIBUTING.md) · [Code of conduct](CODE_OF_CONDUCT.md) · [Security](SECURITY.md)
- [Architecture](docs/ARCHITECTURE.md) · [API reference](docs/API.md) · [Developer setup](docs/DEVELOPMENT.md)
- [Hardware compatibility](docs/HARDWARE.md) · [Testing](docs/TESTING.md) · [Release policy](docs/RELEASING.md)
- [Roadmap](ROADMAP.md) · [Changelog](CHANGELOG.md) · [CI results](https://github.com/pixelgig/PhantomOS/actions)

This project is independent of Bruce and Flipper Devices. Temporary development coordination files will be retired before the first stable release; permanent public documentation is the source of truth.

## Network toolkit (v0.3)
Join the Phantom AP, enter your own 2.4 GHz network SSID and WPA passphrase (or blank for open), and select Connect. The board joins as a station without NAT or routing. It does not save station credentials to LittleFS or deliberately write them to flash; the SDK keeps them in RAM. The local dashboard uses HTTP inside the private WPA2 AP, not TLS; use only trusted clients. Disconnect or reboot to end the station session. The AP may change channel, so reconnect your phone if necessary. If the upstream subnet overlaps 192.168.4.0/24, the board disconnects. Check DNS resolves example.com on demand using the configured resolver (may be cached). Gateway address is DHCP information, not a reachability test. No ICMP ping or Internet speed test is implemented yet. See docs/NETWORK_TOOLKIT.md.
