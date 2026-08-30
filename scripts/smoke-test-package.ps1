[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$PackageDirectory
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$executablePath = Join-Path ([IO.Path]::GetFullPath($PackageDirectory)) "certradar.exe"
if (-not (Test-Path -LiteralPath $executablePath -PathType Leaf)) {
    throw "Packaged executable does not exist: $executablePath"
}

$process = Start-Process -FilePath $executablePath -PassThru -WindowStyle Hidden
try {
    if (-not $process.WaitForInputIdle(5000)) {
        throw "CertRadar did not reach an idle UI state within five seconds."
    }
    $process.Refresh()
    if ($process.HasExited) {
        throw "CertRadar exited during the startup smoke test with code $($process.ExitCode)."
    }
} finally {
    if (-not $process.HasExited) {
        $null = $process.CloseMainWindow()
        $exitedCleanly = $process.WaitForExit(3000)
        if (-not $exitedCleanly) {
            Stop-Process -Id $process.Id
            $null = $process.WaitForExit(3000)
        }
    }
    $process.Dispose()
}

Write-Host "Packaged executable startup smoke test passed."
