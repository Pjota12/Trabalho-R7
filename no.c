#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T 20 // Grau mínimo da árvore B
#define MAX_KEYS (2 * T - 1) // Máximo de chaves em um nó
#define MIN_KEYS (T - 1) // Mínimo de chaves em um nó
#define MAX_CHILDREN (2 * T) // Máximo de filhos em um nó
#define MIN_CHILDREN (T) // Mínimo de filhos em um nós

#define ID_SIZE 10
#define ARQ_SIZE 8


//grand slams, que fornecem 2000 pontos
//ATP1000 1000 prontos

//ARQUIVO CHAMPIONS
/*
1- ano
"CADA PROXIMO É SOBRENOME DOS JOGADORES"
2- Australian Open |--> GRAND SLAM
3- Roland Garros   |        
4- Wimbledon       |        2000 pontos
5- US Open         |
6- ATP Finals  
7- OLIMPIADAS
8- INDIAN WELLS 1000         |--> ATP 1000 
9- Miami 1000                |    
10- Monte Carlo              |       1000 pontos
11- Madrid 1000              |   
12- Rome 1000                |
13- Canada 1000              |
14- Cincinnati 1000          |   
15- Shanghai 1000            |
16- Paris 1000               |
*/

// ARQUIVO JOGADORES
/* 
1- NOME
2- NASCIMENTO
3- MORTE
4- NACIONALIDADE
5- RANKING
6- ANO DO MELHOR RANKING (SE FOR PRIMEIRO NO RANK, TEMPO)
*/



typedef struct node
{
    int isValuesLeaf; //indica se os filhos são Arquivos ou nos      
    int numKeys;  // Número de chaves no nó
    char keys[2*T][ID_SIZE];  //[12202020,21312312,1231312]
    char filhos[2*T][ARQ_SIZE]; //[n1,n2,n3]
}Tnode;

typedef struct Player {
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
//pontuação dos torneios por ano



//1- hash de aposentados que informe o id do jogador(MENMORIA SECUNDARIA) -> contar quantos aposentados/ativos
/*  -> informar id pela hash -> buscar id na arvore -> alocar vetor de player no tamanho -> ordenar por numero de titulos
    ->printar por estilo da questão(grand slams,ATP 1000,ATP Finals,Olimpíadas)
*/
typedef struct HashNode {
    char id[ID_SIZE];
    struct HashNode *next; 
}THashNode;

/*2- hash de nacionalidade que informe o id do jogador(MENMORIA SECUNDARIA) -> ja tem o nascimento no id -> procura jogadores na mesma hash 
que possuem rank e, se ganhou um grandslan no ano respectivo do nascimento*/

typedef struct NationalityNode {
    char nacionalidade[5];
    struct NationalityNode *next;
}TNationalityNode;

/*3- pecorrer hash de aposentados e ativos alocacar vetor de PlayerPoint de tamanho n -> ordenar de maior pro menor a pontuação do jogador*/

struct PlayerPoints {
    char nome[50];    // Sobrenome
    int pontos;
};

/*4- pecorrer o arquivo -> criar struct para cada jogar -> inserir cada jogador ordenado por pontos -> imprimir os 25 primeiros
    para cada ano verificar se o jogador ja existe na lista -> se existir, somar os pontos, se não existir, criar novo jogador
    -> ordenar por pontos -> imprimir os 25 primeiros jogadores
*/

/*5- pecorrer arquivo -> verificar se nas 4 linhas é o mesmo nome -> acessar tabela de de id sobrenamos => id -> retornar nome do 
cara e o ano*/

/*6- hash do ranking 25 e hash do campeonato (por carpeonato (4)) -> pecorre o campeonato especifico -> busca o jogador -> busca os
anos que ganhou o campeonato e verifica se ele esta no rank ou não em cada ano no vetor*/ //0 ou 1

/*7- pecorer hash do pais -> verificar se é aposentado pelo id -> se não for retirar da arvore*/

/*8  pecorrer o arquivo -> verificar a pontação de cada jogador -> ordena por pontuação -> se igual então granslan ->se igual então ATP 1000
-> se igual ano anterior -> imprimir os 8 */

/*9- acessar hash de campeonato -> para o campeonato respectivo contar as hash -> slavar vetor -> alocar vetor com nome e pontução */