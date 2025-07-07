/* 7 - acessa a hash de nacionalidade, e para todo id que encontrar na linha daquela nacionalidade, busca o id na árvore e retira */

//gcc q7.c THMSnacionalidade.h THMSnacionalidade.c THMSativos.h THMSativos.c TABM.h TABM.c TABMaux.h TABMaux.c THMSnome.c THMSnome.h -o q7

#include "q7.h"

void Questao7(int t,int *numeroDeJogadores) {


    char pais[20], sigla[4];
    int hash, pos, ativos;

    printf("Insira o pais: ");
    scanf(" %[^\n]",pais);
    printf("\n0 - Retirar aposentados\n1 - Retirar ativos\n");
    scanf("%d",&ativos);

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
    if(!fp) exit(1);

    while(pos != -1) {
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


            printf("Removendo jogador: %s\n", aux.id);
            Tplayer *player;
            player = buscarJogador(aux.id,0,t); // Busca o jogador na árvore B+
            if(player){
                free(player); // Libera a memória alocada para o jogador
                player = Removerjogador("index.bin",aux.id,t,0,0);
                // Remover de todas as hashs
                (*numeroDeJogadores)--;
                printf("\nJogador removido: %s\n", player->nome);

                printf("retirando hashnome\n");
                THnome_retira("hash_nome.bin","dados_nome.bin",player->nome);

                fclose(fp);
                printf("retirando hashAtivo\n");
                THativos_retira("hash_ativos.bin","dados_ativos.bin",aux.id);
                
                printf("retirando hashNacionalidade\n");
                THnacionalidade_retira("hash_nacionalidade.bin","dados_nacionalidade.bin",aux.id);
                
                printf("retirando hashRanking\n");
                THranking_retira_id("hash_ranking.bin","dados_ranking.bin",aux.id);

                printf("retirando hashCampAno\n");
                THcamp_ano_remove("hash_campeonatos_ano.bin","dados_campeonatos_ano.bin",aux.id);

                printf("retirando hashCamp\n");
                THcamp_remove("hash_campeonatos.bin","dados_campeonatos.bin",aux.id);
                
                printf("retirando hashTemporada\n");
                THtemporada_retira_id("hash_temporada.bin","dados_temporada.bin",aux.id);

                free(player); // Libera a memória alocada para o jogador

                fp = fopen("dados_nacionalidade.bin","rb");
            }else{
                printf("Jogador com id %s não encontrado na árvore B+\n", aux.id);
            }
            
        }
        pos = aux.proximo;
    }
    fclose(fp);
}
