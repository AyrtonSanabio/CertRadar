[CmdletBinding()]
param(
    [ValidateSet("x64", "Win32")]
    [string]$Architecture = "x64"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
$cmakeProject = Get-Content -LiteralPath (Join-Path $repoRoot "CMakeLists.txt") -Raw
if ($cmakeProject -notmatch '(?ms)project\s*\(.*?\bVERSION\s+([0-9]+\.[0-9]+\.[0-9]+)') {
    throw "Project version was not found in CMakeLists.txt."
}
$productVersion = $Matches[1]
$architectureSlug = if ($Architecture -eq "Win32") { "x86" } else { "x64" }
$packageDirectory = Join-Path $repoRoot "artifacts\CertRadar-$productVersion-windows-$architectureSlug-unsigned"
$collector = Join-Path $repoRoot "scripts\collect-vm-evidence.ps1"
$outputDirectory = Join-Path $repoRoot "artifacts\vm-evidence-test-$PID-$architectureSlug"

if (-not (Test-Path -LiteralPath $packageDirectory -PathType Container)) {
    & (Join-Path $repoRoot "scripts\package.ps1") -Architecture $Architecture
}

try {
    $evidencePath = & $collector `
        -PackageDirectory $packageDirectory `
        -Architecture $Architecture `
        -OutputDirectory $outputDirectory
    if (-not (Test-Path -LiteralPath $evidencePath -PathType Leaf)) {
        throw "VM evidence collector did not return a JSON file."
    }

    $rawEvidence = Get-Content -LiteralPath $evidencePath -Raw
    $evidence = $rawEvidence | ConvertFrom-Json
    if ($evidence.schema -ne "certradar.compatibility-evidence/v1") {
        throw "Unexpected compatibility evidence schema."
    }
    if (-not $evidence.automated_checks.package_integrity -or
        -not $evidence.automated_checks.startup_smoke_test) {
        throw "Automated VM checks were not recorded as passed."
    }
    if ($evidence.qualification -ne "pending_manual_validation") {
        throw "Automated evidence must not claim full VM qualification."
    }
    if (@($evidence.manual_checks | Where-Object status -ne "pending").Count -ne 0) {
        throw "Manual checks must remain pending until a person performs them in the VM."
    }
    foreach ($sensitiveValue in @($env:COMPUTERNAME, $env:USERNAME)) {
        if (-not [string]::IsNullOrWhiteSpace($sensitiveValue) -and
            $rawEvidence.Contains($sensitiveValue, [StringComparison]::OrdinalIgnoreCase)) {
            throw "Evidence leaked a local machine or user identifier."
        }
    }
} finally {
    $artifactsRoot = [IO.Path]::GetFullPath((Join-Path $repoRoot "artifacts")).TrimEnd('\') + '\'
    $outputFull = [IO.Path]::GetFullPath($outputDirectory)
    if ($outputFull.StartsWith($artifactsRoot, [StringComparison]::OrdinalIgnoreCase) -and
        (Test-Path -LiteralPath $outputFull)) {
        Remove-Item -LiteralPath $outputFull -Recurse -Force
    }
}

Write-Host "VM evidence test passed ($Architecture)."
