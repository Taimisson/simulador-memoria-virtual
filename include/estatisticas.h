#ifndef ESTATISTICAS_H
#define ESTATISTICAS_H
typedef struct {
    unsigned long acessos;
    unsigned long hits; /* pagina ja estava na memoria principal */
    unsigned long faltas; /* page faults */
    unsigned long carregou_frame_livre; /* falta resolvida em frame livre */
    unsigned long substituicoes; /* falta que exigiu trocar uma pagina */
} Estatisticas;
#endif
