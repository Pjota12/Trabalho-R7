#include "THMSranking.h"
#include "THMSnacionalidade.h"
#include "THMScamp.h"
#include "TABM.h"

/*6- hash do ranking 25 e hash do campeonato (por carpeonato (4)) -> pecorre o campeonato especifico -> busca o jogador -> busca osanos que ganhou o campeonato e verifica se ele esta no rank ou não em cada ano no vetor*/

// gcc q6.c TABM.h TABM.c TABMaux.h TABMaux.c THMSranking.h THMSranking.c THMScamp.h THMScamp.c -o q6

void percorreRank(char *arqHash, char *arqDados, char* id, int ano, int t){
    FILE *fp = fopen(arqHash, "rb");
    if (!fp) exit(1);

    int hash = hash_ranking(ano), pos, cont = 0;
    
    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);

    fp = fopen(arqDados,"rb");
    if (!fp) exit(1);

    THranking aux;
    while((pos != -1) && (cont < 25)) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THranking), 1, fp);
        if(aux.status) {      
            cont++;
            if (!strcmp(aux.id, id)) {
                fclose(fp);
                return; // está no top 25, logo não “furou”
            }
        } 
        pos = aux.proximo;
    }
    fclose(fp);

    Tplayer* jogador = buscarJogador(id, 0, t);
    printf("%s %s\n", id, jogador->nome);
}

void percorreCamp(char *arqHash, char *arqDados, int coluna, int t){
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
        percorreRank("hash_ranking.bin", "dados_ranking.bin", aux.id, aux.ano, t);
        pos = aux.prox;
    }
    fclose(fp);
}

int main(){
    int t;
    printf("t: ");
    scanf("%d", &t);

    // Para construir a hash
    // THcamp_construcao("champions.txt","hash_campeonatos.bin","dados_campeonatos.bin");
    // THranking_construcao("champions.txt","hash_ranking.bin","dados_ranking.bin",t);
    
    printf("\nJogadores que ""furaram"" o ranking: \n");
    for (int i = 0; i < 15; i++){
        if (i == 0){
            printf("\nGrand Slams:\n");
        }
        if (i == 4){
            printf("\nATP Finals:\n");
        }
        if (i == 5){
            printf("\nOlímpiadas:\n");
        }
        if (i == 6){
            printf("\nATP 1000:\n");
        }
        percorreCamp("hash_campeonatos.bin","dados_campeonatos.bin", i, t);
    }
    return 0;
}