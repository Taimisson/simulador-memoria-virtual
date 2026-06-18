#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

/* especificacao do gb
 * memoria principal (fisica): 64 KB
 * memoria virtual: 1 MB
 * bloco (pagina = frame): 8 KB
 */
#define TAM_MEMORIA_PRINCIPAL (64u * 1024u) /* 64 KB  */
#define TAM_MEMORIA_VIRTUAL   (1024u * 1024u) /* 1 MB   */
#define TAM_BLOCO             (8u * 1024u)  /* 8 KB   */

#define NUM_FRAMES   (TAM_MEMORIA_PRINCIPAL / TAM_BLOCO) /* 64K/8K  = 8   frames  */
#define NUM_PAGINAS  (TAM_MEMORIA_VIRTUAL   / TAM_BLOCO) /* 1M/8K   = 128 paginas */

/* Layout do endereco:
 *   offset = log2(8K) = 13 bits
 *   nº da pagina = 20 - 13 = 7  bits  (2^7  = 128 paginas)
 *   nº do frame  = 16 - 13 = 3  bits  (2^3  = 8   frames)
 *   endereco virtual = 20 bits (1 MB) ; endereco fisico = 16 bits (64 KB)
 */
#define BITS_OFFSET     13
#define MASCARA_OFFSET  (TAM_BLOCO - 1u) /* 0x1FFF */

#define PAGINA_DO_ENDERECO(vaddr)  ((uint32_t)(vaddr) >> BITS_OFFSET)
#define OFFSET_DO_ENDERECO(vaddr)  ((uint32_t)(vaddr) & MASCARA_OFFSET)
#define ENDERECO_FISICO(frame, off) (((uint32_t)(frame) << BITS_OFFSET) | (uint32_t)(off))

#endif
