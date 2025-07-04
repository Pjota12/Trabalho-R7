#ifndef THMScampeonatos_ano_h
#define THMScampeonatos_ano_h

// gcc THMScamp_ano.c THMScamp_ano.h THMSnome.h THMSnome.c -o hashCampAno

#include "THMSnome.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ID_SIZE 13
#define TAM_HASH_CAMP_ANO 37
#define QTD_TORNEIOS 15

typedef struct THcampNode {
    int status;
    char id[ID_SIZE];
    int prox;
} THcamp_ano;

int hash_camp_ano(int ano, int tam);
void THcamp_ano_inicializa(char *arqHash, char *arqDados);
void THcamp_ano_insere(char *arqHash, char *arqDados, char *id, int ano);
void THcamp_ano_construcao(char *arqPlayers, char *arqHash, char *arqDados); //constroi hash a partir do arquivo players
void THcamp_ano_retira(char *arqHash, char *arqDados, char *id);
char *THcamp_ano_busca(char *arqHash, char *arqDados, char *id, int ano);
void THcamp_ano_remove(char *arqHash, char *arqDados, char *id, int ano);
char *THcamp_ano_busca_sobrenome(char *tabHash, char *dados, char *sobrenome);

#endif