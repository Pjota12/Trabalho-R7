#include "q2.h"

/*2- hash de nacionalidade que informe o id do jogador(MEMORIA SECUNDARIA) -> ja tem o nascimento no id -> procura jogadores na mesma hash 
que possuem rank e, se ganhou um grandslan no ano respectivo do nascimento*/

// acha o ano em que ele ganhou um grand slam, verifica se ele está no ranking naquele ano e busca na hash de nacionalidade compatriotas que nasceram naquele ano

// gcc q2.c THMSnacionalidade.h THMSnacionalidade.c TABM.h TABM.c TABMaux.h TABMaux.c THMSnome.c THMSnome.h  THMSranking.h THMSranking.c THMScamp.h THMScamp.c -o q2

void buscarCompatriotas(char *arqHash, char *arqDados, char* nacionalidade, int ano, int t){
    FILE *fp = fopen(arqHash, "rb");
    if(!fp) {
        printf("Erro ao abrir hash nacionalidade %s\n", arqDados);  // <---- ESSENCIAL
        exit(1);
    }

    //printf("Hash nacionalidade aberta com sucesso\n");

    int hash = hash_nacionalidade(nacionalidade), pos, achou = 0;

    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    //printf(">> Hash nacionalidade %s deu posição %d\n", nacionalidade, pos);
    fclose(fp);

    fp = fopen(arqDados,"rb");
    if(!fp) {
        printf("Erro ao abrir dados nacionalidade %s\n", arqDados);  // <---- ESSENCIAL
        exit(1);
    }

    THnacionalidade aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THnacionalidade), 1, fp);
        
        char anoNasc[5], nac_aux[4];
        strncpy(anoNasc,&aux.id[1],4);
        anoNasc[4] = '\0';
        strncpy(nac_aux,&aux.id[5],3);
        nac_aux[3] = '\0';

        if((atoi(anoNasc) == ano) && (!strcmp(nacionalidade,nac_aux)) && (aux.status)) {
            Tplayer *jogador = buscarJogador(aux.id, 0, t);
            printf("%s %s nasceu em %d\n", aux.id, jogador->nome, ano);
            achou = 1;
            free(jogador);
        }
        pos = aux.proximo;
    }

    if (!achou) printf("Não há compatriotas nascidos nesse ano!\n ");
    printf("\n");
    fclose(fp);
}

/*void percorreRank(char *arqHash, char *arqDados, char* id, int ano, int t){
    FILE *fp = fopen(arqHash, "rb");
    if (!fp) exit(1);
    //printf("Hash ranking aberta\n");

    int hash = hash_ranking(ano), pos, cont = 0;
    
    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);

    fp = fopen(arqDados,"rb");
    if (!fp) exit(1);
    //printf("Dados ranking abertos\n");

    THranking aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THranking), 1, fp);
        //printf("cont: %dn", cont);
        if (cont <= 25){
            if (!strcmp(aux.id, id)) {
                char nacionalidade[4] = {0};
                strncpy(nacionalidade, &id[5], 3);
                //printf("Nacionalidade: %s\n", nacionalidade);
                buscarCompatriotas("hash_nacionalidade.bin","dados_nacionalidade.bin", nacionalidade, ano, t);
            }
        }
        cont++;
        pos = aux.proximo;
    }
    fclose(fp);
}*/

void percorreGS(char *arqHash, char *arqDados, int coluna, int t){
    FILE *fp = fopen(arqHash, "rb");
    if (!fp) exit(1);
    
    int hash = hash_camp(coluna), pos;
    
    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    
    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);
    
    THcamp aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        
        fread(&aux, sizeof(THcamp), 1, fp);
        if(aux.status) {
            Tplayer *jogador = buscarJogador(aux.id,0,t);
            printf("Compatriotas de %s %s:\n ", aux.id, jogador->nome);
        
            if(jogador->ranking) {
                char nacionalidade[4];
                strncpy(nacionalidade,&aux.id[5],3);
                nacionalidade[3] = '\0';
                buscarCompatriotas("hash_nacionalidade.bin","dados_nacionalidade.bin", nacionalidade, aux.ano, t);
            }
        
            free(jogador);
        }
        
        pos = aux.prox;
    }
    fclose(fp);
}

void Questao2(int t){
    // Para construir a hash
    // THcamp_construcao("champions.txt","hash_campeonatos.bin","dados_campeonatos.bin");

    //THranking_construcao("champions.txt","hash_ranking.bin","dados_ranking.bin",t);

    // THnacionalidade_construcao("tennis_players.txt", "hash_nacionalidade.bin","dados_nacionalidade.bin");

    for (int i = 0; i < 4; i++){
        percorreGS("hash_campeonatos.bin","dados_campeonatos.bin", i, t);
    }
}