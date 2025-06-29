#ifndef THMSnome_h
#define THMSnome_h

#include "TABM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define TAM_HASH 367 // Tamanho da tabela hash
typedef struct THnomeToid{
  char nome[50]; // Nome do aluno
  char id[13]; // ID do aluno
  int status; // Indica se o registro está ocupado
  int proximo; // Próximo registro na lista encadeada
}THnomeToid;

void THnome_inicializa(char *tabHash, char *dados);
char *THnome_busca(char *tabHash, char *dados, char *nome);
void THnome_retira(char *tabHash, char *dado, char *nome);
void THnome_insere(char *tabHash, char *dado, char *nome, char *id);
void THnome_imprime (char *nome_hash, char *nome_dados, int m);


#endif /* THMSnome_h */