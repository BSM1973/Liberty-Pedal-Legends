# PEDAL LEGENDS installers (v0.6.1)

The GitHub Actions workflow builds real, self-contained installers from the
compiled native plugin artifacts. No source code or host binaries are embedded
as placeholders.

## macOS: universal AU + VST3 PKG
The GitHub macos-15 builder compiles both x86_64 and arm64 in the same plugins.
`Installer/macos/build_pkg.sh` validates these two architectures before it
packages the bundles using `pkgbuild`.

System-wide install destinations:
- `/Library/Audio/Plug-Ins/Components/PEDAL LEGENDS.component` (AU)
- `/Library/Audio/Plug-Ins/VST3/PEDAL LEGENDS.vst3` (VST3)

With no Developer ID Installer identity in the runner keychain, the resulting
package is named `*-unsigned.pkg` and **is not notarized by Apple**.
Apple Developer ID Installer signing uses `productsign` if
`MACOS_INSTALLER_IDENTITY` is configured **and the certificate is present in
the runner keychain**. Signing and notarization for public distribution
require the developer's own Apple credentials; do not commit them to Git.

## Windows: VST3 x64 installer EXE
`Installer/windows/build_installer.ps1` uses Inno Setup. It copies the
complete VST3 bundle to
`C:\Program Files\Common Files\VST3\PEDAL LEGENDS.vst3`, registers an
uninstaller, and requires administrator privileges. The generated `.exe` is
**not Authenticode signed** without the publisher's signing certificate and
may produce a Windows SmartScreen warning.

## Downloads and validation
Navigate to the green GitHub Actions run and expand **Artifacts**:
- `PEDAL-LEGENDS-macos-15-INSTALLER` contains one `.pkg`
- `PEDAL-LEGENDS-windows-2022-INSTALLER` contains one `.exe`

GitHub downloads each artifact as a ZIP: extract the installer, then run the
`.pkg` or `.exe`. Also available: native format bundles for advanced users.

Both installers need host-level testing in Ableton Live and other DAWs.
Only after Apple signing/notarization and Windows code signing should these
packages be presented as fully trusted, commercial distribution builds.
