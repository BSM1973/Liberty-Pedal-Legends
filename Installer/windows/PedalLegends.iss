; PEDAL LEGENDS - 64-bit Windows VST3 installer.
; Values are provided by Installer/windows/build_installer.ps1 on the CI runner.
#define ProductVersion GetEnv("PEDAL_LEGENDS_VERSION")
#define PluginSource GetEnv("PEDAL_LEGENDS_BUNDLE")
#define InstallerOutput GetEnv("PEDAL_LEGENDS_OUTPUT")

[Setup]
AppId=LibertyMusicSoftware.PedalLegends.VST3
AppName=PEDAL LEGENDS
AppVersion={#ProductVersion}
AppVerName=PEDAL LEGENDS {#ProductVersion}
AppPublisher=Liberty Music Software
AppPublisherURL=https://libertymusicsoftware.com
AppSupportURL=https://libertymusicsoftware.com
DefaultDirName={autopf}\Liberty Music Software\PEDAL LEGENDS
DisableDirPage=yes
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
WizardStyle=modern
OutputDir={#InstallerOutput}
OutputBaseFilename=PEDAL-LEGENDS-Windows-x64-VST3-v{#ProductVersion}-unsigned
Compression=lzma2
SolidCompression=yes
Uninstallable=yes
SetupLogging=yes
UsePreviousAppDir=no

[Files]
; Installs the entire JUCE VST3 bundle including Contents/x86_64-win.
Source: "{#PluginSource}\*"; DestDir: "{commoncf64}\VST3\PEDAL LEGENDS.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Languages]
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
