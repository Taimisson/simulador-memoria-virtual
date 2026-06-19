#include "tabela_paginas.h"

void tabela_init(TabelaPaginas *t) {

    // inicializa a tabela de paginas e marca todas as entradas como invalidas
    for (unsigned i = 0; i < NUM_PAGINAS; i++) {
        t->entradas[i].valida = 0;
        t->entradas[i].frame = -1;
    }
}
