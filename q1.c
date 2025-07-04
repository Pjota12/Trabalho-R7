//1- hash de aposentados que informe o id do jogador(MENMORIA SECUNDARIA) -> contar quantos aposentados/ativos
/*  -> informar id pela hash -> buscar id na arvore -> alocar vetor de player no tamanho -> ordenar por numero de titulos
    ->printar por estilo da questão(grand slams,ATP 1000,ATP Finals,Olimpíadas)
*/

//gcc q1.c THMSativos.h THMSativos.c TABM.h TABM.c TABMaux.h TABMaux.c THMSnome.c THMSnome.h -o q1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "THMSativos.h"
#include "TABM.h"

typedef struct PlayerTitulos {
    char id[ID_SIZE];
    char nome[50];
    int grand_slams;
    int atp_1000;
    int atp_finals;
    int olimpiadas;
    int total_titulos;
    struct PlayerTitulos *prox;
} PlayerTitulos;

void imprime_jogadores(PlayerTitulos* jogadores, int aposentado) {
    if (aposentado) printf("\nAposentados:\n\n");
    else printf("\nAtivos: \n\n");
    PlayerTitulos* aux = jogadores;
    while (aux){
        if (aux->total_titulos > 0) {
            printf("%s %s: GS = %d, ATP1000 = %d, Finals = %d, Olimp = %d, Total = %d\n", aux->id, aux->nome, aux->grand_slams, aux->atp_1000, aux->atp_finals, aux->olimpiadas, aux->total_titulos);
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

PlayerTitulos *insere_ordenado(PlayerTitulos *lista, Tplayer *player) {
    PlayerTitulos *playerTitulos = (PlayerTitulos *)malloc(sizeof(PlayerTitulos));
    // ID
    strcpy(playerTitulos->id,player->id);
    // Nome
    strcpy(playerTitulos->nome,player->nome);
    // Grand Slams
    playerTitulos->grand_slams = player->quantidadeTorneios[0] + player->quantidadeTorneios[1] + player->quantidadeTorneios[2] + player->quantidadeTorneios[3];
    // ATP Finals
    playerTitulos->atp_finals = player->quantidadeTorneios[4];
    // Olimpiadas
    playerTitulos->olimpiadas = player->quantidadeTorneios[5];
    // ATP 1000
    playerTitulos->atp_1000 = player->quantidadeTorneios[6] + player->quantidadeTorneios[7] + player->quantidadeTorneios[8] + player->quantidadeTorneios[9] + player->quantidadeTorneios[10] + player->quantidadeTorneios[11] + player->quantidadeTorneios[12] + player->quantidadeTorneios[13] + player->quantidadeTorneios[14];
    // Total
    playerTitulos->total_titulos = playerTitulos->grand_slams + playerTitulos->atp_finals + playerTitulos->olimpiadas + playerTitulos->atp_1000;

    PlayerTitulos *aux = lista, *ant = NULL;
    while((aux) && (playerTitulos->total_titulos < aux->total_titulos)) {
        ant = aux;
        aux = aux->prox;
    }
    
    if((aux) && (playerTitulos->total_titulos == aux->total_titulos)) {
        while((aux) && (playerTitulos->total_titulos == aux->total_titulos) && (playerTitulos->grand_slams < aux->grand_slams)) {
            ant = aux;
            aux = aux->prox;
        }
        if((aux) && (playerTitulos->grand_slams == aux->grand_slams)) {
            while((aux) && (playerTitulos->grand_slams == aux->grand_slams) && (playerTitulos->atp_1000 < aux->atp_1000)) {
                ant = aux;
                aux = aux->prox;
            }
            if((aux) && (playerTitulos->atp_1000 == aux->atp_1000)) {
                while((aux) && (playerTitulos->atp_1000 == aux->atp_1000) && (playerTitulos->atp_finals < aux->atp_finals)) {
                    ant = aux;
                    aux = aux->prox;
                }
                if((aux) && (playerTitulos->atp_finals == aux->atp_finals)) {
                    while((aux) && (playerTitulos->atp_finals == aux->atp_finals) && (playerTitulos->olimpiadas < aux->olimpiadas)) {
                        ant = aux;
                        aux = aux->prox;
                    }
                }
            }
        }
    }
    
    if((!lista) || (!ant)) { // insere no inicio
        playerTitulos->prox = lista;
        lista = playerTitulos;
    }
    else { // meio ou fim
        playerTitulos->prox = ant->prox;
        ant->prox = playerTitulos;
    }
    return lista;
}

int main() {

    // Para construir a hash
    //THativos_construcao("tennis_players.txt","hash_ativos.bin","dados_ativos.bin");

    FILE *fp = fopen("hash_ativos.bin","rb");
    if(!fp) exit(1);
    int hash, pos, t = 10;

    printf("t = ");
    scanf("%d",&t);
    printf("0 - aposentados\n1 - ativos\n");
    scanf("%d",&hash);

    fseek(fp,hash*sizeof(int),SEEK_SET);
    fread(&pos,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("dados_ativos.bin","rb");
    if(!fp) exit(1);

    PlayerTitulos *listaPlayerTitulos = NULL;
    
    while(pos != -1) {
        THativos aux;
        fseek(fp,pos,SEEK_SET);
        fread(&aux,sizeof(THativos),1,fp);
        Tplayer *player = buscarJogador(aux.id,0,10);

        listaPlayerTitulos = insere_ordenado(listaPlayerTitulos, player);
        
        pos = aux.proximo;
    }
    
    imprime_jogadores(listaPlayerTitulos, !hash);
    libera(listaPlayerTitulos);

    return 0;
}