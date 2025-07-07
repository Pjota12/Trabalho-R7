#ifndef Q6_H
#define Q6_H
#include "THMSranking.h"
#include "THMSnacionalidade.h"
#include "THMScamp.h"
#include "TABM.h"

void percorreRank(char *arqHash, char *arqDados, char* id, int ano, int t);
void percorreCamp(char *arqHash, char *arqDados, int coluna, int t);
void Questao6(int t);

#endif // Q6_H