#ifndef q1_H
#define q1_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "THMSativos.h"
#include "TABM.h"
typedef struct PlayerTitulos {
    char id[ID_SIZE];
    char nome[50];
    int grand_slams;
    int atp_1000;
    int atp_finals;
    int olimpiadas;
    int total_titulos;
    struct PlayerTitulos *prox;
} PlayerTitulos;

void imprime_players(PlayerTitulos* jogadores, int aposentado);
void libera_titulos(PlayerTitulos *jogadores); // libera a lista
PlayerTitulos *insere_titulos_ordenado(PlayerTitulos *lista, Tplayer *player);
void Questao1(int t);

#endif // q1_H 