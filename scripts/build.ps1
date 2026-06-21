# Compila o simulador. Requer gcc no PATH (MinGW-w64 ou MSYS2).
$ErrorActionPreference = "Stop"
$root = Split-Path $PSScriptRoot -Parent
$src  = Get-ChildItem "$root\src" -Filter *.c | ForEach-Object { $_.FullName }
gcc -Wall -Wextra -O2 -pthread -I"$root\include" $src -o "$root\simulador.exe"
Write-Host "OK -> simulador.exe"
