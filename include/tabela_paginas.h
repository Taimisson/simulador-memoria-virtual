#ifndef TABELA_PAGINAS_H
#define TABELA_PAGINAS_H
#include "config.h"

// Uma entrada por pagina virtual e cada processo tem a sua tabela
typedef struct {
    int valida;   // 1 = pagina tá em um frame da memoria principal
    int frame;    // qual frame (0..NUM_FRAMES-1) quando valida
} EntradaTabela;

typedef struct {
    EntradaTabela entradas[NUM_PAGINAS];
} TabelaPaginas;

void tabela_init(TabelaPaginas *t);

#endif
