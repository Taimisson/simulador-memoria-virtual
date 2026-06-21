/* Testes unitarios (rodam sem depender de saida grafica). */
#include <assert.h>
#include <stdio.h>
#include "config.h"
#include "mmu.h"
#include "substituicao.h"

static void test_layout_enderecos(void) 
{
    // 8 frames, 128 paginas, offset de 13 bits
    assert(NUM_FRAMES == 8);
    assert(NUM_PAGINAS == 128);
    assert(MASCARA_OFFSET == 0x1FFF);
    uint32_t vaddr = (29u << 13) | 0x012C;     // pagina 29, offset 0x012C
    assert(PAGINA_DO_ENDERECO(vaddr) == 29);
    assert(OFFSET_DO_ENDERECO(vaddr) == 0x012C);
    assert(ENDERECO_FISICO(5, 0x012C) == ((5u << 13) | 0x012C));
    printf("  ok  layout de enderecos (pagina/offset/fisico)\n");
}

static void test_falta_depois_hit(void) 
{
    MMU m;
    mmu_init(&m, 1, ALG_FIFO, 0);
    uint32_t v = (3u << 13) | 10;
    assert(mmu_acessar(&m, 0, v) == 0);
    assert(m.stats.faltas == 1 && m.stats.hits == 0);   /* 1o acesso = falta */
    assert(mmu_acessar(&m, 0, v) == 0);
    assert(m.stats.faltas == 1 && m.stats.hits == 1);   /* 2o acesso = hit  */
    mmu_destroy(&m);
    printf("  ok  primeira referencia gera falta, segunda gera hit\n");
}

static void test_endereco_invalido(void) 
{
    MMU m;
    mmu_init(&m, 1, ALG_LRU, 0);
    assert(mmu_acessar(&m, 0, TAM_MEMORIA_VIRTUAL) == -1);  /* fora de 1 MB */
    assert(m.stats.acessos == 0);
    mmu_destroy(&m);
    printf("  ok  endereco fora da memoria virtual e rejeitado\n");
}

static void test_substituicao_quando_cheia(void) 
{
    /* 1 processo acessando 9 paginas distintas -> 8 frames enchem, a 9a substitui */
    MMU m;
    mmu_init(&m, 1, ALG_FIFO, 0);
    for (uint32_t pag = 0; pag < 9; pag++)
        mmu_acessar(&m, 0, pag << 13);
    assert(m.stats.faltas == 9);
    assert(m.stats.carregou_frame_livre == 8);   /* 8 frames livres usados */
    assert(m.stats.substituicoes == 1);           /* a 9a pagina substituiu */
    /* FIFO: a vitima foi a pagina 0 (primeira a entrar) */
    assert(m.tabelas[0].entradas[0].valida == 0);
    mmu_destroy(&m);
    printf("  ok  enche 8 frames e substitui a 9a pagina (FIFO = pagina 0)\n");
}

static void test_clock_da_segunda_chance(void) 
{
    /* todos com bit R=1: o Clock zera e na 2a volta escolhe o frame 0 */
    MMU m;
    mmu_init(&m, 1, ALG_CLOCK, 0);
    for (uint32_t pag = 0; pag < 8; pag++)
        mmu_acessar(&m, 0, pag << 13); /* enche, todos R=1 */
    mmu_acessar(&m, 0, 100u << 13);   /* falta com memoria cheia -> substitui */
    assert(m.stats.substituicoes == 1);
    assert(m.tabelas[0].entradas[0].valida == 0); /* pagina do frame 0 saiu */
    mmu_destroy(&m);
    printf("  ok  Clock da segunda chance e escolhe o frame 0\n");
}

int main(void) 
{
    printf("== testes do simulador ==\n");
    test_layout_enderecos();
    test_falta_depois_hit();
    test_endereco_invalido();
    test_substituicao_quando_cheia();
    test_clock_da_segunda_chance();
    printf("== TODOS OS TESTES PASSARAM ==\n");
    return 0;
}
