#ifndef THMStemporada_h
#define THMStemporada_h

// gcc THMStemporada.h THMStemporada.c THMSnome.h THMSnome.c TABM.c TABM.h TABMaux.c TABMaux.h -o hashTem

#include "TABM.h"
#include "THMSnome.h"
#include <ctype.h>

#define ID_SIZE 13
#define N_TEM 37
#define ANO_ATUAL 2025

typedef struct HashNodeTem {
    char nome[50], id[ID_SIZE];
    int status, ano, pontos, grandSlams, atpFinals, olimpiadas, atp1000,  proximo; 
}THtemporada;

int hash_temporada(int ano);
void THtemporada_inicializa(char *arqHash, char *arqDados);
void THtemporada_retira_id(char *arqHash, char *arqDados, char *id);
void THtemporada_retira(char *arqHash, char *arqDados, THtemporada *player);
void THtemporada_imprime(char *arqHash, char *arqDados);
int THtemporada_busca(char *arqHash, char *arqDados, char *id, int ano);
void THtemporada_insere(char *arqHash, char *arqDados, THtemporada *player);
char *THtemporada_busca_sobrenome(char *tabHashNome, char *dadosNome, char *sobrenome);
void THtemporada_construcao(char *arqChampions, char *arqHash, char *arqDados, int t);

#endif