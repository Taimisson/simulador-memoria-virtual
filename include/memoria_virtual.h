#ifndef MEMORIA_VIRTUAL_H
#define MEMORIA_VIRTUAL_H
/* disco: o conteudo de cada pagina de cada processo.
 * Gerado de forma deterministica (1 MB por processo seria desperdicio de RAM),
 * representando o "arquivo" de onde o bloco e copiado para um frame. */
unsigned char mv_le_byte(int processo, int pagina, int offset);
#endif
