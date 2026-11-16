"""Installer CI contract checks (portable; native installers validated in CI)."""
from pathlib import Path

root = Path(__file__).resolve().parents[1]
workflow = (root / ".github/workflows/build.yml").read_text(encoding="utf-8")
cmake = (root / "CMakeLists.txt").read_text(encoding="utf-8")
mac = (root / "Installer/macos/build_pkg.sh").read_text(encoding="utf-8")
windows = (root / "Installer/windows/build_installer.ps1").read_text(encoding="utf-8")
iss = (root / "Installer/windows/PedalLegends.iss").read_text(encoding="utf-8")
editor = (root / "Source/PluginEditor.cpp").read_text(encoding="utf-8")

assert "project(PedalLegends VERSION 0.6.1" in cmake
assert 'CMAKE_OSX_ARCHITECTURES=x86_64;arm64' in workflow
assert "-DCMAKE_OSX_DEPLOYMENT_TARGET=11.0" in workflow
assert "PEDAL-LEGENDS-${{ matrix.os }}-INSTALLER" in workflow
assert "if-no-files-found: error" in workflow
assert "pkgbuild --root" in mac
assert "/Library/Audio/Plug-Ins/Components" in mac
assert "/Library/Audio/Plug-Ins/VST3" in mac
assert "lipo -archs" in mac
assert "productsign --sign" in mac
assert "pkgutil --payload-files" in mac
assert "ISCC.exe" in windows
assert "choco install innosetup" in windows
assert "VST3" in iss and "recursesubdirs" in iss
assert "{commoncf64}" in iss and "ArchitecturesAllowed=x64compatible" in iss
assert "setSize(1600,930)" in editor
assert "BROWSE  400" in editor
print("PASS: reproducible PKG/EXE packaging, universal Mac, VST3 paths, unchanged UI")
