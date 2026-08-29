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
$buildScript = Join-Path $PSScriptRoot "build.ps1"

& $buildScript -Architecture $Architecture -Configuration $Configuration

$cmakeCache = Join-Path $buildDirectory "CMakeCache.txt"
$ctestPath = (Select-String -LiteralPath $cmakeCache -Pattern '^CMAKE_CTEST_COMMAND:INTERNAL=(.+)$').Matches.Groups[1].Value
if ([string]::IsNullOrWhiteSpace($ctestPath)) {
    throw "CTest executable path was not found in the CMake cache."
}

Write-Host "Running CertRadar tests ($Architecture/$Configuration)..."
& $ctestPath --test-dir $buildDirectory -C $Configuration --output-on-failure
if ($LASTEXITCODE -ne 0) {
    throw "Tests failed with exit code $LASTEXITCODE."
}

Write-Host "All CertRadar tests passed."
