[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$PackageDirectory,
    [Parameter(Mandatory)]
    [ValidateSet("x64", "Win32")]
    [string]$Architecture,
    [string]$OutputDirectory
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
$packagePath = [IO.Path]::GetFullPath($PackageDirectory)
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repoRoot "artifacts\compatibility-evidence"
}
$outputPath = [IO.Path]::GetFullPath($OutputDirectory)

& (Join-Path $PSScriptRoot "verify-package.ps1") `
    -PackageDirectory $packagePath `
    -Architecture $Architecture
& (Join-Path $PSScriptRoot "smoke-test-package.ps1") `
    -PackageDirectory $packagePath

$operatingSystem = Get-CimInstance Win32_OperatingSystem
$executablePath = Join-Path $packagePath "certradar.exe"
$executableHash = (Get-FileHash -LiteralPath $executablePath -Algorithm SHA256).Hash
$architectureSlug = if ($Architecture -eq "Win32") { "x86" } else { "x64" }
$timestamp = [DateTimeOffset]::UtcNow

$evidence = [ordered]@{
    schema = "certradar.compatibility-evidence/v1"
    collected_at_utc = $timestamp.ToString("o")
    product = [ordered]@{
        package_id = Split-Path -Leaf $packagePath
        executable_sha256 = $executableHash
        architecture = $architectureSlug
        signature = "unsigned_development_build"
    }
    platform = [ordered]@{
        family = "windows"
        caption = [string]$operatingSystem.Caption
        version = [string]$operatingSystem.Version
        architecture = [string]$operatingSystem.OSArchitecture
    }
    automated_checks = [ordered]@{
        package_integrity = $true
        startup_smoke_test = $true
    }
    manual_checks = @(
        [ordered]@{ id = "initial_screen"; status = "pending"; description = "Confirmar que a janela inicial abre sem solicitar elevacao." },
        [ordered]@{ id = "priority_search"; status = "pending"; description = "Executar busca com amostra controlada em Downloads, Area de Trabalho e Documentos." },
        [ordered]@{ id = "pause_resume"; status = "pending"; description = "Pausar e continuar uma busca sem perder resultados." },
        [ordered]@{ id = "cancel"; status = "pending"; description = "Cancelar uma busca longa e confirmar resultados parciais." },
        [ordered]@{ id = "full_scan"; status = "pending"; description = "Concluir a busca progressiva sem acessar rede nem contornar permissoes." },
        [ordered]@{ id = "support_summary"; status = "pending"; description = "Revisar o resumo sanitizado gerado para o chamado." }
    )
    qualification = "pending_manual_validation"
}

New-Item -ItemType Directory -Path $outputPath -Force | Out-Null
$fileName = "compatibility-$architectureSlug-$($timestamp.ToString('yyyyMMddTHHmmssZ')).json"
$evidencePath = Join-Path $outputPath $fileName
$evidence | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $evidencePath -Encoding utf8

Write-Output $evidencePath
