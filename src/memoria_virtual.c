#include "memoria_virtual.h"
#include "config.h"
#include <stdio.h>

/* Cada pagina contem um rotulo legivel ("[P1 pag=003] ...") pelo bloco, para que o conteudo exibido identifique de qual processo/pagina veio. */
unsigned char mv_le_byte(int processo, int pagina, int offset) {
    char rotulo[40];
    int n = snprintf(rotulo, sizeof rotulo, "[P%d pag=%03d] conteudo ", processo + 1, pagina);
    if (n <= 0) return '?';
    return (unsigned char) rotulo[offset % n];
}
