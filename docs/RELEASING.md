# Release policy

Use semantic versioning. Until v1.0, mark builds alpha or beta and distinguish source, compiled artifacts and board-validated releases.

## Release checklist
1. Update firmware version, UI version, documentation and CHANGELOG.md consistently.
2. Review license provenance and third-party dependencies, including obligations for distributed binaries.
3. Require successful dashboard tests, firmware compilation and filesystem image build.
4. Record flash/RAM usage and physical test evidence for every claimed board.
5. Verify fresh install, upgrade, recovery instructions and privacy defaults.
6. Tag the exact tested commit; publish binaries with SHA-256 checksums, target board, flash layout and installation warnings. Do not advertise an installer until verified.
7. Before the first stable release, migrate any remaining useful content from BRAIN.md and AGENTS.md into permanent public docs/changelog, remove both temporary files from the active tree and remove references to them. Git history remains intact; deleting files does not erase previous commits.
8. Ensure the README accurately represents supported and planned features.

No stable release until the maintainer approves the physical acceptance results. GitHub Actions artifacts are development builds, not certified releases.
