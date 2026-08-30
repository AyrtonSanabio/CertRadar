[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$ExecutablePath
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$path = [IO.Path]::GetFullPath($ExecutablePath)
if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
    throw "Legacy executable does not exist: $path"
}

$bytes = [IO.File]::ReadAllBytes($path)

function Read-UInt16At([int]$Offset) {
    if ($Offset -lt 0 -or $Offset + 2 -gt $bytes.Length) { throw "Invalid PE offset." }
    return [BitConverter]::ToUInt16($bytes, $Offset)
}

function Read-UInt32At([int]$Offset) {
    if ($Offset -lt 0 -or $Offset + 4 -gt $bytes.Length) { throw "Invalid PE offset." }
    return [BitConverter]::ToUInt32($bytes, $Offset)
}

function Convert-RvaToOffset([uint32]$Rva, $Sections) {
    foreach ($section in $Sections) {
        $extent = [Math]::Max($section.VirtualSize, $section.RawSize)
        if ($Rva -ge $section.VirtualAddress -and $Rva -lt $section.VirtualAddress + $extent) {
            return [int]($section.RawPointer + ($Rva - $section.VirtualAddress))
        }
    }
    throw ("PE RVA 0x{0:X8} is not mapped by a section." -f $Rva)
}

function Read-AsciiZeroTerminated([int]$Offset) {
    $end = $Offset
    while ($end -lt $bytes.Length -and $bytes[$end] -ne 0) { ++$end }
    if ($end -ge $bytes.Length) { throw "Unterminated PE import name." }
    return [Text.Encoding]::ASCII.GetString($bytes, $Offset, $end - $Offset)
}

$stream = [IO.MemoryStream]::new($bytes, $false)
$reader = [IO.BinaryReader]::new($stream)
try {
    if ($reader.ReadUInt16() -ne 0x5A4D) { throw "Legacy executable has no MZ header." }
    $stream.Position = 0x3C
    $peOffset = $reader.ReadUInt32()
    $stream.Position = $peOffset
    if ($reader.ReadUInt32() -ne 0x00004550) { throw "Legacy executable has no PE header." }
    $machine = $reader.ReadUInt16()
    $numberOfSections = $reader.ReadUInt16()
    $stream.Position = $peOffset + 20
    $optionalHeaderSize = $reader.ReadUInt16()
    $stream.Position = $peOffset + 24
    $optionalMagic = $reader.ReadUInt16()
    $stream.Position = $peOffset + 24 + 48
    $majorSubsystem = $reader.ReadUInt16()
    $minorSubsystem = $reader.ReadUInt16()
} finally {
    $reader.Dispose()
    $stream.Dispose()
}

if ($machine -ne 0x014C -or $optionalMagic -ne 0x010B) {
    throw "Legacy executable must be a 32-bit PE image."
}
if ($majorSubsystem -ne 5 -or $minorSubsystem -ne 1) {
    throw "Legacy executable must target Windows subsystem 5.01, found $majorSubsystem.$minorSubsystem."
}

$optionalHeaderOffset = $peOffset + 24
$importDirectoryRva = Read-UInt32At ($optionalHeaderOffset + 104)
$sectionTableOffset = $optionalHeaderOffset + $optionalHeaderSize
$sections = @()
for ($index = 0; $index -lt $numberOfSections; ++$index) {
    $sectionOffset = $sectionTableOffset + ($index * 40)
    $sections += [pscustomobject]@{
        VirtualSize = Read-UInt32At ($sectionOffset + 8)
        VirtualAddress = Read-UInt32At ($sectionOffset + 12)
        RawSize = Read-UInt32At ($sectionOffset + 16)
        RawPointer = Read-UInt32At ($sectionOffset + 20)
    }
}

$imports = @()
if ($importDirectoryRva -ne 0) {
    $descriptorOffset = Convert-RvaToOffset $importDirectoryRva $sections
    while ($true) {
        $originalThunk = Read-UInt32At $descriptorOffset
        $nameRva = Read-UInt32At ($descriptorOffset + 12)
        $firstThunk = Read-UInt32At ($descriptorOffset + 16)
        if ($originalThunk -eq 0 -and $nameRva -eq 0 -and $firstThunk -eq 0) { break }
        $imports += Read-AsciiZeroTerminated (Convert-RvaToOffset $nameRva $sections)
        $descriptorOffset += 20
    }
}

foreach ($runtimeName in @("libstdc++-6.dll", "libgcc_s_dw2-1.dll", "mingwm10.dll")) {
    if (@($imports | Where-Object { $_.Equals($runtimeName, [StringComparison]::OrdinalIgnoreCase) }).Count -ne 0) {
        throw "Legacy executable unexpectedly depends on $runtimeName."
    }
}

Write-Host "Legacy PE verification passed: x86, subsystem 5.01, imports $($imports -join ', ')."
