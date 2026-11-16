"""Fail-closed Apple notarization and Windows installer contracts."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
workflow=(root/".github/workflows/build.yml").read_text(encoding="utf-8")
pkg=(root/"Installer/macos/build_pkg.sh").read_text(encoding="utf-8")
sign=(root/"Installer/macos/prepare_signing.sh").read_text(encoding="utf-8")
win=(root/"Installer/windows/build_installer.ps1").read_text(encoding="utf-8")
iss=(root/"Installer/windows/PedalLegends.iss").read_text(encoding="utf-8")
cmake=(root/"CMakeLists.txt").read_text(encoding="utf-8")
ui=(root/"Source/PluginEditor.cpp").read_text(encoding="utf-8")
assert "project(PedalLegends VERSION 0.6.1" in cmake
assert "CMAKE_OSX_ARCHITECTURES=x86_64;arm64" in workflow
assert "CMAKE_OSX_DEPLOYMENT_TARGET=11.0" in workflow
assert "if-no-files-found: error" in workflow
assert "pkgbuild --root" in pkg
assert "pkgutil --payload-files" in pkg
assert "lipo -archs" in pkg
assert "/Library/Audio/Plug-Ins/Components" in pkg
assert "/Library/Audio/Plug-Ins/VST3" in pkg
assert "Developer ID Application" in sign and "Developer ID Installer" in sign
assert "APPLE_DEVELOPER_ID_APPLICATION_P12_BASE64" in sign
assert "APPLE_DEVELOPER_ID_INSTALLER_P12_BASE64" in sign
assert "APPLE_P12_PASSWORD" in sign
assert "security import" in sign
assert "codesign --force --deep --options runtime --timestamp" in pkg
assert "pkgbuild --root" in pkg and '--sign "$PEDAL_INSTALLER_IDENTITY"' in pkg
assert "notarytool submit" in pkg
assert "APPLE_NOTARY_APPLE_ID" in pkg
assert "APPLE_NOTARY_APP_PASSWORD" in pkg
assert "APPLE_NOTARY_TEAM_ID" in pkg
assert 'if [[ "$status" != "Accepted" ]]' in pkg
assert "stapler staple" in pkg and "stapler validate" in pkg
assert "spctl --assess --type install" in pkg
assert "SIGNED-NOTARIZED" in workflow
assert "TEST-UNSIGNED" in workflow
assert "github.ref == 'refs/heads/main'" in workflow
assert "github.event_name != 'pull_request'" in workflow
assert "ISCC.exe" in win and "choco install innosetup" in win
assert "{commoncf64}" in iss and "ArchitecturesAllowed=x64compatible" in iss
assert "setSize(1600,930)" in ui and "BROWSE  400" in ui
print("PASS: notarization blocks unsigned commercial builds and preserves UI")
