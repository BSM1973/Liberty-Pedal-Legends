#!/usr/bin/env bash
# Import the SAME Apple Developer ID certificates as ECHO 86.
# Invoked only on the trusted main branch; never prints credentials.
set -euo pipefail

require_secret() {
  local var="$1"
  if [[ -z "${!var:-}" ]]; then
    echo "::error::Missing GitHub Actions secret: $var (reuse the value from your ECHO 86 setup)"
    return 1
  fi
}
missing=0
for key in APPLE_DEVELOPER_ID_APPLICATION_P12_BASE64 \
           APPLE_DEVELOPER_ID_INSTALLER_P12_BASE64 \
           APPLE_P12_PASSWORD; do
  require_secret "$key" || missing=1
done
if [[ "$missing" -ne 0 ]]; then
  echo "::error::Apple signing cannot start; no unsigned installer will be published as notarized."
  exit 1
fi

keychain="$RUNNER_TEMP/pedal-legends-developer-id.keychain-db"
password="$(/usr/bin/uuidgen)"
/usr/bin/security create-keychain -p "$password" "$keychain"
/usr/bin/security set-keychain-settings -lut 21600 "$keychain"
/usr/bin/security unlock-keychain -p "$password" "$keychain"
/usr/bin/security list-keychains -d user -s "$keychain"
/usr/bin/security default-keychain -s "$keychain"

appP12="$RUNNER_TEMP/pedal-legends-application.p12"
installerP12="$RUNNER_TEMP/pedal-legends-installer.p12"
trap 'rm -f "$appP12" "$installerP12"' EXIT
printf '%s' "$APPLE_DEVELOPER_ID_APPLICATION_P12_BASE64" | /usr/bin/base64 -D > "$appP12"
printf '%s' "$APPLE_DEVELOPER_ID_INSTALLER_P12_BASE64" | /usr/bin/base64 -D > "$installerP12"
/usr/bin/security import "$appP12" -k "$keychain" -P "$APPLE_P12_PASSWORD" -T /usr/bin/codesign
/usr/bin/security import "$installerP12" -k "$keychain" -P "$APPLE_P12_PASSWORD" -T /usr/bin/pkgbuild -T /usr/bin/productsign
/usr/bin/security set-key-partition-list -S apple-tool:,apple: -s -k "$password" "$keychain"

app_identity="$(/usr/bin/security find-identity -v -p codesigning "$keychain" | /usr/bin/awk -F '"' '/Developer ID Application/{print $2; exit}')"
installer_identity="$(/usr/bin/security find-identity -v "$keychain" | /usr/bin/awk -F '"' '/Developer ID Installer/{print $2; exit}')"
if [[ -z "$app_identity" || -z "$installer_identity" ]]; then
  echo "::error::Developer ID Application and Developer ID Installer identities were not both imported."
  exit 1
fi

echo "PEDAL_SIGNING_READY=true" >> "$GITHUB_ENV"
echo "PEDAL_APP_IDENTITY=$app_identity" >> "$GITHUB_ENV"
echo "PEDAL_INSTALLER_IDENTITY=$installer_identity" >> "$GITHUB_ENV"
echo "::notice::Apple Developer ID identities imported; packaging must now notarize and staple."
