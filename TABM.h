#ifndef TABM_H
#define TABM_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T 10 // Grau mínimo da árvore B
#define MAX_KEYS (2 * T - 1) // Máximo de chaves em um nó
#define MIN_KEYS (T - 1) // Mínimo de chaves em um nó
#define MAX_CHILDREN (2 * T) // Máximo de filhos em um nó
#define MIN_CHILDREN (T) // Mínimo de filhos em um nós

#define ID_SIZE 13
#define ARQ_SIZE 11

#define VAZIO "NULLspace"
#define NOME_FOLHA "leaf"


typedef struct node{
    int isOcuped; //indica se o no esta com ocupado     
    int numKeys;  // Número de chaves no nó
    char keys[MAX_KEYS][ID_SIZE];  //[12202020,21312312,1231312]
    char filhos[MAX_CHILDREN][ARQ_SIZE]; //[n1,n2,n3]
}Tnode;

typedef struct player {
    char id[ID_SIZE];          // ID usado na árvore "1231986BRA"  
    char nome[50];
    int anoNascimento;
    int anoMorte;
    char nacionalidade[4];     // Pode ser codificado ESP, BRA, ARG, etc.
    int ranking;             // melhor ranking do jogador
    int anoRanking;         // Ano do melhor ranking
    int semanasNoTopo;  // Semanas no topo do ranking
    int pontosTotais;       //pontos totais
    int quantidadeTorneios[15]; // grand slams = [0] - [3] | [4] ATP Finals | [5] Olimpiadas | [6-14] ATP 1000
}Tplayer;

// void criarIndex(char *arquivoJogadores, char *arquivoCampeoes);

void InicializarIndex();

void InsereJogador(char *nomeArqIndex,Tplayer *novoJogador,int atual,int Pai);

Tplayer* buscarJogador(char *id,int n);

void imprimirArvoreB();

void KillArvoreB();

#endif // TABM_H