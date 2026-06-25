<h1 align="center">Simulador de Memória Virtual</h1>
<p align="center">
  <em>Paginação · MMU · Tradução de endereços · Falta de página · Substituição (FIFO / LRU / Clock)</em>
</p>

<p align="center">
  <img alt="Linguagem C" src="https://img.shields.io/badge/linguagem-C-00599C?logo=c">
  <img alt="Threads POSIX" src="https://img.shields.io/badge/concorrência-pthreads-555">
  <img alt="Plataformas" src="https://img.shields.io/badge/plataforma-Windows%20%7C%20Linux-2E7D32">
  <img alt="Licença MIT" src="https://img.shields.io/badge/licença-MIT-blue">
  <img alt="Trabalho GB" src="https://img.shields.io/badge/Unisinos-Trabalho%20GB-C0504D">
  <a href="https://youtu.be/D3vofVNypKw"><img alt="Vídeo da apresentação" src="https://img.shields.io/badge/v%C3%ADdeo-apresenta%C3%A7%C3%A3o-FF0000?logo=youtube&logoColor=white"></a>
</p>

<p align="center">
  🎥 <strong>Vídeo da apresentação (~7 min):</strong> <a href="https://youtu.be/D3vofVNypKw">youtu.be/D3vofVNypKw</a>
</p>

---

> **Disciplina:** Análise e Aplicação de Sistemas Operacionais
    
> **Aluno:** Taimisson de Carvalho Schardosim

> **Professor:** Prof. Dr. Cristiano Bonato Both

> **Instituição:** Universidade do Vale do Rio dos Sinos (Unisinos) — Escola Politécnica

> **Entrega:** Segundo Trabalho Prático (GB)

---

## Sumário

- [Descrição](#descrição)
- [Especificação do sistema](#especificação-do-sistema)
- [Como funciona (fluxo da MMU)](#como-funciona-fluxo-da-mmu)
- [Funcionalidades](#funcionalidades)
- [Requisitos](#requisitos)
- [Compilação e execução](#compilação-e-execução)
- [Opções de linha de comando](#opções-de-linha-de-comando)
- [Exemplos de saída](#exemplos-de-saída)
- [Resultados (FIFO × LRU × Clock)](#resultados-fifo--lru--clock)
- [Testes](#testes)
- [Estrutura do projeto](#estrutura-do-projeto)
- [Documentação](#documentação)
- [Mapeamento com a rubrica](#mapeamento-com-a-rubrica)
- [Referências](#referências)
- [Licença](#licença)

---

## Descrição

Este projeto é um **simulador de memória virtual com paginação**, escrito em **C** com
**threads POSIX** (`pthreads`). Ele reproduz o papel da **MMU** (*Memory Management Unit*):
recebe **endereços virtuais** gerados pelos processos, **traduz** para **endereços físicos**,
detecta e trata **faltas de página** (*page faults*) e, quando a memória principal está cheia,
aplica um **algoritmo de substituição** de páginas.

A simulação segue um sistema fictício definido no enunciado do Trabalho GB e exibe, na saída
padrão, cada operação: endereço gerado, falta de página, frame utilizado, substituição e o
conteúdo acessado.

## Especificação do sistema

Tudo é derivado dos três tamanhos do enunciado:

| Grandeza | Cálculo | Resultado |
| --- | --- | --- |
| Memória principal (física) | dado | **64 KB** |
| Memória virtual | dado | **1 MB** |
| Bloco (página = frame) | dado | **8 KB** |
| Número de **frames** | 64 KB ÷ 8 KB | **8** |
| Número de **páginas** | 1 MB ÷ 8 KB | **128** |
| Endereço **virtual** | 7 bits (página) + 13 bits (offset) | **20 bits** |
| Endereço **físico** | 3 bits (frame) + 13 bits (offset) | **16 bits** |

```
ENDEREÇO VIRTUAL (20 bits)            ENDEREÇO FÍSICO (16 bits)
┌────────────┬───────────────┐       ┌────────┬───────────────┐
│ página (7) │  offset (13)  │  -->  │frame(3)│  offset (13)  │
└────────────┴───────────────┘       └────────┴───────────────┘
        (a MMU troca a página pelo frame e o offset não muda)
```

## Como funciona (fluxo da MMU)

A cada acesso a um endereço virtual, a MMU executa:

1. **Separa** o endereço em número da página e *offset* (`página = vaddr >> 13`,
   `offset = vaddr & 0x1FFF`).
2. Se o endereço passa de 1 MB, é **inválido** (rejeitado).
3. Consulta a **tabela de páginas** do processo:
   - **Página presente** → *hit*: usa o frame e mostra o conteúdo.
   - **Página ausente** → **falta de página**:
     - **(a)** se há **frame livre**, o bloco é copiado para o primeiro frame disponível e a
       tabela é atualizada;
     - **(b)** se **não há frame livre**, o algoritmo escolhido seleciona uma **página vítima**,
       a entrada antiga é invalidada e a nova página é carregada no lugar.
4. Monta o **endereço físico** (`paddr = (frame << 13) | offset`) e exibe o conteúdo.

## Funcionalidades

- ✅ Modelagem da **memória virtual** (128 páginas) e da **memória principal** (8 frames).
- ✅ **Tabela de páginas por processo** (espaço de endereçamento isolado).
- ✅ **Tradução** de endereço virtual → físico pela MMU.
- ✅ Detecção e tratamento de **falta de página**.
- ✅ Carregamento em **frame livre** e **substituição** quando a memória enche.
- ✅ **Três algoritmos** de substituição: **FIFO**, **LRU** e **Clock** (segunda chance).
- ✅ **Processos leves** (threads) com **localidade de referência** (paradigma produtor/consumidor).
- ✅ **Modo comparação** que roda os três algoritmos sob a **mesma carga** e gera um CSV.
- ✅ **Testes unitários** do núcleo da MMU.

## Requisitos

- **GCC** com suporte a `pthread`:
  - **Windows:** [MinGW-w64](https://www.mingw-w64.org/) ou [MSYS2](https://www.msys2.org/).
  - **Linux/macOS:** GCC ou Clang.
- *(Opcional)* **Python 3** com **matplotlib**, apenas para gerar o gráfico das faltas.

## Compilação e execução

### Windows (PowerShell)

```powershell
.\scripts\build.ps1        # compila -> simulador.exe
.\scripts\demo.ps1         # modo demo (multithread, detalhado)
.\scripts\comparar.ps1     # FIFO x LRU x Clock -> reports\comparacao.csv
.\scripts\testar.ps1       # compila e roda os testes unitários
```

### Linux / macOS

```bash
make            # compila -> ./simulador
make test       # roda os testes unitários
```

### Compilação manual (qualquer sistema)

```bash
gcc -Wall -Wextra -O2 -pthread -Iinclude src/*.c -o simulador
```

## Opções de linha de comando

```
uso: simulador [--modo demo|comparacao] [--alg fifo|lru|clock]
               [--processos N] [--acessos K] [--paginas P] [--seed S]
```

| Opção | Descrição | Padrão |
| --- | --- | --- |
| `--modo` | `demo` (multithread, detalhado) ou `comparacao` (3 algoritmos) | `demo` |
| `--alg` | Algoritmo de substituição: `fifo`, `lru` ou `clock` | `clock` |
| `--processos` | Número de processos leves (1–64) | `2` |
| `--acessos` | Acessos gerados por processo | `12` |
| `--paginas` | Tamanho do *working set* de cada processo, em páginas (1–128) | `6` |
| `--seed` | Semente do gerador (torna o experimento reproduzível) | `1` |

## Exemplos de saída

**Modo demo** (cada acesso traduzido e atendido):

```
  [P1] vaddr=0x078BE -> pag=3   off=0x18BE | FALTA DE PAGINA -> carrega no frame livre 0   | paddr=0x18BE conteudo=0x6F 'o'
  [P1] vaddr=0x02DF1 -> pag=1   off=0x0DF1 | HIT  (pagina ja em frame 1)                   | paddr=0x2DF1 conteudo=0x61 'a'
  [P2] vaddr=0x04FF5 -> pag=2   off=0x0FF5 | FALTA DE PAGINA -> CLOCK substitui (P1 pag 1) no frame 1 | paddr=0x2FF5 ...
```

**Tabela final dos frames** e **estatísticas** também são impressas ao fim da execução.

## Resultados (FIFO × LRU × Clock)

Mesma carga (2 processos, 8 páginas por processo, 600 acessos, `--seed 7`):

| Algoritmo | Faltas | Substituições | Taxa de falta |
| --- | --- | --- | --- |
| FIFO | 202 | 194 | 33,7% |
| **LRU** | **170** | **162** | **28,3%** |
| Clock | 190 | 182 | 31,7% |

Como o *trace* tem **localidade de referência**, o **LRU** obtém a menor taxa de faltas; o
**Clock** fica próximo, com custo menor. Resultados completos em
[`reports/comparacao.csv`](reports/comparacao.csv).

> Os valores exatos podem variar conforme a implementação de `rand_r` da plataforma, mas a
> ordem (FIFO > Clock > LRU em número de faltas) se mantém.

*(Opcional)* gerar o gráfico de barras:

```bash
python tools/plot_faults.py     # gera reports/comparacao.png (requer matplotlib)
```

## Testes

Os testes unitários validam o núcleo do simulador, sem depender da saída no terminal:

```powershell
.\scripts\testar.ps1     # Windows
make test                # Linux/macOS
```

Eles cobrem: o **layout de endereços** (página/offset/físico), **falta na primeira referência
e hit na segunda**, **rejeição de endereço inválido**, **substituição com os 8 frames cheios**
e a **segunda chance do Clock**.

## Estrutura do projeto

```
simulador-memoria-virtual/
├── include/                 # cabeçalhos (contratos dos módulos)
│   ├── config.h             # tamanhos, número de frames/páginas e macros de bits
│   ├── memoria.h            # memória principal (frames)
│   ├── memoria_virtual.h    # backing store (conteúdo das páginas)
│   ├── tabela_paginas.h     # tabela de páginas por processo
│   ├── substituicao.h       # FIFO / LRU / Clock
│   ├── mmu.h                # a MMU
│   ├── processo.h           # processos leves e trace
│   └── estatisticas.h       # contadores
├── src/                     # implementações
│   ├── memoria.c
│   ├── memoria_virtual.c
│   ├── tabela_paginas.c
│   ├── substituicao.c
│   ├── mmu.c                # tradução, falta de página e carga página→frame
│   ├── processo.c           # threads + gerador de acessos com localidade
│   └── main.c               # linha de comando e modos demo/comparação
├── tests/test_mmu.c         # testes unitários
├── scripts/                 # build / demo / comparar / testar (PowerShell)
├── tools/plot_faults.py     # gráfico opcional das faltas
├── reports/                 # resultados de execução (CSV)
├── docs/                    # enunciado (Trabalho GB.pdf) e vídeo da apresentação
├── Makefile                 # build no Linux/macOS
└── README.md
```

## Documentação

- [`docs/Trabalho GB.pdf`](docs/Trabalho%20GB.pdf) — enunciado oficial do trabalho (definição do sistema fictício e rubrica de avaliação).
- **Vídeo da apresentação (~7 min):** [assista no YouTube](https://youtu.be/D3vofVNypKw) — também incluído na pasta [`docs/`](docs/) do repositório.

## Mapeamento com a rubrica

| Item da rubrica | Onde está no projeto |
| --- | --- |
| 2) Aderência ao enunciado | `config.h`, `banner()` |
| 3) Memória virtual (1 MB, páginas de 8 KB) | `config.h` (128 páginas) |
| 4) Memória principal (64 KB, frames) | `memoria.{h,c}` (8 frames) |
| 5) Tabela de páginas | `tabela_paginas.{h,c}` |
| 6) Tradução pela MMU | `mmu.c` → `mmu_acessar` |
| 7) Detecção/tratamento de falta de página | `mmu.c` (caminho de falta) |
| 8) Carregamento de páginas | `memoria_primeiro_frame_livre` + carga + atualização da tabela |
| 9) Algoritmo de substituição | `substituicao.c` (FIFO/LRU/Clock) |
| 10) Visualização dos resultados | saída de `mmu_acessar`, `mmu_imprime_*` e `reports/comparacao.csv` |

## Referências

- BOTH, Cristiano Bonato. **Definição do Segundo Trabalho Prático — Trabalho GB.** Unisinos, 2026.
- Projeto de referência indicado no enunciado: **multi-lora** — <https://github.com/luciorp/multi-lora>.
- TANENBAUM, Andrew S.; BOS, Herbert. **Modern Operating Systems.** 4ª ed. Pearson, 2015.
- SILBERSCHATZ, A.; GALVIN, P. B.; GAGNE, G. **Operating System Concepts.** Wiley.

## Licença

Distribuído sob a licença **MIT**. Veja [`LICENSE`](LICENSE).
