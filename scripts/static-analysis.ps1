[CmdletBinding()]
param(
    [string]$VsDevCmd,
    [string]$CMake,
    [string]$Ninja,
    [string]$ClangTidy
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($VsDevCmd)) {
    $VsDevCmd = "C:\BuildTools\Common7\Tools\VsDevCmd.bat"
}
if ([string]::IsNullOrWhiteSpace($CMake)) {
    $command = Get-Command cmake.exe -ErrorAction SilentlyContinue
    $CMake = if ($null -ne $command) { $command.Source } else {
        "C:\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    }
}
if ([string]::IsNullOrWhiteSpace($Ninja)) {
    $command = Get-Command ninja.exe -ErrorAction SilentlyContinue
    $Ninja = if ($null -ne $command) { $command.Source } else {
        "C:\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
    }
}
if ([string]::IsNullOrWhiteSpace($ClangTidy)) {
    $command = Get-Command clang-tidy.exe -ErrorAction SilentlyContinue
    $ClangTidy = if ($null -ne $command) { $command.Source } else {
        "C:\BuildTools\VC\Tools\Llvm\x64\bin\clang-tidy.exe"
    }
}
foreach ($requiredTool in @($VsDevCmd, $CMake, $Ninja, $ClangTidy)) {
    if (-not (Test-Path -LiteralPath $requiredTool -PathType Leaf)) {
        throw "Static analysis dependency was not found: $requiredTool"
    }
}

$environmentLines = & cmd.exe /d /s /c "`"$VsDevCmd`" -arch=x64 -host_arch=x64 >nul && set"
if ($LASTEXITCODE -ne 0) { throw "Visual Studio build environment could not be initialized." }
foreach ($line in $environmentLines) {
    $separator = $line.IndexOf('=')
    if ($separator -gt 0) {
        [Environment]::SetEnvironmentVariable(
            $line.Substring(0, $separator), $line.Substring($separator + 1), "Process")
    }
}

$buildDirectory = Join-Path $repoRoot "build\static-analysis"
& $CMake -S $repoRoot -B $buildDirectory -G Ninja `
    -DCMAKE_BUILD_TYPE=Debug `
    "-DCMAKE_MAKE_PROGRAM=$Ninja" `
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if ($LASTEXITCODE -ne 0) { throw "Static analysis compilation database generation failed." }

$sources = @(Get-ChildItem -LiteralPath (Join-Path $repoRoot "src") -Filter "*.cpp" -Recurse |
    Sort-Object FullName | ForEach-Object FullName)
if ($sources.Count -eq 0) { throw "No modern C++ sources were found for static analysis." }

$checks = "-*,clang-analyzer-*,bugprone-*,performance-*,-bugprone-easily-swappable-parameters,-clang-analyzer-optin.core.EnumCastOutOfRange,-performance-no-int-to-ptr"
$errors = "clang-analyzer-*,bugprone-*"
& $ClangTidy @sources -p $buildDirectory "-checks=$checks" "--warnings-as-errors=$errors"
if ($LASTEXITCODE -ne 0) { throw "Clang static analysis reported an actionable finding." }

Write-Host "Clang static analysis passed for $($sources.Count) modern source files."
