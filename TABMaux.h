#ifndef TABMAUX_H
#define TABMAUX_H

#include "TABM.h"

// //PEGA OFSET LOGICO DO ARQUIVO DE ÍNDICE
// int getOfsetLogicoRaiz(char*nomeArquivo);
// //RETORNA O OFSET LOGICO DA RAIZ

// //ATUALIZA O OFSET LOGICO DO ARQUIVO DE ÍNDICE
// int updateOfsetLogicoRaiz(char*nomeArquivo,int ofset);

//Calcula o ofset lógico para inserção de novos nós
// Retorna o ofset lógico do próximo nó disponível no arquivo de índice
void printChaves(Tnode *node);

void printFilhos(Tnode *node);

void printIndexTotal(char *nomeArquivo,int T);

void printFolhaImpressa(char *nomeArquivoFolha, int nivel);

int calculaOfsetLogico(char *nomeArquivo,int T);

// Função para obter o número de folhas do arquivo de índice
int getNumeroDeFolha(char*nomeArquivo);

// Função para atualizar o número de folhas no arquivo de índice
void updateNumeroDeFolha(char*nomeArquivo,int numFolha);

// Função para obter o nome do próximo nó a partir do arquivo
//serve para ler o nome do proximo nó a partir do arquivo de índice
void getProximaFolha(char *nomeArquivoFolha,char *nomeNo);

//ESCREVE O NOME FOLHA BASEADO NO NUMERO DE FOLHAS
void WriteLeafName(char *nomeNovafolha,int i);

// Função q devolve o limite de leitura da folha
long limiteFolha(char *nomeArquivoFolha);

// Função para contar o número de elementos na folha
int contaElementosFolha(char *nomeArquivoFolha);

// Função para inserir um jogador na folha ordenada
void inserirJogadorNaFolhaOrdenado(char *NomeArqFolha,Tplayer *novoJogador);

// Função para inserir um jogador na folha ordenada
Tplayer* buscaBinariaJogadorNaFolha(char *NomeArqFolha,char *idJogador);

int ReadLeafNumber(char *nomeArquivoFolha);

#endif