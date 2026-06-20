#ifndef SUBSTITUICAO_H
#define SUBSTITUICAO_H
#include "memoria.h"

typedef enum { 
    ALG_FIFO, 
    ALG_LRU, 
    ALG_CLOCK 
} AlgoritmoSubstituicao;

const char *alg_nome(AlgoritmoSubstituicao alg);
int  alg_por_nome(const char *nome, AlgoritmoSubstituicao *out);

// escolhe o frame vitima (todos estao ocupados) e ponteiro_clock guarda o estado do ponteiro do algoritmo Clock entre chamadas. 
int escolher_vitima(MemoriaPrincipal *mp, AlgoritmoSubstituicao alg, int *ponteiro_clock);
#endif
