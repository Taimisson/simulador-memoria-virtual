#ifndef MEMORIA_H
#define MEMORIA_H
#include "config.h"

/* Um frame da memoria principal: guarda uma copia do bloco + mapeamento reverso + metadados dos algoritmos de substituição. */
typedef struct {
    int ocupado; /* 0 livre, 1 ocupado */
    int processo; /* dono (-1 se livre) */
    int pagina; /* pagina virtual do dono */
    unsigned char dados[TAM_BLOCO]; /* bloco copiado da mem. virtual */
    unsigned long carregado_em; /* FIFO  : instante da carga */
    unsigned long ultimo_acesso; /* LRU   : instante do ult. uso */
    int bit_referencia; /* CLOCK : bit R */
} Frame;

typedef struct {
    Frame frames[NUM_FRAMES];
} MemoriaPrincipal;

void memoria_init(MemoriaPrincipal *mp);
int  memoria_primeiro_frame_livre(MemoriaPrincipal *mp); /* -1 se nao houver */
#endif
