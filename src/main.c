#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "config.h"
#include "mmu.h"
#include "processo.h"

static void banner(void) {
    printf("============================================================\n");
    printf(" Simulador de Memoria Virtual / Paginacao (MMU) - Trabalho GB\n");
    printf("============================================================\n");
    printf(" Memoria principal : %u KB  -> %u frames de %u KB\n",
           TAM_MEMORIA_PRINCIPAL/1024u, NUM_FRAMES, TAM_BLOCO/1024u);
    printf(" Memoria virtual   : %u KB  -> %u paginas de %u KB\n",
           TAM_MEMORIA_VIRTUAL/1024u, NUM_PAGINAS, TAM_BLOCO/1024u);
    printf(" Endereco virtual  : 20 bits | fisico: 16 bits | offset: %d bits\n", BITS_OFFSET);
    printf(" Layout vaddr      : [ pagina (7 bits) | offset (13 bits) ]\n");
    printf("============================================================\n");
}

static void modo_demo(int num_proc, int acessos, int paginas, unsigned int seed, AlgoritmoSubstituicao alg) {
    printf("\n>> MODO DEMO (multithread) | algoritmo de substituicao: %s\n", alg_nome(alg));
    printf(">> %d processos leves (threads), %d acessos cada, %d paginas/processo\n\n", num_proc, acessos, paginas);

    MMU m;
    mmu_init(&m, num_proc, alg, 1);

    pthread_t th[64];
    Processo pr[64];
    for (int i = 0; i < num_proc; i++) 
    {
        pr[i].id = i;
        pr[i].paginas_validas = paginas;
        pr[i].num_acessos = acessos;
        pr[i].seed = seed + (unsigned)i * 7919u;
        pr[i].mmu = &m;
        pthread_create(&th[i], NULL, processo_thread, &pr[i]);
    }

    for (int i = 0; i < num_proc; i++)
        pthread_join(th[i], NULL);

    mmu_imprime_estado(&m);
    mmu_imprime_estatisticas(&m);
    mmu_destroy(&m);
}

static void modo_comparacao(int num_proc, int acessos, int paginas, unsigned int seed) {

    int max = num_proc * acessos + 8;

    Acesso *trace = malloc(sizeof(Acesso) * (size_t)max);

    int n = gerar_trace(trace, max, num_proc, acessos, paginas, seed);

    printf("\n>> MODO COMPARACAO | mesma carga (%d acessos) nos 3 algoritmos\n", n);
    printf(">> %d processos, %d paginas/processo competindo por %u frames\n", num_proc, paginas, NUM_FRAMES);

    AlgoritmoSubstituicao algs[3] = 
    { 
        ALG_FIFO, 
        ALG_LRU, 
        ALG_CLOCK 
    };

    unsigned long faltas[3];
    unsigned long subs[3];

    for (int a = 0; a < 3; a++) 
    {
        MMU m;

        mmu_init(&m, num_proc, algs[a], 0);

        for (int i = 0; i < n; i++)

            mmu_acessar(&m, trace[i].processo, trace[i].vaddr);

        faltas[a] = m.stats.faltas;
        subs[a] = m.stats.substituicoes;

        mmu_destroy(&m);
    }

    printf("\n  +-----------+----------+----------------+----------------+\n");
    printf("  | algoritmo |  faltas  | substituicoes  | taxa de falta  |\n");
    printf("  +-----------+----------+----------------+----------------+\n");
    for (int a = 0; a < 3; a++)
        printf("  | %-9s | %6lu   |     %6lu     |     %5.1f%%     |\n", alg_nome(algs[a]), faltas[a], subs[a], 100.0 * (double)faltas[a] / (double)n);
    printf("  +-----------+----------+----------------+----------------+\n");

    FILE *f = fopen("reports/comparacao.csv", "w");
    if (!f)
        f = fopen("comparacao.csv", "w");
    if (f) 
    {
        fprintf(f, "algoritmo,acessos,faltas,substituicoes,taxa_falta_pct\n");
        for (int a = 0; a < 3; a++)
            fprintf(f, "%s,%d,%lu,%lu,%.2f\n", alg_nome(algs[a]), n, faltas[a], subs[a], 100.0 * (double)faltas[a] / (double)n);
        fclose(f);
        printf("\n  CSV salvo em reports/comparacao.csv\n");
    }
    free(trace);
}

int main(int argc, char **argv) {
    int num_proc = 2;
    int acessos = 12;
    int paginas = 6;
    unsigned int seed = 1;
    const char *modo = "demo";
    AlgoritmoSubstituicao alg = ALG_CLOCK;

    for (int i = 1; i < argc; i++) 
    {
        if (!strcmp(argv[i], "--modo") && i + 1 < argc) 
        {
            modo = argv[++i];
        } else if (!strcmp(argv[i], "--alg") && i + 1 < argc) 
        {
            if (alg_por_nome(argv[++i], &alg)) 
            {
                fprintf(stderr, "algoritmo invalido\n");
                return 1;
            }
        } else if (!strcmp(argv[i], "--processos") && i + 1 < argc) 
        {
            num_proc = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--acessos") && i + 1 < argc) 
        {
            acessos = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--paginas") && i + 1 < argc) 
        {
            paginas = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--seed") && i + 1 < argc) 
        {
            seed = (unsigned)strtoul(argv[++i], NULL, 10);
        } else if (!strcmp(argv[i], "--help")) 
        {
            printf("uso: %s [--modo demo|comparacao] [--alg fifo|lru|clock]\n"
                   "          [--processos N] [--acessos K] [--paginas P] [--seed S]\n", argv[0]);
            return 0;
        }
    }

    if (num_proc < 1)
        num_proc = 1;
    if (num_proc > 64)
        num_proc = 64;
    if (paginas < 1)
        paginas = 1;
    if (paginas > (int)NUM_PAGINAS)
        paginas = (int)NUM_PAGINAS;

    banner();
    if (!strcmp(modo, "comparacao"))
        modo_comparacao(num_proc, acessos, paginas, seed);
    else
        modo_demo(num_proc, acessos, paginas, seed, alg);
    return 0;
}
