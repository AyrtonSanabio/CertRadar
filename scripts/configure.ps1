[CmdletBinding()]
param(
    [ValidateSet("x64", "Win32")]
    [string]$Architecture = "x64",
    [string]$BuildDirectory = "",
    [string]$CMake = ""
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($BuildDirectory)) {
    $BuildDirectory = Join-Path $repoRoot "build\$Architecture"
}

if ([string]::IsNullOrWhiteSpace($CMake)) {
    $cmakeCommand = Get-Command "cmake.exe" -ErrorAction SilentlyContinue
    if ($null -ne $cmakeCommand) {
        $CMake = $cmakeCommand.Source
    } else {
        $knownCMake = "C:\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
        if (Test-Path -LiteralPath $knownCMake -PathType Leaf) {
            $CMake = $knownCMake
        } else {
            throw "CMake not found. Pass its full path with -CMake."
        }
    }
}

Write-Host "Configuring CertRadar for $Architecture..."
& $CMake -S $repoRoot -B $BuildDirectory -G "Visual Studio 17 2022" -A $Architecture
if ($LASTEXITCODE -ne 0) {
    throw "CMake configuration failed with exit code $LASTEXITCODE."
}
