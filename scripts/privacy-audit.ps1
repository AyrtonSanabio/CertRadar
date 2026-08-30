[CmdletBinding()]
param(
    [string]$OutputDirectory
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $repoRoot "artifacts\privacy-audit"
}
$outputPath = [IO.Path]::GetFullPath($OutputDirectory)

& (Join-Path $PSScriptRoot "test.ps1") -Architecture x64 -Configuration Debug
& (Join-Path $PSScriptRoot "test.ps1") -Architecture Win32 -Configuration Debug

$trackedFiles = @(& git -C $repoRoot ls-files)
if ($LASTEXITCODE -ne 0) { throw "Tracked file inventory failed." }
$credentialExtensions = @(".pfx", ".p12", ".key", ".pem", ".p7b", ".p7c")
$trackedCredentials = @($trackedFiles | Where-Object {
    $credentialExtensions -contains [IO.Path]::GetExtension($_).ToLowerInvariant()
})
if ($trackedCredentials.Count -ne 0) {
    throw "Tracked credential material found: $($trackedCredentials -join ', ')"
}

$productionFiles = @(
    Get-ChildItem -LiteralPath (Join-Path $repoRoot "src"),(Join-Path $repoRoot "include") `
        -Recurse -File -Include "*.cpp","*.hpp" | Sort-Object FullName
)
$forbiddenRules = [ordered]@{
    private_key_export = '\b(PFXExportCertStoreEx|CryptExportKey|NCryptExportKey)\s*\('
    network_upload = '\b(WinHttpSendRequest|HttpSendRequest[AW]?|InternetWriteFile|send)\s*\('
    arbitrary_process = '\b(CreateProcess[AW]?|WinExec|system)\s*\('
    persistent_write = '\b(WriteFile|fwrite)\s*\(|std::ofstream'
}
$violations = @()
foreach ($rule in $forbiddenRules.GetEnumerator()) {
    $matches = @(Select-String -LiteralPath $productionFiles.FullName -Pattern $rule.Value)
    foreach ($match in $matches) {
        $violations += [pscustomobject]@{
            rule = $rule.Key
            file = [IO.Path]::GetRelativePath($repoRoot, $match.Path).Replace('\', '/')
            line = $match.LineNumber
        }
    }
}
if ($violations.Count -ne 0) {
    throw "Privacy audit found forbidden production behavior: $($violations | ConvertTo-Json -Compress)"
}

$evidence = [ordered]@{
    schema = "certradar.privacy-audit/v1"
    audited_at_utc = [DateTimeOffset]::UtcNow.ToString("o")
    status = "passed"
    modern_test_architectures = @("x64", "Win32")
    tracked_file_count = $trackedFiles.Count
    production_source_count = $productionFiles.Count
    tracked_credential_files = 0
    forbidden_behavior_findings = 0
    safeguards = @(
        "defensive_report_sanitization",
        "no_private_key_export_api",
        "no_production_upload_api",
        "no_arbitrary_process_execution",
        "no_production_persistent_write"
    )
}
New-Item -ItemType Directory -Path $outputPath -Force | Out-Null
$evidencePath = Join-Path $outputPath "privacy-audit.json"
$evidence | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $evidencePath -Encoding utf8
Write-Host "Privacy audit passed. Evidence: $evidencePath"
