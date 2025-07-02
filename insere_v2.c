#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ID_SIZE 13
#define ARQ_SIZE 11

typedef struct player {
    char id[ID_SIZE];          // ID usado na árvore "1231986BRA"  
    char nome[50];
    int anoNascimento;
    int anoMorte;
    char nacionalidade[4];     // Pode ser codificado ESP, BRA, ARG, etc.
    int ranking;             // melhor ranking do jogador
    int anoRanking;         // Ano do melhor ranking
    int semanasNoTopo;  // Semanas no topo do ranking
    int pontosTotais;       //pontos totais
    int quantidadeTorneios[15]; // grand slams = [0] - [3] | [4] ATP Finals | [5] Olimpiadas | [6-14] ATP 1000
}Tplayer;

int compara_str(const void *a, const void *b) {
    const char *pa = (const char *) a;
    const char *pb = (const char *) b;
    return strcmp(pa, pb);
}

int compara_por_id(const void *a, const void *b) {
    Tplayer *p1 = (Tplayer*) a;
    Tplayer *p2 = (Tplayer*) b;
    return strcmp(p1->id, p2->id);
}

void dividir_folha(char *indice, int t, int *folhas, int *nos, char *folha, int no, int bloco) {
    printf("Dividir folha %s ",folha);
    char novaFolha[ARQ_SIZE] = "f", strFolhas[10] = "";
    sprintf(strFolhas, "%d", *folhas + 1);
    strcat(novaFolha,strFolhas);
    strcat(novaFolha,".bin");

    FILE *fi, *ff1 = fopen(folha,"rb+"), *ff2 = fopen(novaFolha,"wb"), *temp = fopen("temp.bin","wb");
    if((!ff1) || (!ff2) || (!temp)) exit(1);

    int i, j, nElems, nChaves = 1;
    char proximaFolha[ARQ_SIZE] = "", id[ID_SIZE] = "", filho[ARQ_SIZE] = "";

    fseek(ff1,0L,SEEK_END);
    i = ftell(ff1);
    nElems = i/sizeof(Tplayer);

    Tplayer players[nElems];

    fseek(ff1,0L,SEEK_SET);
    fread(players,sizeof(players),1,ff1);
    fread(proximaFolha,sizeof(proximaFolha),1,ff1);

    if(*folhas == 1) {
        fi = fopen(indice,"wb");
        if((!fi)) exit(1);

        int aux = 0;

        // Cria indice.bin
        (*nos)++;
        fwrite(&nChaves,sizeof(int),1,fi);
        strcpy(id,players[t-1].id);
        fwrite(id,sizeof(id),1,fi);
        strcpy(id,"-");
        for(i = 0; i < ((2*t)-1) - 1; i++) fwrite(id,sizeof(id),1,fi); // chaves
        
        strcpy(filho,"f1.bin");
        fwrite(filho,sizeof(filho),1,fi); 
        strcpy(filho,"f2.bin");
        fwrite(filho,sizeof(filho),1,fi);
        
        strcpy(filho,"-");
        for(i = 0; i < (2*t) - 2; i++) fwrite(filho,sizeof(filho),1,fi); // filhos
    }
    else {
        fi = fopen(indice, "rb+");
        if((!fi)) exit(1);
        
        char filhos[(2*t)][ARQ_SIZE];

        // indice.bin
        fseek(fi,(no - 1) * bloco,SEEK_SET);
        fread(&nChaves,sizeof(int),1,fi);
        nChaves++;

        // chaves
        char chaves[nChaves][ID_SIZE];

        fread(chaves,sizeof(chaves),1,fi);
        strcpy(chaves[nChaves-1],players[t-1].id);
        qsort(chaves,nChaves,sizeof(char)*ID_SIZE,compara_str);
        fseek(fi,(no - 1) * bloco,SEEK_SET); 
        fwrite(&nChaves,sizeof(int),1,fi);
        fwrite(chaves,sizeof(chaves),1,fi);

        // filhos
        fseek(fi,(sizeof(char)*ID_SIZE)*(((2*t)-1)-nChaves),SEEK_CUR);
        fread(filhos,sizeof(filhos),1,fi);

        for(i = ((2*t) - 1); i > -1; i--) {
            strcpy(filhos[i],filhos[i-1]);
            if(!strcmp(filhos[i],folha)) break;
        }
        strcpy(filhos[i],novaFolha);
        
        fseek(fi,-sizeof(filhos),SEEK_CUR);
        fwrite(filhos,sizeof(filhos),1,fi);
    }

    // Folha 1
    for(i = 0; i < (t-1); i++) {
        fwrite(&players[i],sizeof(Tplayer),1,temp);
    }

    // Folha 2
    (*folhas)++;
    for(i = t-1; i < ((2*t)-1); i++) {
        fwrite(&players[i],sizeof(Tplayer),1,ff2);
    }
    fwrite(proximaFolha,sizeof(proximaFolha),1,ff2);
    
    // Folha 1
    fwrite(novaFolha,sizeof(novaFolha),1,temp);

    fclose(fi);
    fclose(ff1);
    fclose(ff2);
    fclose(temp);
    if (rename(folha,"lixo.bin") != 0) perror("Erro ao renomear");
    if (rename("temp.bin", folha) != 0) perror("Erro ao renomear");
    if (remove("lixo.bin") != 0) perror("Erro ao remover lixo.bin");
}

void dividir_no(char *indice, int t, int *folhas, int *nos, int no, int pai, int bloco) {
    printf("Dividir no %d ",no);
    FILE *fi = fopen(indice,"rb+");
    if(!fi) exit(1);

    char chaves[((2*t)-1)][ID_SIZE], filhos[(2*t)][ARQ_SIZE], chave[ID_SIZE] = "-", filho[ARQ_SIZE] = "";
    int nChaves, i, noFilho, nFilhos;

    // Comeco do bloco do no
    fseek(fi,((no - 1) * bloco) + sizeof(int),SEEK_SET);
    fread(chaves,sizeof(chaves),1,fi);
    fread(filhos,sizeof(filhos),1,fi); 

    // Chaves do bloco pai
    fseek(fi,(pai - 1) * bloco,SEEK_SET);
    fread(&nChaves,sizeof(int),1,fi);
    if(nChaves == ((2*t)-1)) {
        fseek(fi,(pai - 1) * bloco,SEEK_SET);
        nChaves = 1;
        fwrite(&nChaves,sizeof(int),1,fi);
        fwrite(chaves[t-1],sizeof(chave),1,fi);
        for(i = 0; i < ((2*t)-1) - 1; i++) fwrite(chave,sizeof(chave),1,fi);
    } 
    else {
        nChaves++;
        char chavesPai[nChaves][ID_SIZE];
        fread(chavesPai,sizeof(chavesPai),1,fi); 
        strcpy(chavesPai[nChaves-1],chaves[t-1]);
        qsort(chavesPai,nChaves,sizeof(char)*ID_SIZE,compara_str);
        
        fseek(fi,(pai - 1) * bloco,SEEK_SET);
        fwrite(&nChaves,sizeof(int),1,fi);
        fwrite(chavesPai,sizeof(chavesPai),1,fi);
        for(i = 0; i < (((2*t)-1)-nChaves); i++) fwrite(chave,sizeof(chave),1,fi);
    }
    // Filhos do bloco pai
    sprintf(filho, "%d", *nos + 1);
    if(no == 1) { 
        // Dividindo a raiz
        fwrite(filho,sizeof(filho),1,fi);
        sprintf(filho, "%d", *nos + 2);
        fwrite(filho,sizeof(filho),1,fi);
        strcpy(filho,"-");
        for(i = 0; i < (2*t) - 2; i++) fwrite(filho,sizeof(filho),1,fi);
        
        fseek(fi,0L,SEEK_END);

        // Soma no da esquerda
        (*nos)++;
    }
    else {
        char filhosPai[(2*t)][ARQ_SIZE], filhoAux[ARQ_SIZE] = "";
        fread(filhosPai,sizeof(filhosPai),1,fi);

        for(i = 0; i < (2*t); i++) {
            strcpy(filhoAux,filhosPai[i]);
            noFilho = atoi(filhoAux);
            if(noFilho != no) break;
        }
        for(i = (2*t) - 1; i > -1; i--) {
            strcpy(filhosPai[i],filhosPai[i-1]);
            strcpy(filhoAux,filhosPai[i]);
            noFilho = atoi(filhoAux);
            if(noFilho == no) break;
        }
        strcpy(filhosPai[i],filho);
        fseek(fi,-sizeof(filhosPai),SEEK_CUR);
        fwrite(filhosPai,sizeof(filhosPai),1,fi);
        
        // Indo para no da esquerda
        fseek(fi,((no - 1) * bloco),SEEK_SET);
        strcpy(filho,"-");
    }
    // No da esquerda
    nChaves = t - 1;
    fwrite(&nChaves,sizeof(int),1,fi);
    for(i = 0; i < nChaves; i++) {
        fwrite(chaves[i],sizeof(char)*ID_SIZE,1,fi);
    }
    for(i = 0; i < (((2*t)-1) - nChaves); i++) fwrite(&chave,sizeof(chave),1,fi);
    for(i = 0; i < t; i++) {
        fwrite(filhos[i],sizeof(filho),1,fi);
    }
    for(i = 0; i < (2*t) - t; i++) fwrite(filho,sizeof(filho),1,fi);

    fseek(fi,0L,SEEK_END);

    // No da direita
    // Soma
    (*nos)++;
    fwrite(&nChaves,sizeof(int),1,fi);

    for(i = t; i < ((2*t)-1); i++) {
        fwrite(chaves[i],sizeof(char)*ID_SIZE,1,fi);
    }
    for(i = 0; i < t; i++) fwrite(&chave,sizeof(chave),1,fi);
    for(i = t; i < (2*t); i++) {
        fwrite(filhos[i],sizeof(filho),1,fi);
    }
    for(i = 0; i < t; i++) fwrite(filho,sizeof(filho),1,fi);
    fclose(fi);
}

void insere_rec(char *indice, int t, Tplayer *player, int *folhas, int *nos, int noInicial, int bloco) {
    printf("Inserindo ");
    FILE *fi = fopen(indice, "rb+");
    if(!fi) exit(1);

    int nChaves, i, no = 1, pai = 1;
    char chaves[((2*t)-1)][ID_SIZE], id[ID_SIZE], filho[ARQ_SIZE] = "", proximaFolha[ARQ_SIZE] = "";

    fseek(fi,(noInicial - 1) * bloco, SEEK_SET);
    fread(&nChaves,sizeof(int),1,fi);
    fread(chaves,sizeof(chaves),1,fi);

    // Acho para qual filho ir
    for(i = 0; i < nChaves; i++) {
        if(strcmp(player->id,chaves[i]) < 0) break;
    }

    fseek(fi,i * sizeof(filho),SEEK_CUR);
    fread(filho,sizeof(filho),1,fi);
    
    while(strncmp(filho,"f",1)) {
        printf("%s ",filho);
        pai = no;
        no = atoi(filho);
        // Comeco do no filho
        fseek(fi,bloco * (no - 1),SEEK_SET);
        fread(&nChaves,sizeof(int),1,fi);
        if(nChaves == ((2*t)-1)) {
            dividir_no(indice, t, folhas, nos, no, pai, bloco);
            fseek(fi,bloco * (pai - 1),SEEK_SET);
            fread(&nChaves,sizeof(int),1,fi);
            fread(chaves,sizeof(chaves),1,fi);

            // Acho para qual filho ir
            for(i = 0; i < nChaves; i++) { 
                if(strcmp(player->id,chaves[i]) < 0) break;
            }
            fseek(fi,i * sizeof(filho),SEEK_CUR);
            fread(filho,sizeof(filho),1,fi);
            continue;
        }
        fread(chaves,sizeof(chaves),1,fi);
        for(i = 0; i < nChaves; i++) {
            if((strcmp(player->id,chaves[i]) < 0)) break;
        }
        fseek(fi,i * sizeof(filho),SEEK_CUR);
        fread(filho,sizeof(filho),1,fi);
    }
    fclose(fi);

    if(!strncmp(filho,"f",1)) {
        printf("%s ",filho);
        FILE *ff = fopen(filho,"rb+");
        if(!ff) exit(1);

        int nElems;

        fseek(ff,0L,SEEK_END);
        i = ftell(ff);
        nElems = i/sizeof(Tplayer);

        if(nElems < ((2*t)-1)) {

            Tplayer players[nElems+1];

            fseek(ff,0L,SEEK_SET);
            fread(players,sizeof(Tplayer)*nElems,1,ff);
            fread(proximaFolha,sizeof(proximaFolha),1,ff);

            players[nElems] = *player;
            qsort(players,nElems+1,sizeof(Tplayer),compara_por_id);

            fseek(ff,0L,SEEK_SET);
            fwrite(players,sizeof(Tplayer)*(nElems+1),1,ff);
            fwrite(proximaFolha,sizeof(proximaFolha),1,ff);

            fclose(ff);
        }
        else {
            fclose(ff);
            dividir_folha(indice, t, folhas, nos, filho, no, bloco);
            insere_rec(indice, t, player, folhas, nos, no, bloco);
        }
    }
}

void insere(char *indice, int t, Tplayer *player, int *folhas, int *nos, int bloco) {
    if(*folhas == 0) {
        (*folhas)++;
        // Cria primeira folha
        FILE *f1 = fopen("f1.bin", "wb");
        char prox[ARQ_SIZE] = "-";
        fwrite(player,sizeof(Tplayer),1,f1);
        fwrite(prox,sizeof(prox),1,f1);
        fclose(f1);
    }
    else {
        FILE *fi = fopen(indice, "rb+");
        if(!fi) exit(1);
        int nChaves;
        fread(&nChaves,sizeof(int),1,fi);
        fclose(fi);
        if(nChaves == 0) { 
            // Só tem a primeira folha
            FILE *f1 = fopen("f1.bin", "rb+");
            if(!f1) exit(1);

            int i, nElems;
            char proximaFolha[ARQ_SIZE];

            fseek(f1,0L,SEEK_END);
            i = ftell(f1) - (sizeof(char)*ARQ_SIZE);
            nElems = i/sizeof(Tplayer);

            if(nElems < ((2*t)-1)) {

                Tplayer players[nElems+1];

                fseek(f1,0L,SEEK_SET);
                fread(players,sizeof(Tplayer)*nElems,1,f1);
                fread(proximaFolha,sizeof(proximaFolha),1,f1);

                players[nElems] = *player;
                qsort(players,nElems+1,sizeof(Tplayer),compara_por_id);

                fseek(f1,0L,SEEK_SET);
                fwrite(players,sizeof(Tplayer)*(nElems+1),1,f1);
                fwrite(proximaFolha,sizeof(proximaFolha),1,f1);

                fclose(f1);
            }
            else {
                fclose(f1);
                dividir_folha(indice, t, folhas, nos, "f1.bin", 0, bloco);
                insere_rec(indice, t, player, folhas, nos, 1, bloco);
            }
        }
        else if(nChaves == ((2*t)-1)) {
            dividir_no(indice, t, folhas, nos, 1, 1, bloco);
            insere_rec(indice, t, player, folhas, nos, 1, bloco);
        }
        else insere_rec(indice, t, player, folhas, nos, 1, bloco);
    }
}

void maiuscula(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

void geradorIdJogador(char *id,char* nome, int anoNascimento,char *nacionalidade,int numeroJogador) {
    char primeiraLetraNome = toupper((unsigned char)nome[0]);
    // Gera um ID fictício baseado no número do jogador
    if(numeroJogador < 10) {
        sprintf(id, "%c%d%s00%d",primeiraLetraNome,anoNascimento, nacionalidade, numeroJogador);
    } else if(numeroJogador < 100) {
        sprintf(id, "%c%d%s0%d",primeiraLetraNome,anoNascimento, nacionalidade, numeroJogador);
    } else {
        sprintf(id, "%c%d%s%d",primeiraLetraNome,anoNascimento, nacionalidade, numeroJogador);
    }
}

void preencherCampeoes(Tplayer *jogador, char *nomeArquivoCampeoes) {
    FILE *fp = fopen(nomeArquivoCampeoes, "r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de campeões.\n");
        exit(1);
    }

    int pontosPorTorneio[15] = {
        2000, 2000, 2000, 2000, // Grand Slams
        0,                     // ATP Finals
        0,                     // Olimpíadas
        1000,1000,1000,1000,1000,1000,1000,1000,1000 // Masters 1000
    };

    // Extrai sobrenome do jogador
    char sobrenomeJogador[50];
    char *ultimoEspaco = strrchr(jogador->nome, ' ');
    strcpy(sobrenomeJogador, ultimoEspaco ? ultimoEspaco + 1 : jogador->nome);

    // Caso especial: Daniil Medvedev → D. Medvedev
    char nomeComparacao[50];
    if (strcmp(sobrenomeJogador, "Medvedev") == 0) {
        char primeiraLetra = toupper((unsigned char)jogador->nome[0]);
        sprintf(nomeComparacao, "%c. Medvedev", primeiraLetra); // Ex: D. Medvedev
    } else {
        strcpy(nomeComparacao, sobrenomeJogador);
    }

    char linha[1024];
    fgets(linha, sizeof(linha), fp); // pula cabeçalho

    while (fgets(linha, sizeof(linha), fp)) {
        char *token = strtok(linha, "\\");
        int coluna = -1; // começa com -1 para ignorar o ano

        while (token && coluna < 14) { // agora <14 porque vamos de 0 a 13 (total 14 colunas de torneios)
            if (coluna >= 0) {
                // Remove o conteúdo entre parênteses
                char *abreParenteses = strchr(token, '(');
                if (abreParenteses)
                    *abreParenteses = '\0';

                // Remove espaços em branco do final
                int len = strlen(token);
                while (len > 0 && isspace(token[len - 1])) token[--len] = '\0';

                if (strcmp(token, "-") != 0) {
                    if (strstr(nomeComparacao, token) != NULL || strstr(token, nomeComparacao) != NULL) {
                        jogador->quantidadeTorneios[coluna]++;
                        jogador->pontosTotais += pontosPorTorneio[coluna];
                    }
                }
            }

            token = strtok(NULL, "\\");
            coluna++;
        }
    }

    fclose(fp);
}

void LerEscreverJogadoresArvore(char *nomeIndex,char *nomeArquivoJogadores,char *nomeArquivoCampeoes,int *numeroDeJogadores, int *folhas, int *nos, int t, int bloco) {
    FILE *fp = fopen(nomeArquivoJogadores, "r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de jogadores.\n");
        exit(1);
    }
    
    char linha[256];
    while (fgets(linha, sizeof(linha), fp)) {
        Tplayer j;
        char *token = strtok(linha, "\\");
        if (token != NULL) strcpy(j.nome, token);

        if(strcmp(j.nome, "Name") == 0) continue; // Pula o cabeçalho

        token = strtok(NULL, "\\");
        j.anoNascimento = token ? atoi(token) : -1;

        token = strtok(NULL, "\\");
        j.anoMorte = (token && strcmp(token, "-") != 0) ? atoi(token) : -1;

        token = strtok(NULL, "\\");
        // if (token) strncpy(j.nacionalidade, token,3);
        if (token) {
            char nacionalidade[20];
            strcpy(nacionalidade, token);
            if(!strcmp(nacionalidade,"Australia")) strcpy(j.nacionalidade,"AUT");
            else if(!strcmp(nacionalidade,"Belarus")) strcpy(j.nacionalidade,"BLR");
            else if(!strcmp(nacionalidade,"Great Britain")) strcpy(j.nacionalidade,"GBR");
            else if(!strcmp(nacionalidade,"Greece")) strcpy(j.nacionalidade,"GRC");
            else {
                strncpy(j.nacionalidade, token,3);
                maiuscula(j.nacionalidade); // Converte nacionalidade para maiúsculas
            }
        }

        token = strtok(NULL, "\\");
        j.ranking = (token && strcmp(token, "-") != 0) ? atoi(token) : -1;

        token = strtok(NULL, "\\");
        if (j.ranking != -1 && token) {
            // verificar se contém semanas entre parênteses
            char *abreParenteses = strchr(token, '(');
            if (abreParenteses != NULL) {
                *abreParenteses = '\0'; // corta o string no '('
                j.anoRanking = atoi(token);
                j.semanasNoTopo = atoi(abreParenteses + 1); // pula o '('
            } else {
                j.anoRanking = atoi(token);
                j.semanasNoTopo = 0;
            }
        } else {
            j.anoRanking = -1;
            j.semanasNoTopo = 0;
        }
        j.nacionalidade[3] = '\0'; // Garante que a string de nacionalidade tenha o tamanho correto
        geradorIdJogador(j.id,j.nome, j.anoNascimento, j.nacionalidade, *numeroDeJogadores);
        // DEBUG
        printf("ID gerado: %s, Nome: %s, Nasc: %d, Morte: %d, Pais:%s, Rank:%d, Ano Rank: %d, Semanas:%d\n", j.id,j.nome,j.anoNascimento,j.anoMorte,j.nacionalidade,j.ranking,j.anoRanking,j.semanasNoTopo); // DEBUG
        // Inicializa os campos de pontos e torneios
        j.pontosTotais = 0;
        for (int i = 0; i < 15; i++) {
            j.quantidadeTorneios[i] = 0; // Inicializa a quantidade de
        }
        preencherCampeoes(&j, nomeArquivoCampeoes); // Preenche os campeões do jogador
        printf("quantidade de Pontos: %d, quantidade de torneios: ", j.pontosTotais);
        printf("[");
        for (int i = 0; i < 15; i++) {
            printf("%d", j.quantidadeTorneios[i]);
            if (i < 14) printf(", ");
        }
        printf("]\n");
        
        // Insere o jogador na árvore
        // InsereJogador(nomeIndex, &j,0,0); // Insere o jogador na árvore B
        insere(nomeIndex,t,&j,folhas,nos,bloco);

        // Insere o jogador na tabela hash de nomes
        // THnome_insere("hash_nome.bin", "dados_nome.bin",j.nome,j.id);
        *numeroDeJogadores += 1; // Incrementa o número de jogadores
    }

    fclose(fp);
}

int main(){
    
    char id[ID_SIZE], indice[40] = "indice.bin";
    int aux = 0, folhas = 0, nos = 0, t = 10, bloco = 354, numeroDeJogadores = 0;

    // Inicializa indice.bin
    FILE *fi = fopen(indice,"wb");
    if(!fi) return 0;
    fwrite(&aux,sizeof(int),1,fi);
    fclose(fi);

    printf("t = ");
    scanf("%d",&t);

    bloco = sizeof(int) + ((2*t)-1) * sizeof(char) * ID_SIZE + (2*t) * sizeof(char) * ARQ_SIZE;

    LerEscreverJogadoresArvore("indice.bin", "tennis_players.txt", "champions.txt",&numeroDeJogadores,&folhas,&nos,t,bloco);

    return 0;
}