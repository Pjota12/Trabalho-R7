#ifndef Q8_H
#define Q8_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "THMStemporada.h"
#include "TABM.h"

#define ANO_ATUAL 2025


typedef struct PlayerTemp {
    char nome[50], id[ID_SIZE];
    int pontos, grandSlams, atpFinals, olimpiadas, atp1000;
    struct PlayerTemp *prox; 
}PlayerTemp;

void libera_melhores(PlayerTemp *jogadores);
void imprime_melhores(PlayerTemp *jogadores);
PlayerTemp *insere_melhores_ordenado(PlayerTemp *lista, THtemporada *player);
void Questao8();

#endif // Q8_H