/* 7 - acessa a hash de nacionalidade, e para todo id que encontrar na linha daquela nacionalidade, busca o id na árvore e retira */

//gcc q7.c THMSnacionalidade.h THMSnacionalidade.c THMSativos.h THMSativos.c TABM.h TABM.c TABMaux.h TABMaux.c THMSnome.c THMSnome.h -o q7

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "THMSnacionalidade.h"
#include "THMSativos.h"
#include "THMSranking.h"
#include "THMSativos.h"
#include "THMScamp.h"
#include "THMScamp_ano.h"
#include "THMStemporada.h"

#define ANO_ATUAL 2025

int main() {

    // Para construir
    THnacionalidade_construcao("tennis_players.txt", "hash_nacionalidade.bin","dados_nacionalidade.bin");

    char pais[20], sigla[4];
    int hash, pos, t, ativos;

    printf("Insira o pais: ");
    scanf(" %s",pais);
    printf("0 - Retirar aposentados\n1 - Retirar ativos");
    scanf("%d",&ativos);
    printf("t = ");
    scanf("%d",&t);

    if(!strcmp(pais,"Australia")) strcpy(sigla,"AUT");
    else if(!strcmp(pais,"Belarus")) strcpy(sigla,"BLR");
    else if(!strcmp(pais,"Great Britain")) strcpy(sigla,"GBR");
    else if(!strcmp(pais,"Greece")) strcpy(sigla,"GRC");
    else {
        strncpy(sigla, pais, 3);
        maiuscula(sigla); // Converte para maiúsculas
    }

    sigla[3] = '\0';
    hash = hash_nacionalidade(sigla);

    FILE *fp = fopen("hash_nacionalidade.bin","rb");
    if(!fp) exit(1);

    fseek(fp,hash * sizeof(int),SEEK_SET);
    fread(&pos,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("dados_nacionalidade.bin","rb");
    if(!fp) fclose;

    while(pos) {
        char nacionalidade[4], strAno[5];
        int ativo = 1, anoNascimento;
        THnacionalidade aux;
        
        fseek(fp,pos,SEEK_SET);
        fread(&aux,sizeof(THnacionalidade),1,fp);

        strncpy(nacionalidade,&aux.id[5],3);
        nacionalidade[3] = '\0';
        
        strncpy(strAno,&aux.id[1],4);
        strAno[5] = '\0';
        anoNascimento = atoi(strAno);

        if(ANO_ATUAL - anoNascimento >= 39) {
            ativo = 0;
        }
        
        if((ativo == ativos) && (!strcmp(nacionalidade,sigla))){
            // 
            Tplayer *player;
            //player = retira arvore aux.id
            
            // Remover de todas as hashs
            THnome_retira("hash_nome.bin","dados_nome.bin",player->nome);

            fclose(fp);
            THativos_retira("hash_ativos.bin","dados_ativos.bin",aux.id);
            
            THnacionalidade_retira("hash_nacionalidade.bin","dados_nacionalidade.bin",aux.id);
            
            THranking_retira_id("hash_ranking.bin","dados_ranking.bin",aux.id);

            THcamp_ano_retira("hash_campeonatos_ano.bin","dados_campeonatos_ano.bin",aux.id);

            THcamp_retira("hash_campeonatos.bin","dados_campeonatos.bin",aux.id);
            
            THtemporada_retira_id("hash_temporada.bin","dados_temporada.bin",aux.id);
        }
        fp = fopen("dados_nacionalidade.bin","rb");
        pos = aux.proximo;
    }

    return 0;
}
