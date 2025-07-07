#ifndef Q9_H
#define Q9_H

#include "TABM.h"
#include "TABMaux.h"
#include "THMScamp.h"

typedef struct PlayerTitulos9 {
    char id[ID_SIZE];
    char nome[50];
    int cont;
    struct PlayerTitulos9 *prox;
} PlayerTitulos9;

void imprime_jogadores9(PlayerTitulos9* jogadores);
void libera_titulos9(PlayerTitulos9 *jogadores);
PlayerTitulos9 *insere_titulos_ordenado9(PlayerTitulos9 *lista, Tplayer *player, char *id, int cont);
int contaTitulos(FILE *fd, char *arqHash, int coluna, char *id);
int checaRep(PlayerTitulos9 *lista, char *id);
void percorreCamp9(char *arqHash, char *arqDados, int coluna, int t);
void Questao9(int t);

#endif // Q9_H