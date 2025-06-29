#ifndef PLAYERS_H
#define PLAYERS_H

#include "TABM.h"
#include "TABMaux.h"
#include "THMSnome.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// void geradorIdJogador(char *id, int numeroJogador);

void LerEscreverJogadoresArvore(char *nomeIndex, char *nomeArquivoJogadores, char *nomeArquivoCampeoes,int *numeroDeJogadores);

#endif // PLAYERS_H