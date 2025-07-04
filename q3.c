/*3- pecorrer hash de aposentados e ativos alocacar vetor de PlayerPoint de tamanho n -> ordenar de maior pro menor a pontuação do jogador*/

//gcc q3.c THMSativos.h THMSativos.c TABM.h TABM.c TABMaux.h TABMaux.c THMSnome.c THMSnome.h -o q3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "THMSativos.h"
#include "TABM.h"

typedef struct TLDE_tenistas {
    char nome[50];
    int pont;
    struct TLDE_tenistas *prox;
}TLDETEN;

void imprime_jogadores(TLDETEN* jogadores) {
    printf("\nLista de jogadores: \n");
    TLDETEN* aux = jogadores;
    int contador = 0;
    while (aux) {
        contador++;
        printf("%s: Total = %d\n", aux->nome, aux->pont);
        aux = aux->prox;
    }
    printf("%d jogadores",contador);
}

void libera(TLDETEN *jogadores) { // libera a lista
    TLDETEN *temp;
    while (jogadores){
        temp = jogadores;
        jogadores = jogadores->prox;
        free(temp);
    }
}

TLDETEN *insere_ordenado(TLDETEN *lista, Tplayer *player) {
    TLDETEN *p = (TLDETEN *)malloc(sizeof(TLDETEN));
    
    // Nome
    strcpy(p->nome,player->nome);
    // Pontuacao
    p->pont = player->pontosTotais;

    TLDETEN *aux = lista, *ant = NULL;
    while((aux) && (p->pont < aux->pont)) {
        ant = aux;
        aux = aux->prox;
    }
    
    if((!lista) || (!ant)) { // insere no inicio
        p->prox = lista;
        lista = p;
    }
    else { // meio ou fim
        p->prox = ant->prox;
        ant->prox = p;
    }
    return lista;
}

int main() {

    // Para construir a hash
    THativos_construcao("tennis_players.txt","hash_ativos.bin","dados_ativos.bin");

    FILE *fh = fopen("hash_ativos.bin","rb"), *fp = fopen("dados_ativos.bin","rb");
    if((!fh) || (!fp)) exit(1);
    int pos, t = 10;

    printf("t = ");
    scanf("%d",&t);

    fread(&pos,sizeof(int),1,fh);

    TLDETEN *listaTLDETEN = NULL;
    
    while(pos != -1) {
        THativos aux;
        fseek(fp,pos,SEEK_SET);
        fread(&aux,sizeof(THativos),1,fp);
        if(aux.status) {
            Tplayer *player = buscarJogador(aux.id,0,t);
    
            listaTLDETEN = insere_ordenado(listaTLDETEN, player);    
        }
        pos = aux.proximo;
    }

    fread(&pos,sizeof(int),1,fh);

    while(pos != -1) {
        THativos aux;
        fseek(fp,pos,SEEK_SET);
        fread(&aux,sizeof(THativos),1,fp);
        if(aux.status) {
            Tplayer *player = buscarJogador(aux.id,0,10);
    
            listaTLDETEN = insere_ordenado(listaTLDETEN, player);    
        }
        pos = aux.proximo;
    }

    imprime_jogadores(listaTLDETEN);
    libera(listaTLDETEN);

    return 0;
}