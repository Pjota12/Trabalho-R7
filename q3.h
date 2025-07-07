#ifndef q3_H
#define q3_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "THMSativos.h"
#include "TABM.h"

typedef struct TLDE_tenistas {
    char nome[50];
    int pont;
    struct TLDE_tenistas *prox;
}TLDETEN;

void imprime_jogadores(TLDETEN* jogadores);
void libera(TLDETEN *jogadores); // libera a lista
TLDETEN *insere_ordenado(TLDETEN *lista, Tplayer *player);
void Questao3(int t);


#endif // q3_H