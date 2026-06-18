#include "memoria.h"


// Inicializa a memoria principal, marcando todos os frames como livres.
void memoria_init(MemoriaPrincipal *mp) {

    for (unsigned i = 0; i < NUM_FRAMES; i++) 
    {
        mp->frames[i].ocupado = 0; 
        mp->frames[i].processo = -1; 
        mp->frames[i].pagina = -1;
        mp->frames[i].carregado_em = 0; 
        mp->frames[i].ultimo_acesso = 0; 
        mp->frames[i].bit_referencia = 0;
    }

}

/* retorna o indice do primeiro frame livre ou -1 se nao houver */
int memoria_primeiro_frame_livre(MemoriaPrincipal *mp) {

    for (int i = 0; i < (int)NUM_FRAMES; i++) 
        if (!mp->frames[i].ocupado) return i;

    return -1;
}
