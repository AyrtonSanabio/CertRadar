[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
$packageScript = Join-Path $repoRoot "scripts\package-legacy.ps1"
$verifyScript = Join-Path $repoRoot "scripts\verify-legacy-package.ps1"
$cmakeProject = Get-Content -LiteralPath (Join-Path $repoRoot "CMakeLists.txt") -Raw
if ($cmakeProject -notmatch '(?ms)project\s*\(.*?\bVERSION\s+([0-9]+\.[0-9]+\.[0-9]+)') {
    throw "Project version was not found in CMakeLists.txt."
}
$productVersion = $Matches[1]
$packageName = "CertRadar-$productVersion-windows-xp-x86-legacy-untested"
$packageDirectory = Join-Path $repoRoot "artifacts\$packageName"
$archivePath = Join-Path $repoRoot "artifacts\$packageName.zip"
$archiveHashPath = "$archivePath.sha256"
$executablePath = Join-Path $packageDirectory "certradar-legacy.exe"
$testRoot = Join-Path $repoRoot "artifacts\legacy-test-$PID"

try {
    & $packageScript
    & $verifyScript -PackageDirectory $packageDirectory

    New-Item -ItemType Directory -Path (Join-Path $testRoot "Downloads\sub") -Force | Out-Null
    New-Item -ItemType Directory -Path (Join-Path $testRoot "Documents") -Force | Out-Null
    Set-Content -LiteralPath (Join-Path $testRoot "Downloads\cliente.PFX") -Value "candidate" -Encoding ascii
    Set-Content -LiteralPath (Join-Path $testRoot "Downloads\sub\backup.p12") -Value "candidate" -Encoding ascii
    Set-Content -LiteralPath (Join-Path $testRoot "Documents\publico.cer") -Value "not-a-container" -Encoding ascii

    $output = (& $executablePath --scan-root $testRoot 2>&1 | Out-String)
    if ($LASTEXITCODE -ne 0) { throw "Legacy collector exited with code $LASTEXITCODE." }
    if (-not $output.Contains("cliente.PFX") -or -not $output.Contains("backup.p12")) {
        throw "Legacy collector did not find the controlled PFX/P12 samples."
    }
    if ($output.Contains("publico.cer")) {
        throw "Legacy collector reported a non-container extension."
    }
    if (-not $output.Contains("2 candidato(s)")) {
        throw "Legacy collector reported an unexpected candidate count."
    }
    if (-not (Test-Path -LiteralPath $archivePath -PathType Leaf) -or
        -not (Test-Path -LiteralPath $archiveHashPath -PathType Leaf)) {
        throw "Legacy ZIP or external SHA-256 was not created."
    }
    $expectedArchiveHash = (Get-Content -LiteralPath $archiveHashPath -Raw).Trim().Split(' ')[0]
    if ((Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash -ne $expectedArchiveHash) {
        throw "Legacy archive SHA-256 does not match."
    }
} finally {
    $artifactsRoot = [IO.Path]::GetFullPath((Join-Path $repoRoot "artifacts")).TrimEnd('\') + '\'
    $testFull = [IO.Path]::GetFullPath($testRoot)
    if ($testFull.StartsWith($artifactsRoot, [StringComparison]::OrdinalIgnoreCase) -and
        (Test-Path -LiteralPath $testFull)) {
        Remove-Item -LiteralPath $testFull -Recurse -Force
    }
}

Write-Host "Legacy package tests passed."
