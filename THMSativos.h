#ifndef THMSativos_h
#define THMSativos_h

// gcc THMSativos.c THMSativos.h THMSnome.h THMSnome.c -o hashAti

#include "THMSnome.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ID_SIZE 13
#define ANO_ATUAL 2025

typedef struct HashNodeAti {
    char id[ID_SIZE];
    int status, proximo; 
}THativos;

void THativos_inicializa(char *arqHash, char *arqDados);
void THativos_insere(char *arqHash, char *arqDados, char *id);
void THativos_construcao(char *arqPlayers, char *arqHash, char *arqDados); // constroi hash a partir do arquivo players
void THativos_retira(char *arqHash, char *arqDados, char *id);
char *THativos_busca(char *arqHash, char *arqDados, char *id);

#endif