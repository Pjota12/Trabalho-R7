#ifndef q2_H
#define q2_H

#include "THMSranking.h"
#include "THMSnacionalidade.h"
#include "THMScamp.h"
#include "TABM.h"

void buscarCompatriotas(char *arqHash, char *arqDados, char* nacionalidade, int ano, int t);
void percorreRank(char *arqHash, char *arqDados, char* id, int ano, int t);
void percorreGS(char *arqHash, char *arqDados, int coluna, int t);
void Questao2(int t);

#endif
