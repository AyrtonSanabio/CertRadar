[CmdletBinding()]
param(
    [ValidateSet("x64", "Win32")]
    [string]$Architecture = "x64"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
$packageScript = Join-Path $repoRoot "scripts\package.ps1"
$verifyScript = Join-Path $repoRoot "scripts\verify-package.ps1"
$smokeTestScript = Join-Path $repoRoot "scripts\smoke-test-package.ps1"
$cmakeProject = Get-Content -LiteralPath (Join-Path $repoRoot "CMakeLists.txt") -Raw
if ($cmakeProject -notmatch '(?ms)project\s*\(.*?\bVERSION\s+([0-9]+\.[0-9]+\.[0-9]+)') {
    throw "Project version was not found in CMakeLists.txt."
}
$productVersion = $Matches[1]
$architectureSlug = if ($Architecture -eq "Win32") { "x86" } else { "x64" }
$packageName = "CertRadar-$productVersion-windows-$architectureSlug-unsigned"
$packageDirectory = Join-Path $repoRoot "artifacts\$packageName"
$archivePath = Join-Path $repoRoot "artifacts\$packageName.zip"
$archiveHashPath = "$archivePath.sha256"

& $packageScript -Architecture $Architecture
& $verifyScript -PackageDirectory $packageDirectory -Architecture $Architecture
& $smokeTestScript -PackageDirectory $packageDirectory

if (-not (Test-Path -LiteralPath $archivePath -PathType Leaf)) {
    throw "Portable archive was not created: $archivePath"
}
if (-not (Test-Path -LiteralPath $archiveHashPath -PathType Leaf)) {
    throw "Archive hash was not created: $archiveHashPath"
}

$expectedHash = (Get-Content -LiteralPath $archiveHashPath -Raw).Trim().Split(' ')[0]
$actualHash = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash
if ($expectedHash -ne $actualHash) {
    throw "Portable archive SHA-256 does not match."
}

Write-Host "CertRadar package test passed ($Architecture)."
