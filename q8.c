/*8  pecorrer o arquivo -> verificar a pontação de cada jogador -> ordena por pontuação -> se igual então granslan ->se igual então ATP 1000
-> se igual ano anterior -> imprimir os 8 */

// gcc q8.c THMStemporada.h THMStemporada.c TABM.h TABM.c TABMaux.h TABMaux.c -o q8

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "THMStemporada.h"
#include "TABM.h"

#define ANO_ATUAL 2025

typedef struct PlayerTemp {
    char nome[50], id[ID_SIZE];
    int pontos, grandSlams, atpFinals, olimpiadas, atp1000;
    struct PlayerTemp *prox; 
}PlayerTemp;

void libera_melhores(PlayerTemp *jogadores) { // libera a lista
    PlayerTemp *temp;
    while (jogadores){
        temp = jogadores;
        jogadores = jogadores->prox;
        free(temp);
    }
}

void imprime_melhores(PlayerTemp *jogadores) {
    PlayerTemp* aux = jogadores;
    int players = 0;
    while((aux) && (players < 8)) {
        players++;
        printf("%d %s %s: %d pontos, GS = %d, ATP1000 = %d\n", players, aux->id, aux->nome, aux->pontos, aux->grandSlams, aux->atp1000);
        aux = aux->prox;
    }
}

PlayerTemp *insere_melhores_ordenado(PlayerTemp *lista, THtemporada *player) {
    PlayerTemp *playerTemp = (PlayerTemp *)malloc(sizeof(PlayerTemp));

    strcpy(playerTemp->id,player->id);
    strcpy(playerTemp->nome,player->nome);
    playerTemp->pontos = player->pontos;
    playerTemp->grandSlams = player->grandSlams;
    playerTemp->atpFinals = player->atpFinals;
    playerTemp->olimpiadas = player->olimpiadas;
    playerTemp->atp1000 = player->atp1000;

    PlayerTemp *aux = lista, *ant = NULL;
    while((aux) && (playerTemp->pontos < aux->pontos)) {
        ant = aux;
        aux = aux->prox;
    }

    if((aux) && (playerTemp->pontos == aux->pontos)) {
        while((aux) && (playerTemp->grandSlams < aux->grandSlams)) {
            ant = aux;
            aux = aux->prox;
        }
        if((aux) && (playerTemp->grandSlams == aux->grandSlams)){
            while((aux) && (playerTemp->atp1000 < aux->atp1000)) {
                ant = aux;
                aux = aux->prox;
            }
            if((aux) && (playerTemp->atp1000 == aux->atp1000)) {
                while((aux) && (THtemporada_busca("hash_temporada.bin","dados_temporada.bin",playerTemp->id,player->ano - 1) < THtemporada_busca("hash_temporada.bin","dados_temporada.bin",aux->id,player->ano - 1))) {
                    ant = aux;
                    aux = aux->prox;
                }
            }
        }
    }

    if((!lista) || (!ant)) { // insere no inicio
        playerTemp->prox = lista;
        lista = playerTemp;
    }
    else { // meio ou fim
        playerTemp->prox = ant->prox;
        ant->prox = playerTemp;
    }
    return lista;
}

int main() {

    // Para construir a hash
    //THtemporada_construcao("champions.txt","hash_temporada.bin","dados_temporada.bin",10);

    FILE *fh = fopen("hash_temporada.bin","rb"), *fd = fopen("dados_temporada.bin","rb");
    if((!fh) || (!fd)) exit(1);

    int ano = 1991, hash, pos;

    while(ano < ANO_ATUAL) {
        hash = hash_temporada(ano);
        fseek(fh,hash*sizeof(int),SEEK_SET);
        fread(&pos,sizeof(int),1,fh);

        PlayerTemp *listaMelhoresAno = NULL;
        THtemporada player;
        
        while(pos != -1) {
            fseek(fd,pos,SEEK_SET);
            fread(&player,sizeof(THtemporada),1,fd);
            if((player.status) && (player.ano == ano)) {
                fclose(fh);
                fclose(fd);
                // printf("%s ",player.nome);
                listaMelhoresAno = insere_melhores_ordenado(listaMelhoresAno,&player); 
                fh = fopen("hash_temporada.bin","rb");
                fd = fopen("dados_temporada.bin","rb");
            }
            pos = player.proximo;
        }
        printf("%d\n",ano);
        imprime_melhores(listaMelhoresAno);
        printf("\n");
        libera_melhores(listaMelhoresAno);
        
        ano++;
    }    

    return 0;

}