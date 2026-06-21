# Build + roda o modo demo (multithread) com o algoritmo Clock.
$root = Split-Path $PSScriptRoot -Parent
& "$PSScriptRoot\build.ps1"
& "$root\simulador.exe" --modo demo --alg clock --processos 2 --acessos 12 --seed 1
