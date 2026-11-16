# PEDAL LEGENDS Windows x64 VST3 installer (Inno Setup).
[CmdletBinding()]
param([string]$Version = "0.6.1")
$ErrorActionPreference = "Stop"

$root = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$bundleRoot = Join-Path $root "build/PedalLegends_artefacts/Release/VST3"
if (-not (Test-Path -LiteralPath $bundleRoot -PathType Container)) {
    throw "Missing VST3 build output: $bundleRoot"
}
$bundles = @(Get-ChildItem -LiteralPath $bundleRoot -Directory -Filter "*.vst3")
if ($bundles.Count -ne 1) {
    throw "Expected exactly one VST3 bundle, found $($bundles.Count)."
}
$bundle = $bundles[0]
$binaries = @(Get-ChildItem -LiteralPath $bundle.FullName -Recurse -File -Filter "*.vst3")
if ($binaries.Count -lt 1) {
    throw "No VST3 executable found inside $($bundle.FullName)."
}

$outDir = Join-Path $root "dist/installers"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$env:PEDAL_LEGENDS_VERSION = $Version
$env:PEDAL_LEGENDS_BUNDLE = $bundle.FullName
$env:PEDAL_LEGENDS_OUTPUT = $outDir

$iscc = $null
$candidates = @()
if ($env:ProgramFiles) { $candidates += (Join-Path $env:ProgramFiles "Inno Setup 6/ISCC.exe") }
if (${env:ProgramFiles(x86)}) { $candidates += (Join-Path ${env:ProgramFiles(x86)} "Inno Setup 6/ISCC.exe") }
foreach ($candidate in $candidates) {
    if (Test-Path -LiteralPath $candidate) { $iscc = $candidate; break }
}
if (-not $iscc) {
    $cmd = Get-Command ISCC.exe -ErrorAction SilentlyContinue
    if ($cmd) { $iscc = $cmd.Source }
}
if (-not $iscc) {
    Write-Host "Installing Inno Setup using Chocolatey..."
    choco install innosetup --yes --no-progress --limit-output
    if ($LASTEXITCODE -ne 0) { throw "Inno Setup installation failed." }
    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate) { $iscc = $candidate; break }
    }
}
if (-not $iscc) { throw "ISCC.exe not found." }

$source = Join-Path $PSScriptRoot "PedalLegends.iss"
Write-Host "Packaging $($bundle.FullName) with $iscc"
& $iscc $source
if ($LASTEXITCODE -ne 0) { throw "Inno Setup compilation failed (exit $LASTEXITCODE)." }

$exeName = "PEDAL-LEGENDS-Windows-x64-VST3-v$Version-unsigned.exe"
$exe = Join-Path $outDir $exeName
if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "Installer not generated: $exe"
}
if ((Get-Item -LiteralPath $exe).Length -lt 100000) {
    throw "Installer unexpectedly small: $exe"
}
Write-Host "Created and checked: $exe"
Get-Item -LiteralPath $exe | Select-Object Name, Length, FullName | Format-List
