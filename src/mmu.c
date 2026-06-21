#include "mmu.h"
#include "memoria_virtual.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Inicializa a MMU: zera a memória principal, cria uma tabela de páginas para cada
// processo, define o algoritmo de substituição e prepara o mutex e as estatísticas.
void mmu_init(MMU *m, int num_processos, AlgoritmoSubstituicao alg, int verbose) {

    memoria_init(&m->mp); // aq vai zerar os frames da memória principal

    m->num_processos = num_processos;
    m->tabelas = calloc((size_t)num_processos, sizeof(TabelaPaginas));

    for (int i = 0; i < num_processos; i++)
        tabela_init(&m->tabelas[i]);

    m->alg = alg;
    m->ponteiro_clock = 0;
    m->relogio = 0;
    m->verbose = verbose;

    Estatisticas z = {0, 0, 0, 0, 0};
    m->stats = z;

    pthread_mutex_init(&m->mutex, NULL);
}

// Libera a memória das tabelas de páginas e destrói o mutex.
void mmu_destroy(MMU *m) {
    free(m->tabelas);
    m->tabelas = NULL;
    pthread_mutex_destroy(&m->mutex);
}

// Devolve o caractere se ele for imprimível, senão um ponto (pra não sujar a saída).
static char imprimivel(unsigned char c) {
    return isprint(c) ? (char)c : '.';
}

// Traduz um endereço virtual de um processo e acessa o conteúdo.
// Retorna 0 em caso de sucesso e -1 se o endereço estiver fora da memória virtual.
int mmu_acessar(MMU *m, int processo, uint32_t vaddr) {
    // separa o endereço virtual em número da página e offset dentro da página
    uint32_t pagina = PAGINA_DO_ENDERECO(vaddr);
    uint32_t offset = OFFSET_DO_ENDERECO(vaddr);

    // se o endereço passa de 1 MB ele não existe na memória virtual -> rejeita
    if (vaddr >= TAM_MEMORIA_VIRTUAL || pagina >= NUM_PAGINAS) {
        if (m->verbose)
            printf("  [P%d] ERRO: endereco virtual 0x%05X fora da memoria virtual (1 MB)\n",
                   processo + 1, (unsigned)vaddr);
        return -1;
    }

    // a MMU é compartilhada pelas threads, então travo aqui pra manter tudo coerente
    pthread_mutex_lock(&m->mutex);
    m->relogio++;
    m->stats.acessos++;
    EntradaTabela *e = &m->tabelas[processo].entradas[pagina];

    int frame;
    char evento[96];

    if (e->valida) {
        // HIT: a página já está carregada em algum frame, é só usar
        frame = e->frame;
        m->stats.hits++;
        snprintf(evento, sizeof evento, "HIT  (pagina ja em frame %d)", frame);
    } else {
        // FALTA DE PÁGINA: a página não está na memória principal
        m->stats.faltas++;
        int livre = memoria_primeiro_frame_livre(&m->mp);
        if (livre >= 0) {
            // (a) ainda tem frame livre, então é só carregar nele
            frame = livre;
            m->stats.carregou_frame_livre++;
            snprintf(evento, sizeof evento, "FALTA DE PAGINA -> carrega no frame livre %d", frame);
        } else {
            // (b) memória cheia: escolho uma vítima e invalido a página antiga dela
            frame = escolher_vitima(&m->mp, m->alg, &m->ponteiro_clock);
            Frame *vf = &m->mp.frames[frame];
            int vp = vf->processo;
            int vpag = vf->pagina;
            m->tabelas[vp].entradas[vpag].valida = 0;   // a página antiga sai da memória
            m->stats.substituicoes++;
            snprintf(evento, sizeof evento,
                     "FALTA DE PAGINA -> %s substitui (P%d pag %d) no frame %d",
                     alg_nome(m->alg), vp + 1, vpag, frame);
        }
        // copia o bloco do "disco" (memória virtual) pro frame e atualiza a tabela
        Frame *f = &m->mp.frames[frame];
        for (unsigned o = 0; o < TAM_BLOCO; o++)
            f->dados[o] = mv_le_byte(processo, (int)pagina, (int)o);
        f->ocupado = 1;
        f->processo = processo;
        f->pagina = (int)pagina;
        f->carregado_em = m->relogio;
        e->valida = 1;
        e->frame = frame;
    }

    Frame *f = &m->mp.frames[frame];
    f->ultimo_acesso = m->relogio;          // marca o último uso (usado pelo LRU)
    f->bit_referencia = 1;                  // liga o bit de referência (usado pelo Clock)
    uint32_t paddr = ENDERECO_FISICO((uint32_t)frame, offset);  // monta o endereço físico
    unsigned char conteudo = f->dados[offset];

    // mostra na saída o que aconteceu: endereço virtual, evento, endereço físico e conteúdo
    if (m->verbose)
        printf("  [P%d] vaddr=0x%05X -> pag=%-3u off=0x%04X | %-52s | paddr=0x%04X conteudo=0x%02X '%c'\n",
               processo + 1, (unsigned)vaddr, (unsigned)pagina, (unsigned)offset,
               evento, (unsigned)paddr, conteudo, imprimivel(conteudo));

    pthread_mutex_unlock(&m->mutex);
    return 0;
}

// Imprime o estado atual da memória principal: o que está em cada um dos 8 frames.
void mmu_imprime_estado(MMU *m) {
    printf("\n  Memoria principal (8 frames de 8 KB):\n");
    printf("  +-------+----------+--------+------+-----------+--------------+\n");
    printf("  | frame | processo | pagina | R    | carregado | ult. acesso  |\n");
    printf("  +-------+----------+--------+------+-----------+--------------+\n");
    for (int i = 0; i < (int)NUM_FRAMES; i++) {
        Frame *f = &m->mp.frames[i];
        if (f->ocupado)
            printf("  |   %d   |    P%-2d   |   %-3d  |  %d   |   %5lu   |    %6lu    |\n",
                   i, f->processo + 1, f->pagina, f->bit_referencia, f->carregado_em, f->ultimo_acesso);
        else
            // frame vazio: mostra tracinhos pra deixar claro que não tem nada ali
            printf("  |   %d   |    --    |   --   |  -   |     --    |      --      |\n", i);
    }
    printf("  +-------+----------+--------+------+-----------+--------------+\n");
}

// Imprime o resumo da simulação: acessos, hits, faltas e substituições.
void mmu_imprime_estatisticas(MMU *m) {
    unsigned long a = m->stats.acessos;
    unsigned long fa = m->stats.faltas;
    double taxa = a ? (100.0 * (double)fa / (double)a) : 0.0;   // taxa de faltas em %
    printf("\n  Estatisticas (%s):\n", alg_nome(m->alg));
    printf("    acessos.................: %lu\n", a);
    printf("    hits....................: %lu\n", m->stats.hits);
    printf("    faltas de pagina........: %lu  (%.1f%% dos acessos)\n", fa, taxa);
    printf("    cargas em frame livre...: %lu\n", m->stats.carregou_frame_livre);
    printf("    substituicoes...........: %lu\n", m->stats.substituicoes);
}
