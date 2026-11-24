# Development validation

| Check | Result | Evidence |
| --- | --- | --- |
| v0.3.0 baseline CI | PASS | [Run 45349151947](https://github.com/pixelgig/PhantomOS/actions/runs/45349151947) at `3a2ce9fa1993bc1f7c1d61add845b4b0d71c5ad9`. Dashboard, firmware and LittleFS build. |
| Baseline dashboard locally | PASS | `node tests/dashboard.test.cjs` on the baseline checkout. |
| Development dashboard locally | PASS | Demo regression, embedded source match, mock live request error, body timeout and disabled controls. |
| Board and phone validation | NOT RUN | No physical device in the execution environment. |

Compiled flags describe code availability. See [physical acceptance](docs/TESTING.md) for board requirements. The default profile excludes station association and DNS code. The experimental profile still relies on the SDK HTTP parser and needs parser limits and isolation verification. Binary distribution is held pending the linked dependency license review.
