#ifndef MMU_H
#define MMU_H
#include <pthread.h>
#include "config.h"
#include "memoria.h"
#include "tabela_paginas.h"
#include "substituicao.h"
#include "estatisticas.h"

// A MMU mantém a memória principal, uma tabela de páginas por processo, o relógio
// lógico (pra FIFO/LRU), o ponteiro do Clock e as estatísticas. O mutex serializa
// os acessos quando há vários processos (threads).
typedef struct {
    MemoriaPrincipal mp;
    TabelaPaginas *tabelas;     // vetor [num_processos] 
    int num_processos;
    AlgoritmoSubstituicao alg;
    int ponteiro_clock;
    unsigned long relogio;
    Estatisticas stats;
    pthread_mutex_t mutex;
    int verbose;
} MMU;

void mmu_init(MMU *m, int num_processos, AlgoritmoSubstituicao alg, int verbose);
void mmu_destroy(MMU *m);
// Traduz e acessa um endereco virtual de um processo. Retorna 0 ok / -1 endereco invalido
int  mmu_acessar(MMU *m, int processo, uint32_t vaddr);
void mmu_imprime_estado(MMU *m);
void mmu_imprime_estatisticas(MMU *m);
#endif
