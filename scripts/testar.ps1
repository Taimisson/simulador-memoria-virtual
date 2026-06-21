# Compila e roda os testes unitarios.
$ErrorActionPreference = "Stop"
$root = Split-Path $PSScriptRoot -Parent
$lib  = Get-ChildItem "$root\src" -Filter *.c | Where-Object { $_.Name -ne "main.c" } | ForEach-Object { $_.FullName }
gcc -Wall -Wextra -O2 -pthread -I"$root\include" "$root\tests\test_mmu.c" $lib -o "$root\test_mmu.exe"
& "$root\test_mmu.exe"
