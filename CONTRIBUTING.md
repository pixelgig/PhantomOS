# Contributing to Phantom OS

Everyone is welcome to fork, modify, use and contribute to this project. No paid tools or hardware purchases are required to contribute to documentation or the dashboard. Contributions to the original project are under the MIT license. Only submit material you have the right to license; disclose third-party code and its license.

## Development workflow
1. Search existing issues, then open an issue for large changes or new hardware targets.
2. Fork the repository and create a descriptive branch from main.
3. Read [architecture](docs/ARCHITECTURE.md), [API](docs/API.md), [development](docs/DEVELOPMENT.md), and [testing](docs/TESTING.md).
4. Keep changes focused; update documentation, tests and CHANGELOG.md. If altering the embedded dashboard, keep the preview and header synchronized.
5. Run the dashboard test and PlatformIO build. Record board model, flash size and test results if hardware is available. Clearly mark tests not performed.
6. Open a pull request describing motivation, implementation, limitations, memory impact and test evidence. Do not submit passwords, private SSIDs, logs containing personal information, or firmware dumps.

## Review criteria
- Local-first operation, no mandatory account, cloud or paid service.
- Preserve supported hardware and existing settings or document migration.
- Bound memory allocations, retries, request sizes and background activity; avoid blocking the watchdog.
- Use plain text DOM rendering for untrusted data, validate input, minimize permissions, and avoid leaking secrets.
- Diagnostics and electronics experiments must be designed for owned or authorized systems. No credential collection or disruptive features in upstream.
- Keep the mobile UI usable without external assets.
- Do not claim hardware support or stable releases without evidence.

AI-assisted contributions are welcome; humans remain responsible for reviewing, licensing and validating their changes. During development, also update AGENTS.md; it is a temporary coordination log, not a permanent requirement.
