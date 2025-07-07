#include "THMSranking.h"

int hash_ranking(int ano) {
    return ano % N_RAN;
}

void THranking_inicializa(char *arqHash, char *arqDados) {
    FILE *fh = fopen(arqHash,"wb"), *fd = fopen(arqDados,"wb");
    if((!fh) || (!fd)) exit(1);
    // Incializa hash
    int end = -1;
    for(int i = 0; i < N_RAN; i++) fwrite(&end,sizeof(int),1,fh);
    fclose(fh);
    fclose(fd);
}

void THranking_retira_id(char *arqHash, char *arqDados, char *id) {
    FILE *fh = fopen(arqHash,"rb+"), *fd = fopen(arqDados,"rb+");
    if((!fh) || (!fd)) exit(1);

    int r, pos;
    r = fread(&pos,sizeof(int),1,fh);
    
    while(r == 1) {

        THranking player;

        while(pos != -1) {
            fseek(fd, pos, SEEK_SET);
            fread(&player,sizeof(THranking),1,fd);

            if(!strcmp(player.id,id)) {
                // if(!strcmp(aux.nome,player->nome)) {
                if(player.status) {
                    player.status = 0;
                    fseek(fd, pos, SEEK_SET);
                    fwrite(&player,sizeof(THranking),1,fd);
                }
                
            }
            pos = player.proximo;
        }
        r = fread(&pos,sizeof(int),1,fh);
    }
    fclose(fh);
    fclose(fd);
}

void THranking_retira(char *arqHash, char *arqDados, THranking *player) {
    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);
    int hash, pos;
    
    hash = hash_ranking(player->ano);

    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1) return;
    fp = fopen(arqDados,"rb+");
    if(!fp) exit(1);
    THranking aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THranking), 1, fp);
        if(!strcmp(aux.id,player->id)) {
        // if(!strcmp(aux.nome,player->nome)) {
            if(!aux.status) {
                fclose(fp);
                return;
            }
            aux.status = 0;
            fseek(fp, pos, SEEK_SET);
            fwrite(&aux,sizeof(THranking),1,fp);
            fclose(fp);
            return;
        }
        pos = aux.proximo;
    }
    fclose(fp);
    return;
}

void THranking_imprime(char *arqHash, char *arqDados) {
    
    FILE *fh = fopen(arqHash,"rb"), *fd = fopen(arqDados,"rb");
    if((!fh) || (!fd)) exit(1);

    int ano = 1990, pos, hash;
    fread(&pos,sizeof(int),1,fh);
    hash = hash_ranking(ano);
    fseek(fh,hash*sizeof(int),SEEK_SET);
    fread(&pos,sizeof(int),1,fh);
    
    while(ano < ANO_ATUAL) {
        fseek(fd, pos, SEEK_SET);

        THranking player;
        int players = 0;

        printf("%d\n",ano);

        while(pos != -1) {
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
        hash = hash_ranking(ano);
        fseek(fh,hash*sizeof(int),SEEK_SET);
        fread(&pos,sizeof(int),1,fh);
    }

    fclose(fh);
    fclose(fd);
}

int THranking_busca(char *arqHash, char *arqDados, char *id, int ano) {
    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);
    int hash, pos;

    hash = hash_ranking(ano);

    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1) return 0;
    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);
    THranking aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THranking), 1, fp);
        if(!strcmp(aux.id,id)) {
            fclose(fp);
            if(!aux.status) {
                return 0;
            }
            return 1;
        }
        pos = aux.proximo;
    }
    fclose(fp);
    return 0;
}

void THranking_insere(char *arqHash, char *arqDados, THranking *player) {

    FILE *fh = fopen(arqHash,"rb+"), *fd = fopen(arqDados,"rb+");
    if((!fh) || (!fd)) exit(1);

    //printf("Inserindo %s %s\n",player->nome,player->id);

    int hash, pos, ant = -1;
    
    hash = hash_ranking(player->ano);
    
    fseek(fh, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fh);
    
    THranking aux, anterior;
    
    while((pos != -1)) { // procura na linha
        fseek(fd, pos, SEEK_SET);
        fread(&aux, sizeof(THranking), 1, fd);
        // if((aux.status) && (!strcmp(aux.nome,player->nome))) {
        if((aux.status) && (aux.ano == player->ano) && (!strcmp(aux.id,player->id))) {
        
            aux.status = 0;
            fseek(fd,pos,SEEK_SET);
            fwrite(&aux,sizeof(THranking),1,fd);
            fclose(fh);
            fclose(fd);
            
            aux.pontos += player->pontos;
            THranking_insere(arqHash,arqDados,&aux);
            return;
        }
        ant = pos;
        pos = aux.proximo;
    }

    ant = -1;
    fseek(fh, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fh);
    
    while((pos != -1)) { // encontra pos certa
        fseek(fd, pos, SEEK_SET);
        fread(&aux, sizeof(THranking), 1, fd);
        if(player->pontos > aux.pontos) break;
        anterior = aux;
        ant = pos;
        pos = aux.proximo;
    }

    THranking elem;

    strcpy(elem.nome,player->nome);
    strcpy(elem.id,player->id);
    elem.ano = player->ano;
    elem.pontos = player->pontos;
    elem.status = 1;
    if(ant == -1) { // inicio
        elem.proximo = pos;
    }
    else {
        elem.proximo = anterior.proximo;
    }

    fseek(fd, 0L, SEEK_END);
    pos = ftell(fd);
    fwrite(&elem, sizeof(THranking), 1, fd); 

    if(ant == -1) { // inicio
        fseek(fh, hash * sizeof(int), SEEK_SET);
        fwrite(&pos, sizeof(int), 1, fh);
    }
    else { // meio ou fim
        anterior.proximo = pos;
        fseek(fd, ant, SEEK_SET);
        fwrite(&anterior, sizeof(THranking), 1, fd);
    }
    fclose(fh);
    fclose(fd);
}

void THranking_copia(char *arqHash, char *arqDados, int ano) {
    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);
    
    int hash, pos;
    hash = hash_ranking(ano);
    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);

    if(pos == -1) return;
    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);

    THranking aux;

    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THranking), 1, fp);
        if(aux.status) {
            aux.ano = ano + 1;
            fclose(fp);
            THranking_insere(arqHash, arqDados, &aux);
            fp = fopen(arqDados,"rb");
        }
        pos = aux.proximo;
    }
    fclose(fp);
}

char *THranking_busca_sobrenome(char *tabHashNome, char *dadosNome, char *sobrenome) {
    FILE *fp = fopen(tabHashNome, "rb");
    if (!fp) exit(1);

    FILE *fd = fopen(dadosNome, "rb");
    if (!fd) {
        fclose(fp);
        exit(1);
    }

    char nome[50];
    char inicial = '\0';

    while (*sobrenome && isspace(*sobrenome)) sobrenome++;  // tira espaço no início
    int len = strlen(sobrenome);
    while (len > 0 && isspace(sobrenome[len - 1])) sobrenome[--len] = '\0';  // tira espaço no fim

    // Agora: se for "A. Medvedev", separa
    char *espaco = strchr(sobrenome, ' ');
    if (espaco && espaco - sobrenome == 2 && sobrenome[1] == '.') {
        inicial = sobrenome[0];
        strcpy(nome, espaco + 1);
    } else {
        // Caso comum: só o sobrenome (ou composto)
        strcpy(nome, sobrenome);
    }

    // Busca na hash
    for (int i = 0; i < TAM_HASH; i++) {
        int pos;
        fseek(fp, i * sizeof(int), SEEK_SET);
        fread(&pos, sizeof(int), 1, fp);
        if (pos == -1) continue;

        while (pos != -1) {
            THnomeToid jogador;
            fseek(fd, pos, SEEK_SET);
            fread(&jogador, sizeof(THnomeToid), 1, fd);
            if (jogador.status == 1) {
                int len_nome = strlen(jogador.nome);
                int len_sub = strlen(nome);

                if (len_nome >= len_sub &&
                    strcasecmp(jogador.nome + len_nome - len_sub, nome) == 0) {
                    
                    if (inicial) {
                        if (toupper(jogador.nome[0]) == toupper(inicial)) {
                            char *id = malloc(ID_SIZE);
                            strcpy(id, jogador.id);
                            fclose(fd);
                            fclose(fp);
                            return id;
                        }
                    } else {
                        char *id = malloc(ID_SIZE);
                        strcpy(id, jogador.id);
                        fclose(fd);
                        fclose(fp);
                        return id;
                    }
                }
            }
            pos = jogador.proximo;
        }
    }

    fclose(fd);
    fclose(fp);
    return NULL;
}

void THranking_construcao(char *arqChampions, char *arqHash, char *arqDados, int t) {
    
    FILE *fp = fopen(arqChampions, "r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de campeões.\n");
        exit(1);
    }

    int pontosPorTorneio[15] = {
        2000, 2000, 2000, 2000, // Grand Slams
        0,                     // ATP Finals
        0,                     // Olimpíadas
        1000,1000,1000,1000,1000,1000,1000,1000,1000 // ATP 1000
    };

    THranking_inicializa(arqHash,arqDados);

    char linha[1024];
    fgets(linha, sizeof(linha), fp); // pula cabeçalho

    while (fgets(linha, sizeof(linha), fp)) {
        char *token = strtok(linha, "\\");
        int coluna = -1, anoAtual;

        THranking player;

        anoAtual =  atoi(token);
        player.ano = anoAtual;
        
        while (token && coluna < 15) {

            if (coluna >= 0) {
                // Remove o conteúdo entre parênteses
                char *abreParenteses = strchr(token, '(');
                if (abreParenteses)
                    *abreParenteses = '\0';

                // Remove espaços em branco do final
                int len = strlen(token);
                while (len > 0 && isspace(token[len - 1])) token[--len] = '\0';

                if (strcmp(token, "-") != 0) {

                    char *id = THranking_busca_sobrenome("hash_nome.bin", "dados_nome.bin", token);
                    if (id) {
                        strcpy(player.id,id);
                        Tplayer *playerArv;
                        playerArv = buscarJogador(id,0,t);
                        strcpy(player.nome,playerArv->nome);
                        free(playerArv);
                        free(id);
                    }
                    
                    player.pontos = pontosPorTorneio[coluna];
                    
                    //printf("%s %s ",player.id,player.nome);
                    
                    THranking_insere(arqHash, arqDados, &player);
                }
            }
            token = strtok(NULL, "\\");
            coluna++;
        }

        THranking_copia(arqHash, arqDados, anoAtual);
    }
    
    //THranking_imprime(arqHash, arqDados);

    fclose(fp);
}

// int main() {

//     THranking_construcao("champions.txt","hash_ranking.bin","dados_ranking.bin",10);
//     THranking_imprime("hash_ranking.bin","dados_ranking.bin");
//     return 0;
// }