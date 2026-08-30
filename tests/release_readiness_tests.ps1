[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
$readinessScript = Join-Path $repoRoot "scripts\check-release-readiness.ps1"
$outputDirectory = Join-Path $repoRoot "artifacts\release-readiness-test-$PID"

try {
    $evidencePath = & $readinessScript -OutputDirectory $outputDirectory
    if (-not (Test-Path -LiteralPath $evidencePath -PathType Leaf)) {
        throw "Release readiness script did not return an evidence file."
    }
    $evidence = Get-Content -LiteralPath $evidencePath -Raw | ConvertFrom-Json
    if ($evidence.schema -ne "certradar.release-readiness/v1") {
        throw "Unexpected release readiness schema."
    }
    if ($evidence.status -ne "blocked") {
        throw "The current unqualified workspace must not be marked ready."
    }
    $blockerIds = @($evidence.blockers | ForEach-Object id)
    foreach ($requiredBlocker in @("windows_vms", "a3_hardware", "license", "code_signing")) {
        if ($blockerIds -notcontains $requiredBlocker) {
            throw "Required release blocker is missing: $requiredBlocker"
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

Write-Host "Release readiness gate test passed."
