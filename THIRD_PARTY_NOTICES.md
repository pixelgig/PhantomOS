# Third-party software

The MIT license applies to original Phantom OS source and documentation. It does not relicense dependencies. The project builds against PlatformIO's espressif8266 platform 4.2.1 and its Arduino ESP8266 core, ESP8266 SDK, compiler/toolchain and LittleFS components. GitHub Actions and Node/Python are development tools. Their respective licenses apply.

Upstream references:
- https://github.com/platformio/platform-espressif8266
- https://github.com/esp8266/Arduino
- https://github.com/littlefs-project/littlefs
- https://github.com/platformio/platformio-core

Before distributing firmware binaries, audit the actual resolved packages and their licenses, include required notices and corresponding source or offers where required. This file is an initial dependency inventory, not a completed compliance audit. Do not claim that linked binaries are MIT-only.

## Reviewed package inventory

The resolved framework `3.30102.0` is Arduino ESP8266 3.1.2. That installed
package contains these upstream license files, relative to its root:

| Component | Package evidence |
| --- | --- |
| Arduino ESP8266 core and bundled libraries | `LICENSE`, plus headers and component notices |
| eboot | `bootloaders/eboot/LICENSE` |
| LittleFS | `libraries/LittleFS/lib/littlefs/LICENSE.md` |
| umm_malloc | `cores/esp8266/umm_malloc/LICENSE` |
| BearSSL | `tools/sdk/ssl/bearssl/LICENSE.txt` |
| uzlib | `tools/sdk/uzlib/LICENSE` |

Package presence does not establish what is linked. The final binary review
still needs a complete link-map inventory, Espressif SDK and toolchain runtime
terms, and the applicable notices and source materials for redistribution.
See the [upstream framework tree](https://github.com/esp8266/Arduino/tree/3.1.2).
