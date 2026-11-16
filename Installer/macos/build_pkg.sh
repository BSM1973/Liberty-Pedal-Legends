#!/usr/bin/env bash
# PEDAL LEGENDS: universal AU/VST3, Developer ID signing and Apple notarization.
# Reuses the proven ECHO 86 certificate + Apple-ID secret names.
set -euo pipefail

VERSION="${1:-0.6.1}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
ARTIFACTS="$ROOT/build/PedalLegends_artefacts/Release"
STAGING="$ROOT/build/installer-stage/macos"
OUT="$ROOT/dist/installers"
AU="$ARTIFACTS/AU/PEDAL LEGENDS.component"
VST3="$ARTIFACTS/VST3/PEDAL LEGENDS.vst3"
SIGNED="${PEDAL_SIGNING_READY:-false}"

if [[ ! -d "$AU" || ! -d "$VST3" ]]; then
  echo "::error::AU/VST3 plugin bundles are missing."
  exit 1
fi
for bundle in "$AU" "$VST3"; do
  executable="$(find "$bundle/Contents/MacOS" -maxdepth 1 -type f -print -quit)"
  if [[ -z "$executable" ]]; then
    echo "::error::Missing Mach-O binary: $bundle"
    exit 1
  fi
  architectures="$(/usr/bin/lipo -archs "$executable")"
  echo "Verified architecture: $(basename "$bundle") = $architectures"
  for arch in arm64 x86_64; do
    if [[ " $architectures " != *" $arch "* ]]; then
      echo "::error::Required architecture $arch missing from $bundle"
      exit 1
    fi
  done

  if [[ "$SIGNED" == "true" ]]; then
    : "${PEDAL_APP_IDENTITY:?Developer ID Application identity missing}"
    /usr/bin/xattr -dr com.apple.quarantine "$bundle" 2>/dev/null || true
    /usr/bin/codesign --force --deep --options runtime --timestamp \
      --sign "$PEDAL_APP_IDENTITY" "$bundle"
    /usr/bin/codesign --verify --deep --strict --verbose=2 "$bundle"
    signature="$(/usr/bin/codesign --display --verbose=2 "$bundle" 2>&1)"
    if ! grep -Fq "Authority=Developer ID Application:" <<< "$signature"; then
      echo "::error::Developer ID Application signature missing from $bundle"
      exit 1
    fi
  fi
done

rm -rf "$STAGING" "$OUT"
mkdir -p "$STAGING/Library/Audio/Plug-Ins/Components" \
         "$STAGING/Library/Audio/Plug-Ins/VST3" "$OUT"
/usr/bin/ditto "$AU" "$STAGING/Library/Audio/Plug-Ins/Components/$(basename "$AU")"
/usr/bin/ditto "$VST3" "$STAGING/Library/Audio/Plug-Ins/VST3/$(basename "$VST3")"

if [[ "$SIGNED" == "true" ]]; then
  for bundle in "$STAGING/Library/Audio/Plug-Ins/Components/PEDAL LEGENDS.component" \
                "$STAGING/Library/Audio/Plug-Ins/VST3/PEDAL LEGENDS.vst3"; do
    /usr/bin/codesign --verify --deep --strict --verbose=2 "$bundle"
  done
  : "${PEDAL_INSTALLER_IDENTITY:?Developer ID Installer identity missing}"
  PACKAGE="$OUT/PEDAL-LEGENDS-macOS-Universal-AU-VST3-v${VERSION}-DeveloperID.pkg"
  /usr/bin/pkgbuild --root "$STAGING" \
    --identifier com.libertymusicsoftware.pedallegends.pkg \
    --version "$VERSION" --install-location / --ownership recommended \
    --sign "$PEDAL_INSTALLER_IDENTITY" "$PACKAGE"
  /usr/sbin/pkgutil --check-signature "$PACKAGE"
else
  PACKAGE="$OUT/PEDAL-LEGENDS-macOS-Universal-AU-VST3-v${VERSION}-TEST-unsigned.pkg"
  /usr/bin/pkgbuild --root "$STAGING" \
    --identifier com.libertymusicsoftware.pedallegends.pkg \
    --version "$VERSION" --install-location / --ownership recommended "$PACKAGE"
  echo "::warning::Unsigned PR/test package only: no Apple notarization."
fi

PAYLOAD="$(/usr/sbin/pkgutil --payload-files "$PACKAGE")"
for entry in "Library/Audio/Plug-Ins/Components/PEDAL LEGENDS.component" \
             "Library/Audio/Plug-Ins/VST3/PEDAL LEGENDS.vst3"; do
  if ! grep -Fq "$entry" <<< "$PAYLOAD"; then
    echo "::error::Missing package payload $entry"
    exit 1
  fi
done
test -s "$PACKAGE" || { echo "::error::Empty package"; exit 1; }

if [[ "$SIGNED" == "true" ]]; then
  # These are the SAME three notarization secrets already used for ECHO 86.
  for key in APPLE_NOTARY_APPLE_ID APPLE_NOTARY_APP_PASSWORD APPLE_NOTARY_TEAM_ID; do
    if [[ -z "${!key:-}" ]]; then
      echo "::error::Missing GitHub Actions secret: $key (copy your ECHO 86 configuration)"
      exit 1
    fi
  done

  result="$RUNNER_TEMP/pedal-legends-notary.json"
  echo "Submitting signed PEDAL LEGENDS .pkg to Apple's notary service..."
  /usr/bin/xcrun notarytool submit "$PACKAGE" \
    --apple-id "$APPLE_NOTARY_APPLE_ID" \
    --password "$APPLE_NOTARY_APP_PASSWORD" \
    --team-id "$APPLE_NOTARY_TEAM_ID" \
    --wait --output-format json > "$result"

  status="$(/usr/bin/python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("status","Unknown"))' "$result")"
  echo "Apple notarization status: $status"
  if [[ "$status" != "Accepted" ]]; then
    echo "::error::Notarization was not accepted by Apple; refusing commercial artifact."
    exit 1
  fi

  /usr/bin/xcrun stapler staple "$PACKAGE"
  /usr/bin/xcrun stapler validate "$PACKAGE"
  /usr/sbin/spctl --assess --type install --verbose=2 "$PACKAGE"
  /usr/sbin/pkgutil --check-signature "$PACKAGE"

  NOTARIZED="$OUT/PEDAL-LEGENDS-macOS-Universal-AU-VST3-v${VERSION}-SIGNED-NOTARIZED.pkg"
  mv "$PACKAGE" "$NOTARIZED"
  PACKAGE="$NOTARIZED"
  echo "PEDAL_NOTARIZED=true" >> "$GITHUB_ENV"
  echo "::notice::Apple notarization Accepted; stapled, signed AU/VST3 and installer validated."
fi

echo "Verified installer: $PACKAGE"
ls -lh "$PACKAGE"
