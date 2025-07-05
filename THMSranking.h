#ifndef THMSranking_h
#define THMSranking_h

// gcc THMSranking.c THMSranking.h THMSnome.h THMSnome.c TABM.c TABM.h TABMaux.c TABMaux.h -o hashRan

#include "TABM.h"
#include "THMSnome.h"

#define ID_SIZE 13
#define N_RAN 37
#define ANO_ATUAL 2025

typedef struct HashNodeRan {
    char nome[50], id[ID_SIZE];
    int status, ano, pontos, proximo; 
} THranking;

int hash_ranking(int ano);
void THranking_inicializa(char *arqHash, char *arqDados);
void THranking_retira(char *arqHash, char *arqDados, THranking *player);
void THranking_retira_id(char *arqHash, char *arqDados, char *id);
void THranking_imprime(char *arqHash, char *arqDados);
int THranking_busca(char *arqHash, char *arqDados, char *id, int ano);
void THranking_insere(char *arqHash, char *arqDados, THranking *player);
void THranking_copia(char *arqHash, char *arqDados, int ano);
char *THcamp_busca_sobrenome(char *tabHashNome, char *dadosNome, char *sobrenome);
void THranking_construcao(char *arqChampions, char *arqHash, char *arqDados, int t);

#endif