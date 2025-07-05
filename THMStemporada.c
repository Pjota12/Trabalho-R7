#include "THMStemporada.h"

int hash_temporada(int ano) {
    return ano % N_TEM;
}

void THtemporada_inicializa(char *arqHash, char *arqDados) {
    FILE *fh = fopen(arqHash,"wb"), *fd = fopen(arqDados,"wb");
    if((!fh) || (!fd)) exit(1);
    // Incializa hash
    int end = -1;
    for(int i = 0; i < N_TEM; i++) fwrite(&end,sizeof(int),1,fh);
    fclose(fh);
    fclose(fd);
}

void THtemporada_retira_id(char *arqHash, char *arqDados, char *id) {
    FILE *fh = fopen(arqHash,"rb+"), *fd = fopen(arqDados,"rb+");
    if((!fh) || (!fd)) exit(1);

    int r, pos;
    r = fread(&pos,sizeof(int),1,fh);
    
    while(r) {

        THtemporada player;

        while(pos != -1) {
            fseek(fd, pos, SEEK_SET);
            fread(&player,sizeof(THtemporada),1,fd);

            if(!strcmp(player.id,id)) {
                if(player.status) {
                    player.status = 0;
                    fseek(fd, pos, SEEK_SET);
                    fwrite(&player,sizeof(THtemporada),1,fd);
                }
                
            }
            pos = player.proximo;
        }
        r = fread(&pos,sizeof(int),1,fh);
    }
    fclose(fh);
    fclose(fd);
}

void THtemporada_insere(char *arqHash, char *arqDados, THtemporada *player) {

    FILE *fh = fopen(arqHash,"rb+"), *fd = fopen(arqDados,"rb+");
    if((!fd) || (!fh)) exit(1);

    int hash, pos, ant = -1, ppl = -1;
    
    hash = hash_temporada(player->ano);

    fseek(fh, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fh);

    THtemporada aux;

    while(pos != -1) {
        fseek(fd, pos, SEEK_SET);
        fread(&aux, sizeof(THtemporada), 1, fd);
        if((aux.ano == player->ano) && (!strcmp(aux.id,player->id))) {
            aux.pontos += player->pontos;
            aux.grandSlams += player->grandSlams;
            aux.atpFinals += player->atpFinals;
            aux.olimpiadas += player->olimpiadas;
            aux.atp1000 += player->atp1000;
            fseek(fd, pos, SEEK_SET);
            fwrite(&aux,sizeof(THtemporada),1,fd);
            fclose(fh);
            fclose(fd);
            return;
        }
        
        if((!aux.status) && (ppl == -1)) ppl = pos;
        ant = pos;
        pos = aux.proximo;
    }

    player->status = 1;

    if(ppl == -1) {
        player->proximo = -1;
        fseek(fd, 0L, SEEK_END);
        pos = ftell(fd);
        fwrite(player, sizeof(THtemporada), 1, fd);
        if(ant == -1) {
            fseek(fh, hash * sizeof(int), SEEK_SET);
            fwrite(&pos, sizeof(int), 1, fh);
        }
        else {
            aux.proximo = pos;
            fseek(fd, ant, SEEK_SET);
            fwrite(&aux, sizeof(THtemporada), 1, fd);
        }
        fclose(fh);
        fclose(fd);
        return;
    }

    fseek(fd, ppl, SEEK_SET);
    fread(&aux, sizeof(THtemporada), 1, fd);
    player->proximo = aux.proximo;
    fseek(fd, ppl, SEEK_SET);
    fwrite(player, sizeof(THtemporada), 1, fd);
    
    fclose(fh);
    fclose(fd);
}

void THtemporada_imprime(char *arqHash, char *arqDados) {
    
    FILE *fh = fopen(arqHash,"rb"), *fd = fopen(arqDados,"rb");
    if((!fh) || (!fd)) exit(1);

    int ano = 1990, pos, hash;
    fread(&pos,sizeof(int),1,fh);
    hash = hash_temporada(ano);
    fseek(fh,hash*sizeof(int),SEEK_SET);
    fread(&pos,sizeof(int),1,fh);
    
    while(ano < ANO_ATUAL) {
        fseek(fd, pos, SEEK_SET);

        THtemporada player;
        int players = 0;

        printf("%d\n",ano);

        while(pos != -1) {
            fseek(fd, pos, SEEK_SET);
            fread(&player,sizeof(THtemporada),1,fd);
            if((player.status) && (player.ano == ano)) {
                players++;
                printf("%d %s %s %d %d %d %d %d\n",players,player.id,player.nome,player.pontos,player.grandSlams,player.atpFinals,player.olimpiadas,player.atp1000);
            }
            pos = player.proximo;
        }
        printf("\n");
        ano++;
        hash = hash_temporada(ano);
        fseek(fh,hash*sizeof(int),SEEK_SET);
        fread(&pos,sizeof(int),1,fh);
    }

    fclose(fh);
    fclose(fd);
}

int THtemporada_busca(char *arqHash, char *arqDados, char *id, int ano) {
    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);
    int hash, pos;

    hash = hash_temporada(ano);

    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1) return 0;
    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);
    THtemporada aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THtemporada), 1, fp);
        if(!strcmp(aux.id,id)) {
            fclose(fp);
            if(!aux.status) {
                return 0;
            }
            return aux.pontos;
        }
        pos = aux.proximo;
    }
    fclose(fp);
    return 0;
}

char *THtemporada_busca_sobrenome(char *tabHashNome, char *dadosNome, char *sobrenome) {
    FILE *fp = fopen(tabHashNome, "rb");
    if (!fp) exit(1);

    char primeiraLetra = '\0', sobrenomeComp[50];
    char *ultimoEspaco = strrchr(sobrenome, ' ');
    if(ultimoEspaco) {
        primeiraLetra = sobrenome[0];
    }
    strcpy(sobrenomeComp, ultimoEspaco ? ultimoEspaco + 1 : sobrenome);

    int pos;
    char *id = NULL;
    for (int i = 0; i < TAM_HASH; i++) {
        fseek(fp, i * sizeof(int), SEEK_SET);
        fread(&pos, sizeof(int), 1, fp);
        if (pos == -1) continue;

        FILE *fd = fopen(dadosNome, "rb");
        if (!fd) {
            fclose(fp);
            exit(1);
        }
        THnomeToid jogador;
        while (pos != -1) {
            fseek(fd, pos, SEEK_SET);
            fread(&jogador, sizeof(THnomeToid), 1, fd);
            if (jogador.status == 1) {
                // Pega o último "palavra" do nome (o sobrenome)
                char *ultimo = strrchr(jogador.nome, ' ');
                const char *sobrenome_jogador = ultimo ? ultimo + 1 : jogador.nome;
                if (strcasecmp(sobrenome_jogador, sobrenomeComp) == 0) {
                    if(primeiraLetra) {
                        if(toupper(jogador.nome[0]) == toupper(primeiraLetra)) {
                            char *id = malloc(ID_SIZE * sizeof(char));
                            strcpy(id, jogador.id);
                            fclose(fd);
                            fclose(fp);
                            return id;
                        }
                    }
                    else {
                        char *id = malloc(ID_SIZE * sizeof(char));
                        strcpy(id, jogador.id);
                        fclose(fd);
                        fclose(fp);
                        return id;
                    }
                }
            }
            pos = jogador.proximo;
        }
        fclose(fd);
    }
    fclose(fp);
    return NULL;
}

void THtemporada_construcao(char *arqChampions, char *arqHash, char *arqDados, int t) {
    
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

    THtemporada_inicializa(arqHash,arqDados);

    char linha[1024];
    fgets(linha, sizeof(linha), fp); // pula cabeçalho

    while (fgets(linha, sizeof(linha), fp)) {
        char *token = strtok(linha, "\\");
        int coluna = -1;

        THtemporada player;

        player.ano = atoi(token);
        
        while (token && coluna < 15) {

            if (coluna >= 0) {

                player.grandSlams = 0;
                player.atpFinals = 0;
                player.olimpiadas = 0;
                player.atp1000 = 0;

                if(coluna < 4) player.grandSlams = 1;
                else if(coluna < 5) player.atpFinals = 1;
                else if(coluna < 6) player.olimpiadas = 1;
                else player.atp1000 = 1;

                // Remove o conteúdo entre parênteses
                char *abreParenteses = strchr(token, '(');
                if (abreParenteses)
                    *abreParenteses = '\0';

                // Remove espaços em branco do final
                int len = strlen(token);
                while (len > 0 && isspace(token[len - 1])) token[--len] = '\0';

                if (strcmp(token, "-") != 0) {

                    char *id = THtemporada_busca_sobrenome("hash_nome.bin", "dados_nome.bin", token);
                    if (id) {
                        strcpy(player.id,id);
                        Tplayer *playerArv;
                        playerArv = buscarJogador(id,0,t);
                        strcpy(player.nome,playerArv->nome);
                    }
                    free(id);
                    
                    player.pontos = pontosPorTorneio[coluna];
                    
                    //printf("%s %s ",player.id,player.nome);
                    
                    THtemporada_insere(arqHash, arqDados, &player);
                }
            }
            token = strtok(NULL, "\\");
            coluna++;
        }
        
    }

    fclose(fp);
}

// int main() {

//     THtemporada_construcao("champions.txt","hash_temporada.bin","dados_temporada.bin",10);
//     THtemporada_imprime("hash_temporada.bin","dados_temporada.bin");
//     return 0;
// }