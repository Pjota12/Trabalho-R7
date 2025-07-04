#ifndef THMScampeonatos_h
#define THMScampeonatos_h

// gcc THMScamp.c THMScamp.h THMSnome.h THMSnome.c -o hashCamp

#include "THMSnome.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ID_SIZE 13
#define TAM_HASH_CAMP 17
#define QTD_TORNEIOS 15

typedef struct THcampNode {
    int ano, status;
    char id[ID_SIZE];
    int prox;
} THcamp;

int hash_camp();
void THcamp_inicializa(char *arqHash, char *arqDados);
void THcamp_insere(char *arqHash, char *arqDados, char *id, int coluna, int ano);
void THcamp_construcao(char *arqPlayers, char *arqHash, char *arqDados); //constroi hash a partir do arquivo players
void THcamp_retira(char *arqHash, char *arqDados, char *id);
char *THcamp_busca(char *arqHash, char *arqDados, char *id, int coluna);
void THcamp_remove(char *arqHash, char *arqDados, char *id, int coluna);
char *THcamp_busca_sobrenome(char *tabHash, char *dados, char *sobrenome);

#endif