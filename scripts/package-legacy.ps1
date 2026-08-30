[CmdletBinding()]
param(
    [string]$CompilerPath
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Assert-StrictChildPath {
    param([string]$ChildPath, [string]$ParentPath)
    $parentFull = [IO.Path]::GetFullPath($ParentPath).TrimEnd('\') + '\'
    $childFull = [IO.Path]::GetFullPath($ChildPath)
    if (-not $childFull.StartsWith($parentFull, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing path outside the intended directory: $childFull"
    }
    return $childFull
}

$repoRoot = Split-Path -Parent $PSScriptRoot
$cmakeProject = Get-Content -LiteralPath (Join-Path $repoRoot "CMakeLists.txt") -Raw
if ($cmakeProject -notmatch '(?ms)project\s*\(.*?\bVERSION\s+([0-9]+\.[0-9]+\.[0-9]+)') {
    throw "Project version was not found in CMakeLists.txt."
}
$productVersion = $Matches[1]
$packageName = "CertRadar-$productVersion-windows-xp-x86-legacy-untested"
$artifactsRoot = Join-Path $repoRoot "artifacts"
$packageDirectory = Assert-StrictChildPath (Join-Path $artifactsRoot $packageName) $artifactsRoot
$archivePath = Assert-StrictChildPath (Join-Path $artifactsRoot "$packageName.zip") $artifactsRoot
$archiveHashPath = Assert-StrictChildPath "$archivePath.sha256" $artifactsRoot

& (Join-Path $PSScriptRoot "build-legacy.ps1") -CompilerPath $CompilerPath
$legacyExecutable = Join-Path $repoRoot "build\legacy\certradar-legacy.exe"
& (Join-Path $PSScriptRoot "verify-legacy.ps1") -ExecutablePath $legacyExecutable

New-Item -ItemType Directory -Path $artifactsRoot -Force | Out-Null
if (Test-Path -LiteralPath $packageDirectory) {
    Remove-Item -LiteralPath $packageDirectory -Recurse -Force
}
New-Item -ItemType Directory -Path $packageDirectory | Out-Null
Copy-Item -LiteralPath $legacyExecutable -Destination $packageDirectory
Copy-Item -LiteralPath (Join-Path $repoRoot "distribution\LEIA-ME-LEGACY.txt") -Destination $packageDirectory
Copy-Item -LiteralPath (Join-Path $repoRoot "SECURITY.md") -Destination $packageDirectory

$manifestLines = foreach ($file in @(Get-ChildItem -LiteralPath $packageDirectory -File | Sort-Object Name)) {
    "$((Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash)  $($file.Name)"
}
$manifestLines | Set-Content -LiteralPath (Join-Path $packageDirectory "SHA256SUMS.txt") -Encoding ascii
& (Join-Path $PSScriptRoot "verify-legacy-package.ps1") -PackageDirectory $packageDirectory

if (Test-Path -LiteralPath $archivePath) { Remove-Item -LiteralPath $archivePath -Force }
if (Test-Path -LiteralPath $archiveHashPath) { Remove-Item -LiteralPath $archiveHashPath -Force }
Compress-Archive -Path (Join-Path $packageDirectory "*") -DestinationPath $archivePath -CompressionLevel Optimal
$archiveHash = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash
"$archiveHash  $packageName.zip" | Set-Content -LiteralPath $archiveHashPath -Encoding ascii
Write-Host "Untested legacy package created: $archivePath"
