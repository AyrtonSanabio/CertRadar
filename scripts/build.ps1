[CmdletBinding()]
param(
    [ValidateSet("x64", "Win32")]
    [string]$Architecture = "x64",
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
$buildDirectory = Join-Path $repoRoot "build\$Architecture"
$configureScript = Join-Path $PSScriptRoot "configure.ps1"

& $configureScript -Architecture $Architecture -BuildDirectory $buildDirectory

$cmakeCache = Join-Path $buildDirectory "CMakeCache.txt"
if (-not (Test-Path -LiteralPath $cmakeCache -PathType Leaf)) {
    throw "CMake cache was not created at $cmakeCache."
}

$cmakePath = (Select-String -LiteralPath $cmakeCache -Pattern '^CMAKE_COMMAND:INTERNAL=(.+)$').Matches.Groups[1].Value
if ([string]::IsNullOrWhiteSpace($cmakePath)) {
    throw "CMake executable path was not found in the cache."
}

Write-Host "Building CertRadar ($Architecture/$Configuration)..."
& $cmakePath --build $buildDirectory --config $Configuration --parallel
if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE."
}
