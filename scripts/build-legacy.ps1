[CmdletBinding()]
param(
    [string]$CompilerPath
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($CompilerPath)) {
    $compilerCommand = Get-Command g++.exe -ErrorAction SilentlyContinue
    if ($null -ne $compilerCommand) {
        $CompilerPath = $compilerCommand.Source
    } elseif (Test-Path -LiteralPath 'A:\bin\g++.exe' -PathType Leaf) {
        $CompilerPath = 'A:\bin\g++.exe'
    } else {
        throw "A 32-bit MinGW g++ compiler was not found. Pass -CompilerPath explicitly."
    }
}
$compiler = [IO.Path]::GetFullPath($CompilerPath)
if (-not (Test-Path -LiteralPath $compiler -PathType Leaf)) {
    throw "Legacy compiler does not exist: $compiler"
}

$target = (& $compiler -dumpmachine | Select-Object -First 1).Trim()
if ($target -notmatch 'mingw32') {
    throw "Legacy compiler is not a 32-bit MinGW target: $target"
}

$buildDirectory = Join-Path $repoRoot "build\legacy"
$sourcePath = Join-Path $repoRoot "legacy\main.cpp"
$executablePath = Join-Path $buildDirectory "certradar-legacy.exe"
New-Item -ItemType Directory -Path $buildDirectory -Force | Out-Null
if (Test-Path -LiteralPath $executablePath -PathType Leaf) {
    Remove-Item -LiteralPath $executablePath -Force
}

$arguments = @(
    "-std=gnu++98", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-Werror",
    "-DWINVER=0x0501", "-D_WIN32_WINNT=0x0501", "-march=i586",
    "-static", "-static-libgcc", "-static-libstdc++",
    "-Wl,--subsystem,console:5.01",
    $sourcePath, "-o", $executablePath, "-lshell32"
)
& $compiler @arguments
if (-not (Test-Path -LiteralPath $executablePath -PathType Leaf)) {
    throw "Legacy build failed and did not create the expected executable."
}

Write-Host "Legacy executable created: $executablePath"
