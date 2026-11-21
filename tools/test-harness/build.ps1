# Builds the mod outside of Windhawk, both as a DLL (to check that it compiles
# exactly like Windhawk compiles it) and as a standalone test executable.
#
# Usage:
#   .\tools\test-harness\build.ps1            # compile checks only
#   .\tools\test-harness\build.ps1 -Run       # also run the harness
#
# The harness enumerates your displays, opens the panel, triggers the
# on-screen indicator with a "+0%" hotkey (so brightness does not change),
# saves PNG renders into tools\test-harness\out and exits.

param(
    [switch]$Run,
    [string]$WindhawkDir = "$env:ProgramFiles\Windhawk"
)

$ErrorActionPreference = 'Stop'

$repo = Resolve-Path "$PSScriptRoot\..\.."
$source = Join-Path $repo 'twinkle-brightness.wh.cpp'
$out = Join-Path $PSScriptRoot 'out'
New-Item -ItemType Directory -Force $out | Out-Null

$clang = Join-Path $WindhawkDir 'Compiler\bin\clang++.exe'
if (-not (Test-Path $clang)) {
    throw "Windhawk compiler not found at $clang"
}
$engine = Get-ChildItem (Join-Path $WindhawkDir 'Engine') -Directory |
    Sort-Object { [version]$_.Name } | Select-Object -Last 1
$windhawkLib = Join-Path $engine.FullName '64\windhawk.lib'

$header = Get-Content $source -TotalCount 20
$version = ($header | Select-String '@version\s+(\S+)').Matches[0].Groups[1].Value
$libs = (($header | Select-String '@compilerOptions\s+(.+)$').Matches[0].Groups[1].Value).Trim() -split '\s+'

# Arguments that contain quotes go through a clang response file, because
# PowerShell versions differ in how they pass embedded quotes to programs.
function Invoke-Clang([string[]]$arguments) {
    $rsp = Join-Path $out 'args.rsp'
    $lines = $arguments | ForEach-Object {
        '"' + ($_.Replace('\', '/').Replace('"', '\"')) + '"'
    }
    [System.IO.File]::WriteAllLines($rsp, $lines)
    & $clang "@$rsp"
    return $LASTEXITCODE
}

$common = @(
    '-std=c++23', '-O2', '-DUNICODE', '-D_UNICODE',
    '-DWINVER=0x0A00', '-D_WIN32_WINNT=0x0A00', '-D_WIN32_IE=0x0A00',
    '-DNTDDI_VERSION=0x0A000008', '-D__USE_MINGW_ANSI_STDIO=0',
    '-DWH_MOD_ID=L"twinkle-brightness"', "-DWH_MOD_VERSION=L`"$version`"",
    '-target', 'x86_64-w64-mingw32', '-Wall'
)

Write-Host "Compiling the mod DLL (as Windhawk does)..."
$exit = Invoke-Clang ($common + @('-shared', '-DWH_MOD', $windhawkLib,
    '-x', 'c++', $source, '-include', 'windhawk_api.h',
    '-Wl,--export-all-symbols',
    '-o', (Join-Path $out 'twinkle-brightness.dll')) + $libs)
if ($exit -ne 0) { throw 'Mod compilation failed' }

Write-Host "Compiling the test harness..."
$exit = Invoke-Clang ($common + @('-municode', '-static',
    "-DMOD_SOURCE=`"$($source.Replace('\', '/'))`"",
    "-I$PSScriptRoot", (Join-Path $PSScriptRoot 'harness.cpp'),
    '-o', (Join-Path $out 'harness.exe')) + $libs)
if ($exit -ne 0) { throw 'Harness compilation failed' }

Write-Host "OK"

if ($Run) {
    Push-Location $out
    try {
        & .\harness.exe dark dark
        & .\harness.exe light light
    } finally {
        Pop-Location
    }
}
