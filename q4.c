/*4- pecorrer o arquivo -> criar struct para cada jogador -> inserir cada jogador ordenado por pontos -> imprimir os 25 primeiros
    para cada ano verificar se o jogador ja existe na lista -> se existir, somar os pontos, se não existir, criar novo jogador
    -> ordenar por pontos -> imprimir os 25 primeiros jogadores
*/

// gcc q4.c THMSranking.h THMSranking.c TABM.c TABM.h TABMaux.c TABMaux.h THMSnome.c THMSnome.h -o q4

#include "THMSranking.h"

void imprime_ranking_ano(char *arqHash, char *arqDados) {
    
    FILE *fh = fopen(arqHash,"rb"), *fd = fopen(arqDados,"rb");
    if((!fh) || (!fd)) exit(1);

    int ano = 1990, pos, hash;
    fread(&pos,sizeof(int),1,fh);
    
    while(ano < ANO_ATUAL) {
        hash = hash_ranking(ano);
        fseek(fh,hash*sizeof(int),SEEK_SET);
        fread(&pos,sizeof(int),1,fh);

        fseek(fd, pos, SEEK_SET);

        THranking player;
        int players = 0;

        printf("%d\n",ano);

        while((pos != -1) && (players < 25)) {
            fseek(fd, pos, SEEK_SET);
            fread(&player,sizeof(THranking),1,fd);
            if((player.status) && (player.ano == ano)) {
                players++;
                printf("%d %s %s %d\n",players,player.id,player.nome,player.pontos);
            }
            pos = player.proximo;
        }
        printf("\n");
        ano++;
    }

    fclose(fh);
    fclose(fd);
}

int main() {

    int t = 10;
    printf("t = ");
    scanf("%d",&t);
    THranking_construcao("champions.txt","hash_ranking.bin","dados_ranking.bin",t);
    imprime_ranking_ano("hash_ranking.bin","dados_ranking.bin");
    return 0;
}



