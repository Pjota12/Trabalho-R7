
/*9- acessar hash de campeonato -> para o campeonato respectivo contar as hash -> salvar vetor -> alocar vetor com nome e pontução */

// gcc q9.c THMScamp.h THMScamp.c TABM.h TABM.c TABMaux.h TABMaux.c -o q9

#include "q9.h"

void imprime_jogadores9(PlayerTitulos9* jogadores) {
    PlayerTitulos9* aux = jogadores;
    while (aux){
        printf("%s: %d \n", aux->nome, aux->cont);
        aux = aux->prox;
    }
}

void libera_titulos9(PlayerTitulos9 *jogadores) { // libera a lista
    PlayerTitulos9 *temp;
    while (jogadores){
        temp = jogadores;
        jogadores = jogadores->prox;
        free(temp);
    }
}

PlayerTitulos9 *insere_titulos_ordenado9(PlayerTitulos9 *lista, Tplayer *player, char *id, int cont) {
    PlayerTitulos9 *playerTitulos9 = (PlayerTitulos9 *)malloc(sizeof(PlayerTitulos9));

    strcpy(playerTitulos9->id, id);
    strcpy(playerTitulos9->nome, player->nome);
    playerTitulos9->cont = cont;

    PlayerTitulos9 *aux = lista, *ant = NULL;
    while((aux) && (playerTitulos9->cont < aux->cont)) {
        ant = aux;
        aux = aux->prox;
    }
    
    if((aux) && (playerTitulos9->cont == aux->cont)) {
        while((aux) && (strcmp(playerTitulos9->nome, aux->nome) > 0)) {
            ant = aux;
            aux = aux->prox;
        }
    }

    if((!lista) || (!ant)) { // insere no inicio
        playerTitulos9->prox = lista;
        lista = playerTitulos9;
    }
    else { // meio ou fim
        playerTitulos9->prox = ant->prox;
        ant->prox = playerTitulos9;
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

int checaRep(PlayerTitulos9 *lista, char *id) {
    while (lista) {
        if (strcmp(lista->id, id) == 0) return 1;
        lista = lista->prox;
    }
    return 0;
}

void percorreCamp9(char *arqHash, char *arqDados, int coluna, int t){
    FILE *fp = fopen(arqHash, "rb");
    if(!fp) exit(1);
    
    PlayerTitulos9 *lista = NULL;

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
        if(aux.status) { // se o status for 0, pula para o próximo
            Tplayer *jogador = buscarJogador(aux.id,0,t);
            cont = contaTitulos(fp, arqHash, coluna, aux.id);
            if (!checaRep(lista, aux.id)) lista = insere_titulos_ordenado9(lista, jogador, aux.id, cont);    
            free(jogador); // libera a memória alocada para o jogador
        }    
        pos = aux.prox;
    }

    imprime_jogadores9(lista);
    libera_titulos9(lista);

    fclose(fp);
}

void Questao9(int t) {

    // Para construir a hash
    //THcamp_construcao("tennis_players.txt","hash_campeonatos.bin","dados_campeonatos.bin");

    char *nomes_torneios[15] = {"Australian Open", "Roland Garros", "Wimbledon", "US Open", "ATP Finals", "Olimpíadas", "Indian Wells", "Miami", "Monte Carlo", "Madrid", "Rome", "Canada", "Cincinnati", "Shanghai", "Paris"};

    for (int i = 0; i < 15; i++){
        printf("\n%s: \n", nomes_torneios[i]);
        percorreCamp9("hash_campeonatos.bin", "dados_campeonatos.bin", i, t);
    }
}
