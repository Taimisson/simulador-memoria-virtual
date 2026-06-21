/* rand_r e POSIX: precisa do feature-test macro para ser declarada no MinGW/GCC 14
 * (sem isso, a declaracao implicita vira ERRO e o build falha no Windows). */
#define _POSIX_C_SOURCE 200112L
#include "processo.h"
#include <stdlib.h>

/* gera um endereco com localidade 80% reusa uma pagina "quente", 20% espalha */
static uint32_t gera_vaddr(unsigned int *s, int paginas) {

    int pag; // pagina virtual a ser acessada, entre 0 e paginas_validas-1

    if ((rand_r(s) % 100) < 80)
        pag = rand_r(s) % (paginas < 4 ? paginas : 4);   /* paginas quentes */
    else
        pag = rand_r(s) % paginas;

    uint32_t off = (uint32_t)(rand_r(s) % TAM_BLOCO);

    return ((uint32_t)pag << BITS_OFFSET) | off;
}

void *processo_thread(void *arg) {
    Processo *p = (Processo *)arg;
    unsigned int s = p->seed; 
    for (int i = 0; i < p->num_acessos; i++)
        mmu_acessar(p->mmu, p->id, gera_vaddr(&s, p->paginas_validas));
    return NULL;
}

// retorna o numero de acessos gerados
int gerar_trace(Acesso *out, int max, int num_proc, int acessos_por_proc, int paginas_por_proc, unsigned int seed) {

    unsigned int s[64]; // é 64 pq é o numero maximo de processos q o simulador suporte 

    for (int i = 0; i < num_proc && i < 64; i++)
        s[i] = seed + (unsigned)i * 7919u;

    int n = 0;

    for (int k = 0; k < acessos_por_proc; k++)
        for (int proc = 0; proc < num_proc && n < max; proc++) 
        {
            out[n].processo = proc;
            out[n].vaddr = gera_vaddr(&s[proc], paginas_por_proc);
            n++;
        }

    return n;
}
