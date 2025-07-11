#ifndef THMScampeonatos_ano_h
#define THMScampeonatos_ano_h

// gcc THMScamp_ano.c THMScamp_ano.h THMSnome.h THMSnome.c -o hashCampAno

#include "THMSnome.h"

#define ID_SIZE 13
#define TAM_HASH_CAMP_ANO 37
#define QTD_TORNEIOS 15
#define ID_DESCONHECIDO "UNKNOWN_ID"
#define VENC_INDETERMINADO "UNDEFINED_WINNER"

typedef struct THcampanoNode {
    int status;
    char id[ID_SIZE];
    int prox;
} THcamp_ano;

int hash_camp_ano(int ano);
void THcamp_ano_inicializa(char *arqHash, char *arqDados);
void THcamp_ano_insere(char *arqHash, char *arqDados, char *id, int ano);
void THcamp_ano_construcao(char *arqPlayers, char *arqHash, char *arqDados); //constroi hash a partir do arquivo players
char *THcamp_ano_busca(char *arqHash, char *arqDados, char *id, int ano);
void THcamp_ano_remove(char *arqHash, char *arqDados, char *id);
char *THcamp_ano_busca_sobrenome(char *tabHash, char *dados, char *sobrenome);

#endif
