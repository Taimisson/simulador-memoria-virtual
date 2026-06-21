#ifndef PROCESSO_H
#define PROCESSO_H
#include "config.h"
#include "mmu.h"

// Processo leve: uma thread que gera uma sequencia de acessos a enderecos virtuais dentro do seu "working set" (paginas_validas), com localidade
typedef struct {
    int id;
    int paginas_validas;   // tamanho do processo em paginas (1..128)
    int num_acessos;
    unsigned int seed;
    MMU *mmu;
} Processo;

void *processo_thread(void *arg);

// Trace deterministico (para comparar algoritmos sob a MESMA carga).
typedef struct {
    int processo;
    uint32_t vaddr;
} Acesso;

int gerar_trace(Acesso *out, int max, int num_proc, int acessos_por_proc, int paginas_por_proc, unsigned int seed);

#endif
