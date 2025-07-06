
/*9- acessar hash de campeonato -> para o campeonato respectivo contar as hash -> salvar vetor -> alocar vetor com nome e pontução */

// gcc q9.c THMScamp.h THMScamp.c TABM.h TABM.c TABMaux.h TABMaux.c -o q9

#include "TABM.h"
#include "TABMaux.h"
#include "THMScamp.h"

typedef struct PlayerTitulos {
    char id[ID_SIZE];
    char nome[50];
    int cont;
    struct PlayerTitulos *prox;
} PlayerTitulos;

void imprime_jogadores(PlayerTitulos* jogadores) {
    PlayerTitulos* aux = jogadores;
    while (aux){
        printf("%s: %d \n", aux->nome, aux->cont);
        aux = aux->prox;
    }
}

void libera_titulos(PlayerTitulos *jogadores) { // libera a lista
    PlayerTitulos *temp;
    while (jogadores){
        temp = jogadores;
        jogadores = jogadores->prox;
        free(temp);
    }
}

PlayerTitulos *insere_titulos_ordenado(PlayerTitulos *lista, Tplayer *player, char *id, int cont) {
    PlayerTitulos *playerTitulos = (PlayerTitulos *)malloc(sizeof(PlayerTitulos));

    strcpy(playerTitulos->id, id);
    strcpy(playerTitulos->nome, player->nome);
    playerTitulos->cont = cont;

    PlayerTitulos *aux = lista, *ant = NULL;
    while((aux) && (playerTitulos->cont < aux->cont)) {
        ant = aux;
        aux = aux->prox;
    }
    
    if((aux) && (playerTitulos->cont == aux->cont)) {
        while((aux) && (strcmp(playerTitulos->nome, aux->nome) > 0)) {
            ant = aux;
            aux = aux->prox;
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

int contaTitulos(FILE *fd, char *arqHash, int coluna, char *id){
    FILE *fh = fopen(arqHash, "rb");
    if (!fh) exit(1);

    int hash = hash_camp(coluna), pos, cont = 0;

    fseek(fh, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fh);
    fclose(fh);

    while (pos != -1) {
        THcamp aux;
        fseek(fd, pos, SEEK_SET);
        fread(&aux, sizeof(THcamp), 1, fd);
        if (strcmp(aux.id, id) == 0) {
            cont++;
        }
        pos = aux.prox;
    }

    return cont;
}

int checaRep(PlayerTitulos *lista, char *id) {
    while (lista) {
        if (strcmp(lista->id, id) == 0) return 1;
        lista = lista->prox;
    }
    return 0;
}

void percorreCamp(char *arqHash, char *arqDados, int coluna, int t){
    FILE *fp = fopen(arqHash, "rb");
    if(!fp) exit(1);
    
    PlayerTitulos *lista = NULL;

    int hash = hash_camp(coluna), pos, cont = 0;

    fseek(fp,hash*sizeof(int),SEEK_SET);
    fread(&pos,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);

    while(pos != -1) {
        THcamp aux;
        fseek(fp,pos,SEEK_SET);
        fread(&aux,sizeof(THcamp),1,fp);
        Tplayer *jogador = buscarJogador(aux.id,0,t);
        cont = contaTitulos(fp, arqHash, coluna, aux.id);
        if (!checaRep(lista, aux.id)) lista = insere_titulos_ordenado(lista, jogador, aux.id, cont);    
        pos = aux.prox;
    }

    imprime_jogadores(lista);
    libera_titulos(lista);

    fclose(fp);
}

int main() {

    // Para construir a hash
    //THcamp_construcao("tennis_players.txt","hash_campeonatos.bin","dados_campeonatos.bin");

    int t;

    printf("t = ");
    scanf("%d",&t);

    char *nomes_torneios[15] = {"Australian Open", "Roland Garros", "Wimbledon", "US Open", "ATP Finals", "Olimpíadas", "Indian Wells", "Miami", "Monte Carlo", "Madrid", "Rome", "Canada", "Cincinnati", "Shanghai", "Paris"};

    for (int i = 0; i < 15; i++){
        printf("\n%s: \n", nomes_torneios[i]);
        percorreCamp("hash_campeonatos.bin", "dados_campeonatos.bin", i, t);
    }

    return 0;
}
