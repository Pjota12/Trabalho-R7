#ifndef THMScampeonatos_h
#define THMScampeonatos_h

// gcc THMScamp.c THMScamp.h THMSnome.h THMSnome.c -o hashCamp

#include "THMSnome.h"

#define ID_SIZE 13
#define TAM_HASH_CAMP 17
#define QTD_TORNEIOS 15
#define ID_DESCONHECIDO "UNKNOWN_ID"
#define VENC_INDETERMINADO "UNDEFINED_WINNER"

typedef struct THcampNode {
    int ano, status;
    char id[ID_SIZE];
    int prox;
} THcamp;

int hash_camp(int coluna);
void THcamp_inicializa(char *arqHash, char *arqDados);
void THcamp_insere(char *arqHash, char *arqDados, char *id, int coluna, int ano);
void THcamp_construcao(char *arqPlayers, char *arqHash, char *arqDados); //constroi hash a partir do arquivo players
char *THcamp_busca(char *arqHash, char *arqDados, char *id, int coluna);
void THcamp_remove(char *arqHash, char *arqDados, char *id);
char *THcamp_busca_sobrenome(char *tabHash, char *dados, char *sobrenome);

#endif
