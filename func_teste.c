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
#define QTD_TOR 15
#define ANO_INICIAL 1990
#define ANO_FINAL 2025

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
    char keys[MAX_KEYS][ID_SIZE];  //[12202020,21312312,1231312]
    char filhos[MAX_CHILDREN][ARQ_SIZE]; //[n1,n2,n3]
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

typedef struct PlayerTitulos { // guarda um contador para cada torneio, não sei se precisa ou não, mas seria útil para guardar separadamente por torneio e imprimir na ordem certa
    char id[ID_SIZE];
    char nome[50];
    int grand_slams;
    int atp_1000;
    int atp_finals;
    int olimpiadas;
    int total_titulos;
    int aposentado; // 1 = aposentado, 0 = ativo
    struct PlayerTitulos *prox;
} PlayerTitulos;

void imprime_jogadores(PlayerTitulos* jogadores, int aposentado) {
    if (aposentado) printf("\nAposentados:\n\n");
    else printf("\nAtivos: \n\n");
    PlayerTitulos* aux = jogadores;
    while (aux){
        if (aux->aposentado == aposentado && aux->total_titulos > 0) {
            printf("%s: GS = %d, ATP1000 = %d, Finals = %d, Olimp = %d, Total = %d\n", aux->nome, aux->grand_slams, aux->atp_1000, aux->atp_finals, aux->olimpiadas, aux->total_titulos);
        }
        aux = aux->prox;
    }
}

void libera(PlayerTitulos *jogadores) { // libera a lista
    PlayerTitulos *temp;
    while (jogadores){
        temp = jogadores;
        jogadores = jogadores->prox;
        free(temp);
    }
}

void impJogcomTitulo() {
    PlayerTitulos *jogadores = NULL;
    // percorre a hash de campeonatos e conta quantos titulos cada jogador tem na conta_titulos e faz uma lista encadeada com a struct PlayerTitulos em ordem decrescente de títulos
    imprime_jogadores(jogadores, 1); // aposentados
    imprime_jogadores(jogadores, 0); // ativos
    libera(jogadores);
    return 0;
}

/*2- hash de nacionalidade que informe o id do jogador(MENMORIA SECUNDARIA) -> ja tem o nascimento no id -> procura jogadores na mesma hash 
que possuem rank e, se ganhou um grandslan no ano respectivo do nascimento*/

//acha o ano em que ele ganhou um grand slam e busca na hash de nacionalidade compatriotas que nasceram naquele ano

void leFolha(char *folha){
    FILE *ff = fopen(folha, "rb+");
    Tplayer tenista;
    fread(&tenista, sizeof(Tplayer), 1, ff);
    for (int i = 0; i < 3; i++){ // limita-se aos GrandSlams
        if (tenista.quantidadeTorneios[i]){
            aux_hashCamp(tenista.id); 
        }
    }
}

void percorreArvore(){
    FILE *fi = fopen("indice.bin", "rb+");
    if (!fi) exit(1);
    Tnode aux;
    while (fread(&aux, sizeof(Tnode), 1, fi)){
        if (aux.isValuesLeaf) {
            for (int i = 0; i < 2*T; i++){
                le_folha(aux.filhos[i]);
            }
        }
    }
    fclose(fi);
}

/*3- pecorrer hash de aposentados e ativos alocacar vetor de PlayerPoint de tamanho n -> ordenar de maior pro menor a pontuação do jogador*/

typedef struct TLDE_tenistas{
    char nome[50];
    int pont;
    struct TLDE_tenistas *prox, *ant;
}TLDETEN;

TLDETEN* insereDecresc(TLDETEN *l, Tplayer tenista) {
    TLDETEN *novo = (TLDETEN *) malloc(sizeof(TLDETEN));
    strcpy(novo->nome, tenista.nome);
    novo->pont = tenista.pontosTotais;
    novo->prox = novo->ant = NULL;

    if (!l) return novo;

    TLDETEN *aux = l;

    // inserção no início 
    if (novo->pont > aux->pont) {
        novo->prox = l;
        l->ant = novo;
        return novo;
    }

    // percorre até achar posição correta
    while (aux->prox && aux->prox->pont >= novo->pont) aux = aux->prox;

    // inserção no meio ou fim
    novo->prox = aux->prox;
    novo->ant = aux;
    if (aux->prox) aux->prox->ant = novo;
    aux->prox = novo;

    return l;
}

TLDETEN* leFolha(char *folha, TLDETEN *l){
    FILE *ff = fopen(folha, "rb+");
    Tplayer tenista;
    while (fread(&tenista, sizeof(Tplayer), 1, ff)){
        l = insere_decresc(l, tenista); // insere em ordem decrescente por pontuação total
    }
    fclose(ff);
    return l;
}

void pontDecresc(){
    FILE *fi = fopen("indice.bin", "rb+");
    if (!fi) exit(1);
    Tnode aux;
    TLDETEN *l = NULL;
    while (fread(&aux, sizeof(Tnode), 1, fi)){
        if (aux.isValuesLeaf) {
            for (int i = 0; i < 2*T; i++){
                l = le_folha(aux.filhos[i], l);
            }
        }
    }

    TLDETEN *temp;
    while (l) {
        printf("%s: %d pontos\n", l->nome, l->pont); //imprime e libera a lista
        temp = l;
        l = l->prox;
        free(temp);
    }

    fclose(fi);
}

/*4- pecorrer o arquivo -> criar struct para cada jogador -> inserir cada jogador ordenado por pontos -> imprimir os 25 primeiros
    para cada ano verificar se o jogador ja existe na lista -> se existir, somar os pontos, se não existir, criar novo jogador
    -> ordenar por pontos -> imprimir os 25 primeiros jogadores
*/

typedef struct PlayerAno{
    char nome[50];
    int pontosAno;
} TPlayerAno;

void ranking_por_ano() {
    for (int ano = ANO_INICIAL; ano <= ANO_FINAL; ano++) { // i = ano
        TPlayerAno *jogadores = NULL;
        int cont = 0, lim;

        // Para cada jogador:
        //   - Calcule pontos obtidos no ano baseada na hash de campeonatos
        //   - Se > 0, insira na lista encadeada e cont++

        // Ordene jogadores por pontosAno decrescente

        printf("Ano %d:\n", ano);
        if (cont<25) lim = cont;
        else lim = 25;
        TPlayerAno *aux = jogadores;
        for (int i = 0; i < lim; i++) { //printa os 25 melhores ou menos se ainda não tivr alcançado 25
            printf("%dº %s: %d pontos\n", i+1, aux->nome, aux->pontosAno);
        }
        printf("\n");
    }
}

/*5- pecorrer arquivo -> verificar se nas 4 linhas é o mesmo nome -> acessar tabela de id sobrenames => id -> retornar nome do 
cara e o ano*/

typedef struct vencedor{
    char nome[50], ano[5]; 
    struct vencedor *prox;
}vencedor;

vencedor *vencGrandSlam_aux(char *champions){
    FILE *fc = fopen(champions, "r");
    if (!fc) exit(1);
    
    vencedor *vencedores = NULL;

    char linha[1000];
    fgets(linha, sizeof(linha), fc);

    while (fgets(linha, sizeof(linha), fc)){
        fgets(linha, sizeof(linha), fc);

        // pegando o ano
        char *info = strtok(linha, "\\");
        char ano[5];
        strcpy(ano, info);
        printf("%s \n", ano);

        // pegando o nome do primeiro slam
        info = strtok(NULL, "\\");
        char venc_slams_ant[50];
        char *nome = strtok(info, "(");
        int len = strlen(nome);
        while (len > 0 && nome[len-1] == ' ') nome[--len] = '\0'; // como se fosse um .trim()
        strcpy(venc_slams_ant, nome);
        printf("%s \n", venc_slams_ant);
        int iguais = 1;

        for(int i = 1; i < 4; i++) {
            info = strtok(NULL, "\\");
            nome = strtok(info, "(");
            len = strlen(nome);
            while (len > 0 && nome[len-1] == ' ') nome[--len] = '\0';
            printf("%s \n", nome);
            if ((strcmp(nome, "-") == 0)|| (strcmp(venc_slams_ant, nome) != 0)) {
                iguais = 0;
                break;
            }
        }

        if (iguais) {
            vencedor *novo = (vencedor*) malloc(sizeof(vencedor));
            // char nome[50];
            // acha o nome com a tabela hash que relaciona nome e sobrenome e retorna na variavel nome
            strcpy(novo->nome, venc_slams_ant);
            strcpy(novo->ano, ano);
            novo->prox = vencedores;
            vencedores = novo;
        }
    }
    fclose(fc);
    return vencedores;
}

void vencGrandSlam(void){
    vencedor *vencedores = vencGrandSlam_aux("champions.txt"); // consertar código, não pode ler diretamente o arquivo champions.txt
    vencedor *aux = vencedores;
    while (aux){
        printf("%s venceu os 4 Slams em %s\n", aux->nome, aux->ano);
        vencedor *temp = aux;
        aux = aux->prox;
        free(temp);
    }
    return 0;
}

/*6- hash do ranking 25 e hash do campeonato (por carpeonato (4)) -> pecorre o campeonato especifico -> busca o jogador -> busca os
anos que ganhou o campeonato e verifica se ele esta no rank ou não em cada ano no vetor*/ //0 ou 1

/* 7 - acessa a hash de nacionalidade, e para todo id que encontrar na linha daquela nacionalidade, busca o id na árvore e retira */

void retiraPais(char *nacionalidade){
    FILE *fi = fopen("indice.bin", "rb+");
    if (!fi) exit(1);
    hashNacional(nacionalidade); // usa a retira nó dentro da hash
}

/*8  pecorrer o arquivo -> verificar a pontação de cada jogador -> ordena por pontuação -> se igual então granslan ->se igual então ATP 1000
-> se igual ano anterior -> imprimir os 8 */

/*9- acessar hash de campeonato -> para o campeonato respectivo contar as hash -> salvar vetor -> alocar vetor com nome e pontução */
