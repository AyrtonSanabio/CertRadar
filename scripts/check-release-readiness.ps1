[CmdletBinding()]
param(
    [string]$OutputDirectory
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repoRoot "artifacts\release-readiness"
}
$outputPath = [IO.Path]::GetFullPath($OutputDirectory)
$cmakeProject = Get-Content -LiteralPath (Join-Path $repoRoot "CMakeLists.txt") -Raw
if ($cmakeProject -notmatch '(?ms)project\s*\(.*?\bVERSION\s+([0-9]+\.[0-9]+\.[0-9]+)') {
    throw "Project version was not found in CMakeLists.txt."
}
$productVersion = $Matches[1]

$blockers = @()
function Add-Blocker([string]$Id, [string]$Reason) {
    if (@($blockers | Where-Object id -eq $Id).Count -eq 0) {
        $script:blockers += [ordered]@{ id = $Id; reason = $Reason }
    }
}

$packageVerifier = Join-Path $PSScriptRoot "verify-package.ps1"
foreach ($architecture in @(@("x64", "x64"), @("Win32", "x86"))) {
    $buildArchitecture = $architecture[0]
    $packageArchitecture = $architecture[1]
    $packageName = "CertRadar-$productVersion-windows-$packageArchitecture-unsigned"
    $packageDirectory = Join-Path $repoRoot "artifacts\$packageName"
    $buildExecutable = Join-Path $repoRoot "build\$buildArchitecture\Release\certradar.exe"
    if (-not (Test-Path -LiteralPath $packageDirectory -PathType Container)) {
        Add-Blocker "package_$packageArchitecture" "Pacote Release $packageArchitecture ausente."
        continue
    }
    try {
        & $packageVerifier -PackageDirectory $packageDirectory -Architecture $buildArchitecture
    } catch {
        Add-Blocker "package_$packageArchitecture" "Pacote Release $packageArchitecture falhou na verificacao."
    }
    $packagedExecutable = Join-Path $packageDirectory "certradar.exe"
    if ((Test-Path -LiteralPath $buildExecutable -PathType Leaf) -and
        (Get-FileHash -LiteralPath $buildExecutable -Algorithm SHA256).Hash -ne
        (Get-FileHash -LiteralPath $packagedExecutable -Algorithm SHA256).Hash) {
        Add-Blocker "package_${packageArchitecture}_stale" "O executavel empacotado nao corresponde ao Release atual."
    }
    $signature = Get-AuthenticodeSignature -LiteralPath $packagedExecutable
    if ($signature.Status -ne [System.Management.Automation.SignatureStatus]::Valid) {
        Add-Blocker "code_signing" "O pacote $packageArchitecture nao possui assinatura Authenticode valida."
    }
}

$evidenceDirectory = Join-Path $repoRoot "artifacts\compatibility-evidence"
$vmEvidence = @()
if (Test-Path -LiteralPath $evidenceDirectory -PathType Container) {
    foreach ($file in @(Get-ChildItem -LiteralPath $evidenceDirectory -Filter "*.json" -File)) {
        try { $vmEvidence += Get-Content -LiteralPath $file.FullName -Raw | ConvertFrom-Json } catch { }
    }
}
$qualifiedWindows = @($vmEvidence | Where-Object {
    $_.qualification -eq "qualified" -and $_.platform.version -match '^10\.'
})
if ($qualifiedWindows.Count -lt 2 -or
    @($qualifiedWindows | Select-Object -ExpandProperty platform | Select-Object -ExpandProperty caption |
        Where-Object { $_ -match 'Windows 11' }).Count -eq 0 -or
    @($qualifiedWindows | Select-Object -ExpandProperty platform | Select-Object -ExpandProperty caption |
        Where-Object { $_ -match 'Windows 10' }).Count -eq 0) {
    Add-Blocker "windows_vms" "Nao ha evidencias qualificadas para Windows 10 e Windows 11."
}

$hardwareEvidencePath = Join-Path $repoRoot "artifacts\hardware\a3-signature-evidence.json"
$hardwareQualified = $false
if (Test-Path -LiteralPath $hardwareEvidencePath -PathType Leaf) {
    try {
        $hardwareEvidence = Get-Content -LiteralPath $hardwareEvidencePath -Raw | ConvertFrom-Json
        $hardwareQualified = $hardwareEvidence.qualified -eq $true -and
            $hardwareEvidence.private_key_exported -eq $false
    } catch { $hardwareQualified = $false }
}
if (-not $hardwareQualified) {
    Add-Blocker "a3_hardware" "Nao ha prova de assinatura A3 real em hardware de laboratorio."
}

$licensePath = @(Join-Path $repoRoot "LICENSE", Join-Path $repoRoot "LICENSE.txt") |
    Where-Object { Test-Path -LiteralPath $_ -PathType Leaf } | Select-Object -First 1
if ($null -eq $licensePath) {
    Add-Blocker "license" "O repositorio ainda nao possui arquivo LICENSE."
}

$workingTree = (& git -C $repoRoot status --porcelain)
if ($LASTEXITCODE -ne 0) { throw "Git status failed." }
if (-not [string]::IsNullOrWhiteSpace(($workingTree -join "`n"))) {
    Add-Blocker "working_tree" "A arvore de trabalho possui alteracoes nao commitadas."
}

$evidence = [ordered]@{
    schema = "certradar.release-readiness/v1"
    evaluated_at_utc = [DateTimeOffset]::UtcNow.ToString("o")
    product_version = $productVersion
    current_commit = ((& git -C $repoRoot rev-parse --short HEAD).Trim())
    status = if ($blockers.Count -eq 0) { "ready" } else { "blocked" }
    blockers = @($blockers)
    external_gates = [ordered]@{
        windows_10_11_vms = ($blockers.id -notcontains "windows_vms")
        a3_hardware = ($blockers.id -notcontains "a3_hardware")
        license = ($blockers.id -notcontains "license")
        code_signing = ($blockers.id -notcontains "code_signing")
    }
}
New-Item -ItemType Directory -Path $outputPath -Force | Out-Null
$evidencePath = Join-Path $outputPath "release-readiness.json"
$evidence | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $evidencePath -Encoding utf8
Write-Host "Release readiness status: $($evidence.status)"
Write-Output $evidencePath
