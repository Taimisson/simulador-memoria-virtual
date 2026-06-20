#include "substituicao.h"
#include <string.h>

const char *alg_nome(AlgoritmoSubstituicao alg) {
    switch (alg) {
        case ALG_FIFO:  return "FIFO";
        case ALG_LRU:   return "LRU";
        case ALG_CLOCK: return "CLOCK";
    }
    return "?";
}

int alg_por_nome(const char *nome, AlgoritmoSubstituicao *out) {
    // comparação case-insensitive para aceitar variações de maiúsculas e minúsculas.
    if (!strcmp(nome, "fifo") || !strcmp(nome, "FIFO")) {
        *out = ALG_FIFO;
        return 0;
    }
    if (!strcmp(nome, "lru") || !strcmp(nome, "LRU")) {
        *out = ALG_LRU;
        return 0;
    }
    if (!strcmp(nome, "clock") || !strcmp(nome, "CLOCK")) {
        *out = ALG_CLOCK;
        return 0;
    }
    return -1;
}

// Escolhe o frame vitima quando todos estao ocupados
int escolher_vitima(MemoriaPrincipal *mp, AlgoritmoSubstituicao alg, int *ponteiro_clock) {
    int idx = 0;

    // FIFO: substitui a pagina com menor instante de carga (a mais antiga).
    if (alg == ALG_FIFO) {
        unsigned long melhor = mp->frames[0].carregado_em;
        for (int i = 1; i < (int)NUM_FRAMES; i++) {
            if (mp->frames[i].carregado_em < melhor) {
                melhor = mp->frames[i].carregado_em;
                idx = i;
            }
        }
        return idx;
    }

    // LRU: substitui a pagina com menor instante de ultimo acesso.
    if (alg == ALG_LRU) {
        unsigned long melhor = mp->frames[0].ultimo_acesso;
        for (int i = 1; i < (int)NUM_FRAMES; i++) {
            if (mp->frames[i].ultimo_acesso < melhor) {
                melhor = mp->frames[i].ultimo_acesso;
                idx = i;
            }
        }
        return idx;
    }

    // CLOCK vai avancar o ponteiro, bit R == 0 -> vitima, senao zera o bit (concede a segunda chance) e segue para o proximo frame.
    for (;;) {
        int p = *ponteiro_clock;
        if (mp->frames[p].bit_referencia == 0) 
        {
            *ponteiro_clock = (p + 1) % NUM_FRAMES;
            return p;
        }
        mp->frames[p].bit_referencia = 0;
        *ponteiro_clock = (p + 1) % NUM_FRAMES;
    }
}
