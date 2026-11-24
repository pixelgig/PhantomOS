# Development validation

| Check | Result | Evidence |
| --- | --- | --- |
| v0.3.0 baseline CI | PASS | [Run 45349151947](https://github.com/pixelgig/PhantomOS/actions/runs/45349151947) at `3a2ce9fa1993bc1f7c1d61add845b4b0d71c5ad9`. Dashboard, firmware and LittleFS build. |
| Baseline dashboard locally | PASS | `node tests/dashboard.test.cjs` on the baseline checkout. |
| Development dashboard locally | PASS | Demo regression, embedded source match, mock live request error, body timeout and disabled controls. |
| Board and phone validation | NOT RUN | No physical device in the execution environment. |

Compiled flags describe code availability. See [physical acceptance](docs/TESTING.md) for board requirements. The default profile excludes station association and DNS code. The experimental profile still relies on the SDK HTTP parser and needs parser limits and isolation verification. Binary distribution is held pending the linked dependency license review.

## First hardening build

[GitHub Actions 45352712861](https://github.com/pixelgig/PhantomOS/actions/runs/45352712861)
compiled commit `74ffbaac1f0697556a0c91018ae64e945844ccc5` and built LittleFS.
Static RAM was 29,848 / 81,920 bytes. Sketch flash usage was 359,103 /
1,044,464 bytes. These figures are build-time reports. Runtime heap
stability and largest free block still need measurements on a board.

| File | Bytes | CI-reported SHA-256 |
| --- | ---: | --- |
| firmware.bin | 363248 | `95437e430df0d035866b3aec87f3a75aaca287b1d115a5a2f902e293f3226bf0` |
| littlefs.bin | 1024000 | `2b04def663faa8485f82c563738df13bc3e6a2d9da58844d46e3b0db33284871` |

The NodeMCU profile selects `eagle.flash.4m1m.ld`. The resolved linker
declares `_FS_start=0x40500000` and `_FS_end=0x405FA000`. The pinned
PlatformIO builder translates these addresses into an image interval
`[0x300000,0x3FA000)` and uses `0x0` as the firmware upload offset.
These are build-profile values. Actual hardware flash identification,
backup, install, firmware-only upgrade and restore are still required.
Use the pinned PlatformIO targets when performing physical validation.

## Continuation

The read-only GPIO preview and foreground monitor pass Node VM tests,
including a stalled mock HTTP response body and overlapping status refreshes.
Firmware pin sampling, scan stability, AP key recovery after power loss,
and phone reconnect behavior remain physical tests. The review branch adds
compile-time resource gates and hashes images in CI without publishing them.

Remaining release work includes hardware validation, HTTP input limits,
completed linked dependency compliance review, configuration migration,
the native Android companion, an installer and stable-release signoff.


## Electronics and CI budget checkpoint

[GitHub Actions 45353260720](https://github.com/pixelgig/PhantomOS/actions/runs/45353260720)
passed for commit `b81aae36f0423a08be0f45ea9a8371c09305cfe6`. It compiled
the read-only GPIO prototype and reported 29,928 / 81,920 bytes of static RAM
and 360,835 / 1,044,464 bytes of sketch flash. CI passed the configured
budgets. The board has yet to be tested.

The LittleFS hash changed between the two CI runs although the `data/`
sources were unchanged. Treat hashes as identifying individual builds
until byte-for-byte reproducibility is verified.
