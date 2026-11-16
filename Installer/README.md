# PEDAL LEGENDS — installation macOS et Windows

Le workflow GitHub Actions crée un installateur universel macOS AU + VST3 et
un programme d'installation Windows x64 VST3. Le moteur DSP, le pédalier
1600 × 930, les 80 modèles et les 400 presets ne sont pas modifiés.

## Notarisation Apple — exactement comme ECHO 86

Le dépôt ECHO 86 `BSM1973/Liberty-ECHO-86` utilise six secrets GitHub
Actions. Utiliser **les mêmes noms** dans
`BSM1973/Liberty-Pedal-Legends` (pas besoin de nouveaux certificats) :

- `APPLE_DEVELOPER_ID_APPLICATION_P12_BASE64`
- `APPLE_DEVELOPER_ID_INSTALLER_P12_BASE64`
- `APPLE_P12_PASSWORD`
- `APPLE_NOTARY_APPLE_ID`
- `APPLE_NOTARY_APP_PASSWORD`
- `APPLE_NOTARY_TEAM_ID`

Les secrets définis au niveau du dépôt ECHO 86 ne sont **pas** automatiquement
accessibles depuis PEDAL LEGENDS. S'ils n'ont pas été configurés au niveau
de l'organisation, les ajouter dans GitHub →
`Liberty-Pedal-Legends` → Settings → Secrets and variables → Actions.
Utiliser les valeurs sauvegardées hors de GitHub : les secrets existants
ne peuvent pas être relus en clair sur GitHub. Ne jamais copier les
certificats ni mots de passe dans un dépôt ou une conversation.

Sur `main`, Actions refuse de produire un artefact commercial si l'un
des six secrets manque. Quand ils sont présents :
1. Les certificats Developer ID Application et Developer ID Installer sont
   importés dans un trousseau temporaire du runner macOS.
2. Les bundles universels AU/VST3 sont signés avec Hardened Runtime,
   un timestamp Apple et Developer ID Application.
3. Le `.pkg` est signé avec Developer ID Installer.
4. `xcrun notarytool submit --wait` attend la décision Apple.
5. Après `Accepted` seulement, `xcrun stapler staple`,
   `stapler validate`, `spctl --assess` et `pkgutil --check-signature`
   vérifient le paquet.

Le résultat commercial s'appelle
`PEDAL-LEGENDS-macOS-SIGNED-NOTARIZED` et contient le `.pkg`.
En pull request (non fiable), les secrets ne sont jamais utilisés : un
`PEDAL-LEGENDS-macOS-TEST-UNSIGNED` est identifié comme test uniquement.

## Contenu

- macOS universel : AU dans
  `/Library/Audio/Plug-Ins/Components/PEDAL LEGENDS.component` et VST3 dans
  `/Library/Audio/Plug-Ins/VST3/PEDAL LEGENDS.vst3`.
- Windows x64 : `C:\Program Files\Common Files\VST3\PEDAL LEGENDS.vst3`,
  avec désinstallation via Inno Setup. Windows nécessite séparément une
  signature Authenticode pour éviter certains avertissements SmartScreen.

## Télécharger

Ouvrir `Actions` → `PEDAL LEGENDS - Mac notarized PKG and Windows EXE`,
puis le dernier run vert sur `main`. Télécharger les archives des
installateurs dans **Artifacts** et extraire le `.pkg` ou `.exe`.
Tester ensuite les plugins dans les DAW cibles avant distribution.
