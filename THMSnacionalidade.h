#ifndef THMSnacionalidade_h
#define THMSnacionalidade_h

// gcc THMSnacionalidade.c THMSnacionalidade.h THMSnome.h THMSnome.c -o hashNac

#include "THMSnome.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ID_SIZE 13
#define N_NAC 59

typedef struct HashNodeNac {
    char id[ID_SIZE];
    int status, proximo; 
}THnacionalidade;

void maiuscula(char *str);
unsigned int hash_nacionalidade(const char* str);
void THnacionalidade_inicializa(char *arqHash, char *arqDados);
void THnacionalidade_insere(char *arqHash, char *arqDados, char *id);
void THnacionalidade_construcao(char *arqPlayers, char *arqHash, char *arqDados); //constroi hash a partir do arquivo players
void THnacionalidade_retira(char *arqHash, char *arqDados, char *id);
char *THnacionalidade_busca(char *arqHash, char *arqDados, char *id);

#endif