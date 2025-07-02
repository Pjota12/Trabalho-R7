#include "TABMaux.h"

// //PEGA OFSET LOGICO DO ARQUIVO DE ÍNDICE
// int getOfsetLogicoRaiz(char*nomeArquivo){
//     FILE *fp = fopen(nomeArquivo, "rb");
//     if (fp == NULL) {
//         printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
//         exit(1);
//     }
//     TmainIndex index;
//     fread(&index, sizeof(TmainIndex), 1, fp); // Lê o índice principal do arquivo
//     fclose(fp);
//     return index.raizIndex; // Retorna o índice da raiz
// }

// //ATUALIZA O OFSET LOGICO DO ARQUIVO DE ÍNDICE
// int updateOfsetLogicoRaiz(char*nomeArquivo,int ofset){
//     FILE *fp = fopen(nomeArquivo, "rb+");
//     if (fp == NULL) {
//         printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
//         exit(1);
//     }
//     TmainIndex index;
//     fread(&index, sizeof(TmainIndex), 1, fp); // Lê o índice principal do arquivo
//     index.raizIndex = ofset; // Atualiza o índice da raiz
//     fseek(fp, 0, SEEK_SET); // Move o ponteiro do arquivo para o início
//     fwrite(&index, sizeof(TmainIndex), 1, fp); // Escreve o índice atualizado de volta no arquivo
//     fclose(fp);
//     return index.raizIndex; // Retorna o novo índice da raiz
// }


void printChaves(Tnode *node) {
    printf("Chaves: ");
    for (int i = 0; i < node->numKeys; i++) {
        printf("%s ", node->keys[i]);
    }
    printf("\n");
}

void printFilhos(Tnode *node) {
    printf("Filhos: ");
    for (int i = 0; i < node->numKeys + 1; i++) {
        printf("%s ", node->filhos[i]);
    }
    printf("\n");
}

void printIndexTotal(char *nomeArquivo, int T) {
    FILE *fp = fopen(nomeArquivo, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }
    
    fseek(fp, 0, SEEK_END); // Move o ponteiro do arquivo para o final
    long limite = ftell(fp);
    fseek(fp, sizeof(int), SEEK_SET); // Pula o cabeçalho (raiz lógica)
    printf("PRINTANDO INDEX TOTAL:\n");

    Tnode *node;
    while (ftell(fp) < limite && (node = lerNo(fp, T)) != NULL) {
        printf("Nó:\n");
        printf("isOcuped: %d\n", node->isOcuped);
        printf("numKeys: %d\n", node->numKeys);
        printChaves(node);
        printFilhos(node);
        printf("\n");
    }

    printf("Fim do arquivo.\n\n\n");
    fclose(fp);
}

void printFolhaImpressa(char *nomeArquivoFolha, int nivel) {
    FILE *fp = fopen(nomeArquivoFolha, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivoFolha);
        exit(1);
    }
    for (int i = 0; i < nivel; i++) {
            printf("\t"); // Indenta de acordo com o nível
    }
    printf("Folha: %s\n", nomeArquivoFolha);
    Tplayer jogador;
    long limiteDeLeitura = limiteFolha(nomeArquivoFolha); // Obtém o limite da folha
    while (ftell(fp) < limiteDeLeitura && fread(&jogador, sizeof(Tplayer), 1, fp) == 1) {
        for (int i = 0; i < nivel; i++) {
            printf("\t"); // Indenta de acordo com o nível
        }
        printf("Jogador ID: %s, Nome: %s\n", jogador.id, jogador.nome);
    }
    fclose(fp);
}

//FUNÇÂO PARA CALCULAR O OFSET PARA INSERÇÃO DE NOVOS NÓS



int calculaOfsetLogico(char *nomeArquivo,int T){
    FILE *fp = fopen(nomeArquivo, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END); // Move o ponteiro do arquivo para o final
    long limite = ftell(fp); // Obtém o tamanho do arquivo


    Tnode *node; // Cria um novo nó para armazenar os dados lidos do arquivo
    fseek(fp, sizeof(int), SEEK_SET); // Pula o cabeçalho (raiz lógica)
    int offset = 0;

    while (ftell(fp) < limite && (node = lerNo(fp,T)) != NULL){ // Lê os nós do arquivo de índice
        if (!node->isOcuped) {
            fclose(fp);
            return offset; // Retorna imediatamente o primeiro offset livre
        }
        offset++;
    }

    fclose(fp);
    liberaNo(node,T); // Libera a memória alocada para o nó
    return offset; // Se não achou nenhum livre, retorna o próximo disponível no final
}


// Função para obter o número de folhas do arquivo de índice
int getNumeroDeFolha(char*nomeArquivo){
    FILE *fp = fopen(nomeArquivo, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }
    int index;
    fread(&index, sizeof(int), 1, fp); // Lê o índice principal do arquivo
    fclose(fp);
    return index; // Retorna o índice da raiz
}

// Função para atualizar o número de folhas no arquivo de índice
void updateNumeroDeFolha(char*nomeArquivo,int numFolha){
    FILE *fp = fopen(nomeArquivo, "rb+");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }
    int index;
    fread(&index, sizeof(int), 1, fp); // Lê o índice principal do arquivo
    index = numFolha; // Atualiza o número de folhas
    fseek(fp, 0, SEEK_SET); // Move o ponteiro do arquivo para o início
    fwrite(&index, sizeof(int), 1, fp); // Escreve o índice atualizado de volta no arquivo
    fclose(fp);
}

// Função para obter o nome do próximo nó a partir do arquivo
//serve para ler o nome do proximo nó a partir do arquivo de índice
void getProximaFolha(char *nomeArquivoFolha ,char *nomeNo){
    FILE *fp = fopen(nomeArquivoFolha,"rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }
    fseek(fp,-ARQ_SIZE,SEEK_END); // Move o ponteiro do arquivo para o final menos o tamanho de ARQ_SIZE
    fread(nomeNo,sizeof(char), ARQ_SIZE, fp); // Lê o nome do nó
    nomeNo[ARQ_SIZE - 1] = '\0'; // garante terminação de string
    fclose(fp);
}

void WriteLeafName(char *nomeNovafolha, int i) {
    // Supondo que NOME_FOLHA seja algo como "folha"
    if (i < 10)
        sprintf(nomeNovafolha, "%s0%d.bin", NOME_FOLHA, i);
    else
        sprintf(nomeNovafolha, "%s%d.bin", NOME_FOLHA, i);
}

int contaElementosFolha(char *nomeArquivoFolha){
    FILE *fp = fopen(nomeArquivoFolha, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivoFolha);
        exit(1);
    }
    fseek(fp, 0, SEEK_END); // Move o ponteiro do arquivo para o final
    // printf("----------------------------\n");
    // printf("CONTA ELEMENTOS FOLHA\n");
    // printf("Tamanho do arquivo: %ld bytes\n", ftell(fp)); // Imprime o tamanho do arquivo
    // printf("Tamanho do ARQ_SIZE: %d bytes\n", ARQ_SIZE); // Imprime o tamanho de ARQ_SIZE
    // printf("Tamanho do Tplayer: %zu bytes\n", sizeof(Tplayer)); // Imprime o tamanho de Tplayer
    int tamanho = ftell(fp);
    // printf("quantidade de jogadores: %ld\n", (tamanho - ARQ_SIZE) / sizeof(Tplayer)); // Imprime a quantidade de jogadores
    // printf("----------------------------\n");
    fclose(fp);
    if (tamanho <= 0) return 0; // Nenhum jogador
    return (tamanho - ARQ_SIZE) / sizeof(Tplayer);

}

long limiteFolha(char *nomeArquivoFolha){
    FILE *fp = fopen(nomeArquivoFolha, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivoFolha);
        exit(1);
    }
    fseek(fp, 0, SEEK_END); // Move o ponteiro do arquivo para o final
    long tamanho = ftell(fp)-ARQ_SIZE; // Obtém o tamanho do arquivo
    fclose(fp);
    return tamanho; // Retorna o tamanho do arquivo
}

void inserirJogadorNaFolhaOrdenado(char *NomeArqFolha,Tplayer *novoJogador){
    FILE *ff = fopen(NomeArqFolha, "rb+");
    if (ff == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", NomeArqFolha);
        exit(1);
    }
    Tplayer *player = malloc(sizeof(Tplayer));
    if (player == NULL) {
        printf("Erro ao alocar memória para o jogador.\n");
        fclose(ff);
        exit(1);
    }

    char nomeProximaFolha[ARQ_SIZE];
    getProximaFolha(NomeArqFolha, nomeProximaFolha);

    long limiteLeitura = limiteFolha(NomeArqFolha); // Obtém o tamanho do arquivo menos o espaço para o nome da próxima folha
    int quantidadeElementos = contaElementosFolha(NomeArqFolha); // Armazena a quantidade de elementos lidos
    int posicaoInsercao = -1;

    if(quantidadeElementos==0) { // Se o nome da próxima folha for vazio, cria uma nova folha
        fwrite(novoJogador, sizeof(Tplayer), 1, ff); // Escreve o novo jogador no arquivo
        fwrite(nomeProximaFolha, sizeof(char), ARQ_SIZE, ff); // Escreve o nome vazio da próxima folha
        fclose(ff); // Fecha o arquivo
        free(player); // Libera a memória alocada para o jogador
        return; // Retorna após inserir o jogador
    }

    // Percorre o arquivo até encontrar a posição correta para inserir o novo jogador
    int i = 0;
    while (ftell(ff) < limiteLeitura && fread(player, sizeof(Tplayer), 1, ff) == 1){
        if(strcmp(player->id, novoJogador->id) == 0) { // Verifica se o ID do novo jogador já existe
            printf("Jogador com ID %s já existe na folha.\n", novoJogador->id);
            free(player); // Libera a memória alocada para o jogador
            fclose(ff); // Fecha o arquivo
            return; // Retorna sem inserir o jogador
        }
        if (strcmp(player->id, novoJogador->id) > 0) { // Verifica se o ID do novo jogador é menor que o ID do jogador atual
            posicaoInsercao = i; // Armazena a posição de inserção
            break; // Sai do loop se encontrar a posição correta
        }
        i++;
    }

    if(posicaoInsercao == -1) { // Se não encontrou posição de inserção, insere no final
        fseek(ff, -ARQ_SIZE, SEEK_END); // Vai para onde começa o nome da próxima folha
        fwrite(novoJogador, sizeof(Tplayer), 1, ff); // Insere o jogador antes do nome da próxima folha
        // Agora escreve novamente o nome da próxima folha depois do novo jogador
        fwrite(nomeProximaFolha, sizeof(char), ARQ_SIZE, ff);
        free(player); // Libera a memória alocada para o jogador
        fclose(ff);
    }else{
        FILE *fTemp = fopen("temp.bin", "wb");
        if (fTemp == NULL) {
            printf("Erro ao abrir o arquivo temporário.\n");
            fclose(ff);
            free(player);
            exit(1);
        }
        fseek(ff, 0, SEEK_SET); // Move o ponteiro do arquivo para o início
        // Escreve os jogadores até a posição de inserção no arquivo temporário
        for(int j=0;j < quantidadeElementos+1; j++){
            if (j == posicaoInsercao) { // Se for a posição de inserção, escreve o novo jogador
                fwrite(novoJogador, sizeof(Tplayer), 1, fTemp); // Insere o novo jogador
            }else{
                fread(player, sizeof(Tplayer), 1, ff); // Lê o jogador atual
                fwrite(player, sizeof(Tplayer), 1, fTemp); // Escreve o jogador atual no arquivo temporário
            }
        }

        // Escreve o nome da próxima folha no final do arquivo temporário
        fseek(fTemp, 0, SEEK_END);
        fwrite(nomeProximaFolha, sizeof(char), ARQ_SIZE, fTemp); // Escreve o nome da próxima folha no final do arquivo

        fclose(ff); // Fecha o arquivo original
        fclose(fTemp); // Fecha o arquivo temporário
        if (remove(NomeArqFolha)) { // Remove o arquivo original
            printf("Erro ao remover o arquivo antigo.\n");
            free(player); // Libera a memória alocada para o jogador
            exit(1);
        }

        if (rename("temp.bin", NomeArqFolha)) { // Renomeia o arquivo temporário para o nome original
            printf("Erro ao renomear o arquivo temporário.\n");
            free(player); // Libera a memória alocada para o jogador
            exit(1);
        }
        printf("Jogador inserido na folha:%s com sucesso.\n",NomeArqFolha);
    }
}

Tplayer *buscaBinariaJogadorNaFolha(char *nomeArquivoFolha, char *idJogador) {
    printf("Buscando jogador com ID: %s na folha: %s\n", idJogador, nomeArquivoFolha);
    FILE *ff = fopen(nomeArquivoFolha, "rb");
    if (ff == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivoFolha);
        exit(1);
    }
    Tplayer *player = malloc(sizeof(Tplayer));
    int inicio = 0;
    int fim = contaElementosFolha(nomeArquivoFolha) - 1; //
    printf("Quantidade de jogadores na folha: %d fim:%d inicio:%d\n", fim + 1,fim,inicio); // Imprime a quantidade de jogadores na folha
    while(inicio <= fim){
        int meio = (inicio + fim) / 2; // Calcula o índice do meio
        fseek(ff, meio * sizeof(Tplayer), SEEK_SET); // Move o ponteiro do arquivo para o meio
        fread(player, sizeof(Tplayer), 1, ff); // Lê o jogador no meio

        if (strcmp(player->id, idJogador) == 0) { // Se o ID do jogador for igual ao ID procurado
            fclose(ff);
            return player; // Retorna o jogador encontrado
        } else if (strcmp(player->id, idJogador) < 0) { // Se o ID do jogador for menor que o ID procurado
            inicio = meio + 1; // Move a busca para a metade direita
        } else {
            fim = meio - 1; // Move a busca para a metade esquerda
        }
    }
    fclose(ff);
    free(player); // Libera a memória alocada para o jogador
    return NULL; // Retorna NULL se o jogador não for encontrado
}
