[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$PackageDirectory
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$packagePath = [IO.Path]::GetFullPath($PackageDirectory)
if (-not (Test-Path -LiteralPath $packagePath -PathType Container)) {
    throw "Legacy package directory does not exist: $packagePath"
}

$expectedFiles = @("certradar-legacy.exe", "LEIA-ME-LEGACY.txt", "SECURITY.md", "SHA256SUMS.txt")
$actualFiles = @(Get-ChildItem -LiteralPath $packagePath -File | ForEach-Object Name | Sort-Object)
if (($actualFiles -join "`n") -ne (($expectedFiles | Sort-Object) -join "`n")) {
    throw "Legacy package file set is not the expected allowlist. Found: $($actualFiles -join ', ')"
}

$manifestLines = @(Get-Content -LiteralPath (Join-Path $packagePath "SHA256SUMS.txt") |
    Where-Object { -not [string]::IsNullOrWhiteSpace($_) })
if ($manifestLines.Count -ne 3) { throw "Legacy SHA-256 manifest must contain three payloads." }
foreach ($line in $manifestLines) {
    if ($line -notmatch '^(?<hash>[A-Fa-f0-9]{64})  (?<name>[^\\/]+)$') {
        throw "Invalid legacy SHA-256 manifest line: $line"
    }
    $payload = Join-Path $packagePath $Matches.name
    if (-not (Test-Path -LiteralPath $payload -PathType Leaf)) {
        throw "Legacy manifest references a missing file: $($Matches.name)"
    }
    if ((Get-FileHash -LiteralPath $payload -Algorithm SHA256).Hash -ne $Matches.hash) {
        throw "Legacy SHA-256 mismatch for $($Matches.name)."
    }
}

& (Join-Path $PSScriptRoot "verify-legacy.ps1") `
    -ExecutablePath (Join-Path $packagePath "certradar-legacy.exe")
Write-Host "Legacy package verification passed: $packagePath"
