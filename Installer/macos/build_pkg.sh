#!/usr/bin/env bash
# PEDAL LEGENDS - reproducible macOS AU/VST3 system-wide installer.
set -euo pipefail

VERSION="${1:-0.6.1}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
ARTIFACTS="${ROOT}/build/PedalLegends_artefacts/Release"
STAGING="${ROOT}/build/installer-stage/macos"
OUT="${ROOT}/dist/installers"
AU="${ARTIFACTS}/AU/PEDAL LEGENDS.component"
VST3="${ARTIFACTS}/VST3/PEDAL LEGENDS.vst3"

if [[ ! -d "$AU" || ! -d "$VST3" ]]; then
  echo "::error::Missing AU/VST3 plugin bundles in $ARTIFACTS"
  find "$ARTIFACTS" -maxdepth 3 -type d -print || true
  exit 1
fi

# Guarantee one installer works natively both on Apple Silicon and Intel.
for bundle in "$AU" "$VST3"; do
  executable="$(find "$bundle/Contents/MacOS" -maxdepth 1 -type f -print -quit)"
  if [[ -z "$executable" ]]; then
    echo "::error::No Mach-O executable in $bundle"
    exit 1
  fi
  architectures="$(/usr/bin/lipo -archs "$executable")"
  echo "$bundle : $architectures"
  for arch in arm64 x86_64; do
    if [[ " $architectures " != *" $arch "* ]]; then
      echo "::error::Required architecture $arch missing from $bundle"
      exit 1
    fi
  done
done

rm -rf "$STAGING"
mkdir -p "$STAGING/Library/Audio/Plug-Ins/Components" \
         "$STAGING/Library/Audio/Plug-Ins/VST3" "$OUT"
/usr/bin/ditto "$AU" "$STAGING/Library/Audio/Plug-Ins/Components/$(basename "$AU")"
/usr/bin/ditto "$VST3" "$STAGING/Library/Audio/Plug-Ins/VST3/$(basename "$VST3")"

UNSIGNED="$OUT/PEDAL-LEGENDS-macOS-Universal-AU-VST3-v${VERSION}-unsigned.pkg"
SIGNED="$OUT/PEDAL-LEGENDS-macOS-Universal-AU-VST3-v${VERSION}.pkg"
/usr/bin/pkgbuild --root "$STAGING" \
  --identifier "com.libertymusicsoftware.pedallegends.pkg" \
  --version "$VERSION" --install-location / \
  --ownership recommended "$UNSIGNED"

# Optional Developer ID Installer signing when a valid identity has already
# been imported into the CI macOS keychain. No certificates are stored in git.
if [[ -n "${MACOS_INSTALLER_IDENTITY:-}" ]]; then
  /usr/bin/productsign --sign "$MACOS_INSTALLER_IDENTITY" "$UNSIGNED" "$SIGNED"
  rm "$UNSIGNED"
  PACKAGE="$SIGNED"
  /usr/sbin/pkgutil --check-signature "$PACKAGE"
else
  PACKAGE="$UNSIGNED"
  echo "::warning::Installer unsigned/not notarized. Configure Apple Developer ID Installer signing for public distribution."
fi

PAYLOAD="$(/usr/sbin/pkgutil --payload-files "$PACKAGE")"
if ! grep -Fq 'Library/Audio/Plug-Ins/Components/PEDAL LEGENDS.component' <<< "$PAYLOAD"; then
  echo "::error::AU missing from PKG payload"
  exit 1
fi
if ! grep -Fq 'Library/Audio/Plug-Ins/VST3/PEDAL LEGENDS.vst3' <<< "$PAYLOAD"; then
  echo "::error::VST3 missing from PKG payload"
  exit 1
fi
[[ -s "$PACKAGE" ]] || { echo "::error::Empty macOS package"; exit 1; }
echo "Created and checked: $PACKAGE"
ls -lh "$PACKAGE"
