# Build + compara FIFO x LRU x Clock sob a mesma carga; gera reports\comparacao.csv.
$root = Split-Path $PSScriptRoot -Parent
& "$PSScriptRoot\build.ps1"
New-Item -ItemType Directory -Force -Path "$root\reports" | Out-Null
& "$root\simulador.exe" --modo comparacao --processos 2 --paginas 8 --acessos 300 --seed 7
