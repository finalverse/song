# Finalverse Song Studio alpha release

The first public target is `v5.0.0-alpha`. It is a prerelease for testing the Finalverse identity, packaging, compatibility, and read-only AI foundations—not a stable production release.

## Release workflow

1. Merge release-readiness changes into `main`.
2. Run **Build: All** in `testing` mode with publishing and backend deployment disabled.
3. Confirm the Linux, macOS, and Windows artifacts install or launch and identify as Finalverse Song Studio.
4. Run **Deploy** with `release_type=alpha`, `create_tag=true`, and only the verified desktop platforms.
5. Inspect the generated draft GitHub prerelease, filenames, checksums, signatures, license notices, and release notes.
6. Publish the GitHub prerelease only after the checklist below passes.

The deploy workflow never uploads Song artifacts to inherited MuseScore services and never deploys the inherited backend. It creates a draft GitHub prerelease so human verification remains the final gate.

## Required verification

- Linux x64 and arm64 AppImages launch and retain native score compatibility.
- The macOS DMG launches on Apple silicon and Intel Macs.
- The Windows MSI installs, upgrades, launches, and uninstalls with the Finalverse-owned upgrade GUID.
- App names, bundle identifiers, installer metadata, filenames, About text, icons, and generated document creator metadata identify Finalverse Song Studio.
- Checksums in `checksums.sha256.txt` match every uploaded artifact.
- Provider credentials, signing credentials, user scores, and private endpoints do not appear in source, artifacts, or logs.
- The AI module remains read-only, provider-neutral, and optional.
- GPLv3, inherited copyright, and third-party notices are present.

## Signing status

The build workflows can produce unsigned alpha artifacts when Finalverse signing credentials are not configured. An unsigned artifact may remain attached to a draft for internal testing, but it must not be presented as a verified public build.

Before publishing publicly, configure organization-owned credentials without placing values in source or command output:

- macOS: encrypted Developer ID certificate, certificate password, signing identity, Apple account app-specific password, and Apple Team ID;
- Windows: Finalverse-owned code-signing service credentials;
- GitHub: the workflow-provided token only—no personal access token embedded in a remote, script, or log.

## Known alpha limitations

- The Assistant has a provider-neutral runtime and bounded score context, but no cloud provider or user-facing Assistant panel yet.
- AI cannot mutate a score.
- Collaboration, Finalverse accounts, publishing services, and automatic updates are disabled or not yet implemented.
- Windows PortableApps packaging still contains inherited compatibility material and is excluded from the first alpha release.
