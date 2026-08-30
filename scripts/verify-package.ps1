[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$PackageDirectory,
    [Parameter(Mandatory)]
    [ValidateSet("x64", "Win32")]
    [string]$Architecture
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$packagePath = [IO.Path]::GetFullPath($PackageDirectory)
if (-not (Test-Path -LiteralPath $packagePath -PathType Container)) {
    throw "Package directory does not exist: $packagePath"
}

$expectedFiles = @("certradar.exe", "LEIA-ME.txt", "SECURITY.md", "SHA256SUMS.txt")
$actualFiles = @(Get-ChildItem -LiteralPath $packagePath -File | ForEach-Object Name | Sort-Object)
$expectedSorted = @($expectedFiles | Sort-Object)
if (($actualFiles -join "`n") -ne ($expectedSorted -join "`n")) {
    throw "Package file set is not the expected minimal allowlist. Found: $($actualFiles -join ', ')"
}

$forbidden = @(Get-ChildItem -LiteralPath $packagePath -Recurse -File |
    Where-Object { $_.Extension -in @(".pfx", ".p12", ".key", ".pem", ".p7b", ".p7c") })
if ($forbidden.Count -ne 0) {
    throw "Package contains forbidden credential material."
}

$manifestPath = Join-Path $packagePath "SHA256SUMS.txt"
$manifestLines = @(Get-Content -LiteralPath $manifestPath | Where-Object { -not [string]::IsNullOrWhiteSpace($_) })
if ($manifestLines.Count -ne 3) {
    throw "SHA-256 manifest must describe exactly the three payload files."
}
foreach ($line in $manifestLines) {
    if ($line -notmatch '^(?<hash>[A-Fa-f0-9]{64})  (?<name>[^\\/]+)$') {
        throw "Invalid SHA-256 manifest line: $line"
    }
    $payloadPath = Join-Path $packagePath $Matches.name
    if (-not (Test-Path -LiteralPath $payloadPath -PathType Leaf)) {
        throw "Manifest references a missing file: $($Matches.name)"
    }
    $actualHash = (Get-FileHash -LiteralPath $payloadPath -Algorithm SHA256).Hash
    if ($actualHash -ne $Matches.hash) {
        throw "SHA-256 mismatch for $($Matches.name)."
    }
}

$executablePath = Join-Path $packagePath "certradar.exe"
$stream = [IO.File]::OpenRead($executablePath)
$reader = [IO.BinaryReader]::new($stream)
try {
    if ($reader.ReadUInt16() -ne 0x5A4D) { throw "Executable does not have an MZ header." }
    $stream.Position = 0x3C
    $peOffset = $reader.ReadUInt32()
    $stream.Position = $peOffset
    if ($reader.ReadUInt32() -ne 0x00004550) { throw "Executable does not have a PE header." }
    $machine = $reader.ReadUInt16()
} finally {
    $reader.Dispose()
    $stream.Dispose()
}

$expectedMachine = if ($Architecture -eq "x64") { 0x8664 } else { 0x014C }
if ($machine -ne $expectedMachine) {
    throw ("Unexpected PE architecture: 0x{0:X4}." -f $machine)
}

$signature = Get-AuthenticodeSignature -LiteralPath $executablePath
if ($signature.Status -ne [System.Management.Automation.SignatureStatus]::NotSigned) {
    throw "The unsigned development package must not claim a different signature state: $($signature.Status)."
}

Write-Host "Package verification passed: $packagePath"
