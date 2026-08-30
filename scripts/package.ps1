[CmdletBinding()]
param(
    [ValidateSet("x64", "Win32")]
    [string]$Architecture = "x64"
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
$artifactsRoot = Join-Path $repoRoot "artifacts"
$testScript = Join-Path $PSScriptRoot "test.ps1"
$verifyScript = Join-Path $PSScriptRoot "verify-package.ps1"
$cmakeProject = Get-Content -LiteralPath (Join-Path $repoRoot "CMakeLists.txt") -Raw
if ($cmakeProject -notmatch '(?ms)project\s*\(.*?\bVERSION\s+([0-9]+\.[0-9]+\.[0-9]+)') {
    throw "Project version was not found in CMakeLists.txt."
}
$productVersion = $Matches[1]
$architectureSlug = if ($Architecture -eq "Win32") { "x86" } else { "x64" }
$packageName = "CertRadar-$productVersion-windows-$architectureSlug-unsigned"
$packageDirectory = Assert-StrictChildPath (Join-Path $artifactsRoot $packageName) $artifactsRoot
$archivePath = Assert-StrictChildPath (Join-Path $artifactsRoot "$packageName.zip") $artifactsRoot
$archiveHashPath = Assert-StrictChildPath "$archivePath.sha256" $artifactsRoot

& $testScript -Architecture $Architecture -Configuration Release

$executablePath = Join-Path $repoRoot "build\$Architecture\Release\certradar.exe"
if (-not (Test-Path -LiteralPath $executablePath -PathType Leaf)) {
    throw "Release executable was not created: $executablePath"
}

New-Item -ItemType Directory -Path $artifactsRoot -Force | Out-Null
if (Test-Path -LiteralPath $packageDirectory) {
    Remove-Item -LiteralPath $packageDirectory -Recurse -Force
}
New-Item -ItemType Directory -Path $packageDirectory | Out-Null

Copy-Item -LiteralPath $executablePath -Destination (Join-Path $packageDirectory "certradar.exe")
Copy-Item -LiteralPath (Join-Path $repoRoot "distribution\LEIA-ME.txt") -Destination $packageDirectory
Copy-Item -LiteralPath (Join-Path $repoRoot "SECURITY.md") -Destination $packageDirectory

$payloadFiles = @(Get-ChildItem -LiteralPath $packageDirectory -File | Sort-Object Name)
$manifestLines = foreach ($file in $payloadFiles) {
    $hash = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
    "$hash  $($file.Name)"
}
$manifestLines | Set-Content -LiteralPath (Join-Path $packageDirectory "SHA256SUMS.txt") -Encoding ascii

& $verifyScript -PackageDirectory $packageDirectory -Architecture $Architecture

if (Test-Path -LiteralPath $archivePath) { Remove-Item -LiteralPath $archivePath -Force }
if (Test-Path -LiteralPath $archiveHashPath) { Remove-Item -LiteralPath $archiveHashPath -Force }
Compress-Archive -Path (Join-Path $packageDirectory "*") -DestinationPath $archivePath -CompressionLevel Optimal
$archiveHash = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash
"$archiveHash  $packageName.zip" | Set-Content -LiteralPath $archiveHashPath -Encoding ascii

Write-Host "Portable package created: $archivePath"
