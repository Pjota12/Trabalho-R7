#include "Players.h"

// typedef struct player {
//     char id[ID_SIZE];          // ID usado na árvore "1231986BRA"  
//     char nome[50];
//     int anoNascimento;
//     int anoMorte;
//     char nacionalidade[4];     // Pode ser codificado ESP, BRA, ARG, etc.
//     int ranking;             // melhor ranking do jogador
//     int anoRanking;         // Ano do melhor ranking
//     int semanasNoTopo;  // Semanas no topo do ranking
//     int pontosTotais;       //pontos totais
//     int quantidadeTorneios[15]; // grand slams = [0] - [3] | [4] ATP Finals | [5] Olimpiadas | [6-14] ATP 1000
// }Tplayer;
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


void LerEscreverJogadoresArvore(char *nomeIndex,char *nomeArquivoJogadores,char *nomeArquivoCampeoes,int *numeroDeJogadores,int T) {
    FILE *fp = fopen(nomeArquivoJogadores, "r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de jogadores.\n");
        exit(1);
    }
               // melhor ranking do jogador
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
<<<<<<< HEAD
=======
        maiuscula(j.nacionalidade); // Converte nacionalidade para maiúsculas
>>>>>>> 5d838b4f4bd3ec384daa6a91445af26a01c41952
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
        InsereJogador(nomeIndex, &j,0,0,T); // Insere o jogador na árvore B

        // Insere o jogador na tabela hash de nomes
        THnome_insere("hash_nome.bin", "dados_nome.bin",j.nome,j.id);
        *numeroDeJogadores += 1; // Incrementa o número de jogadores
    }

    fclose(fp);
}
    
