#include "THMScamp_ano.h"
#include "TABM.h"

/*5- pecorrer arquivo -> verificar se nas 4 linhas é o mesmo nome -> acessar tabela de id sobrenames => id -> retornar nome do 
cara e o ano*/

// gcc q5.c TABM.h TABM.c THMScamp_ano.h THMScamp_ano.c -o q5

int vencedorGS(char *arqHash, char *arqDados, int ano, int t){
    FILE *fp = fopen(arqHash, "rb");
    if (!fp) exit(1);

    int hash = hash_camp_ano(ano), pos, iguais = 1, cont = 0;
    char id[ID_SIZE];
    
    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);

    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);

    THcamp_ano aux;
    while(pos != -1 && cont < 4) { // percorre só os 4 primeiros (os Grand Slams)
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THcamp_ano), 1, fp);
        if (!cont) {
            strcpy(id, aux.id); // salva o primeiro
        }
        if (strcmp(aux.id, id) != 0) {
                iguais = 0; // se algum for diferente do primeiro, marca como falso e quebra o loop
                break;
        }
        cont++;
        pos = aux.prox;
    }

    if (iguais) {
        Tplayer *jogador = buscarJogador(id, 0, t);
        printf("%s venceu os 4 Grand Slams em %d.\n", id, ano);
        return 1;
    }

    fclose(fp);
    return 0;
}

int main(){
    int t, cont = 0;

    printf("t: ");
    scanf("%d", &t);

    // Para construir a hash
    //THcamp_ano_construcao("champions.txt","hash_campeonatos_ano.bin","dados_campeonatos_ano.bin");
    
    for (int i = 1990; i < 2025; i++){
        cont += vencedorGS("hash_campeonatos_ano.bin","dados_campeonatos_ano.bin", i, t);
    }

    if (!cont) printf(("Ninguém nunca venceu os 4 Grand Slams."));

    return 0;
}
