# Hardware compatibility

| Target | Status | Requirements |
| --- | --- | --- |
| ESP8266 NodeMCU ESP-12E/F, 4 MB | Development target; physical validation pending | USB data cable, compatible USB serial driver |
| Other ESP8266 boards | Not validated | Confirm flash layout, RAM and pinout |
| ESP32 / ESP32-S3 | Planned port | Separate firmware build and board validation |
| Android browser | Intended companion | Wi-Fi and local HTTP access |
| Native Android app | Planned | Future implementation |

No extra components are needed for the current Wi-Fi toolkit. The ESP8266 is a 2.4 GHz single-radio device. BLE, NFC, infrared, sub-GHz and 125 kHz RFID are not provided by current firmware. Do not mistake GPIO for a radio interface.

## Electrical safety
Use a reliable USB power supply. ESP8266 GPIO uses 3.3 V logic; do not connect 5 V signals directly. Disconnect power before wiring peripherals. Future hardware modules must document pin mapping, voltage, current, boot strap constraints and tested board revisions. Do not attach unknown electrical systems directly.

Verify actual chip and flash size before installing. Backup old firmware/data if needed. Filesystem initialization replaces existing filesystem contents. See README for instructions and troubleshooting.

Optional station mode uses the same 2.4 GHz radio as the AP; association or scanning may interrupt browser access. No additional hardware required; no simultaneous independent radio channels.

## Read-only GPIO prototype

The pinned NodeMCU variant maps D1 to GPIO5, D2 to GPIO4, D5 to GPIO14, D6
to GPIO12, and D7 to GPIO13. The electronics endpoint samples these five
inputs without changing pin modes or configuring pull-ups. Floating pin
levels have no diagnostic meaning. Confirm the labels on the actual board
and use only 3.3 V-compatible signals with a shared ground after confirming
the wiring. Hardware acceptance for these inputs is pending.
