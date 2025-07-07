#include "TABM.h"
#include "TABMaux.h"


//MODELO INDEX
//-> int com numero de folhas               |
//->node                                    | 
    //-> int isOcuped                       |
    //-> int isValuesLeaf                   |
    //-> int numKeys                        |   
    //-> char keys[MAX_KEYS][ID_SIZE]       |
    //-> char filhos[MAX_CHILDREN][ARQ_SIZE]|
//-------------------------------------------

// MODELO FOLHA
//-> Tplayer jogador[MAX_KEYS]             |
    //-> char proximaFolha[ARQ_SIZE]       |

//FUNÇÕES AUXILIARES PARA TNODE
Tnode* criaNo(int T) {
    Tnode *no = malloc(sizeof(Tnode));
    no->isOcuped = 1;
    no->numKeys = 0;

    no->keys = malloc(sizeof(char*) * (2 * T - 1));
    for (int i = 0; i < (2 * T - 1); i++) {
        no->keys[i] = calloc(ID_SIZE, sizeof(char));
    }

    no->filhos = malloc(sizeof(char*) * (2 * T));
    for (int i = 0; i < (2 * T); i++) {
        no->filhos[i] = calloc(ARQ_SIZE, sizeof(char));
    }

    return no;
}

void liberaNo(Tnode *no, int T) {
    if (!no) return;
    for (int i = 0; i < (2 * T - 1); i++) free(no->keys[i]);
    for (int i = 0; i < (2 * T); i++) free(no->filhos[i]);
    free(no->keys);
    free(no->filhos);
    free(no);
}

void salvarNo(FILE *fi, Tnode *node, int T) {
    // Salva campos primitivos
    fwrite(&node->isOcuped, sizeof(int), 1, fi);
    fwrite(&node->numKeys, sizeof(int), 1, fi);

    // Salva as chaves
    for (int i = 0; i < 2 * T - 1; i++) {
        fwrite(node->keys[i], sizeof(char), ID_SIZE, fi);
    }

    // Salva os filhos
    for (int i = 0; i < 2 * T; i++) {
        fwrite(node->filhos[i], sizeof(char), ARQ_SIZE, fi);
    }
}

Tnode* lerNo(FILE *fi, int T) {
    Tnode *node = criaNo(T);
    if (fread(&node->isOcuped, sizeof(int), 1, fi) != 1 ||
        fread(&node->numKeys, sizeof(int), 1, fi) != 1) {
        printf("Erro ao ler os campos primitivos do nó.\n");
        liberaNo(node, T); // Libera a memória alocada para o nó
        return NULL;
    }

    for (int i = 0; i < 2 * T - 1; i++) {
        if (fread(node->keys[i], sizeof(char), ID_SIZE, fi) != ID_SIZE) {
            printf("Erro ao ler as chaves do nó.\n");
            liberaNo(node, T); // Libera a memória alocada para o nó
            return NULL;
        }
    }

    for (int i = 0; i < 2 * T; i++) {
        if (fread(node->filhos[i], sizeof(char), ARQ_SIZE, fi) != ARQ_SIZE) {
            printf("Erro ao ler os filhos do nó.\n");
            liberaNo(node, T); // Libera a memória alocada para o nó
            return NULL;
        }
    }

    return node;
}

int tamanhoNo(int T) {
    // Calcula o tamanho do nó em bytes
    return sizeof(int) * 2 + (2 * T - 1) * ID_SIZE + (2 * T) * ARQ_SIZE;
}

//FUNÇÕES PARA CRIAR O ARQUIVO DE ÍNDICE E INICIALIZAR A ÁRVORE B+
//----------------------------------------------------------------------------------------------------------------------------------
// Cria o arquivo de índice e o primeiro nó da árvore B+ com o numero de chaves = 0 e um main Index com raizIndex = 0 e numLeaf = 0
void InicializarIndex(int T){
    FILE *fi = fopen("index.bin", "wb"); // Abre o arquivo de índice para escrita
    if (fi == NULL) {
        printf("Erro ao criar o arquivo de índice.\n");
        exit(1);
    }
    printf("Arquivo de índice criado com sucesso.\n");

    //cria o nó raiz da árvore B+
    Tnode *node = criaNo(T);
    if(!node){
        printf("Erro ao alocar memória para o nó.\n");
        fclose(fi);
        exit(1);
    }

    // Escreve o índice principal no arquivo
    int numLeaf = 0;
    fwrite(&numLeaf, sizeof(int), 1, fi);

    //Inicializando folha 0;
    FILE *ffolha = fopen("leaf00.bin", "wb"); // Abre o arquivo do nó 0 para escrita
    if (ffolha == NULL) {
        printf("Erro ao criar o arquivo do nó.\n");
        fclose(fi);
        liberaNo(node, T); // Libera a memória alocada para o nó
        exit(1);
    }
    printf("Arquivo de leaf00.bin criado com sucesso.\n");
    // Iniciando string para guarda a proxima folha:
    char nextNode[ARQ_SIZE];
    sprintf(nextNode,VAZIO); // Nome do próximo nó (VAZIO)
    fwrite(nextNode, sizeof(char), ARQ_SIZE, ffolha); // Escreve o nome da próxima folha no arquivo

    //Setando os valores iniciais do nó:
    node->isOcuped = 1;
    // node->isValuesLeaf = 1;
    node->numKeys = 0; 
    

    // Inicializa o número de chaves como 0
    for (int j = 0; j < ((2*T)-1); j++) {
        strcpy(node->keys[j], ""); // Inicializa as chaves como strings vazias
    }

    // Inicializa os filhos do nó: 2*T FILHOS MAXIMO
    for (int j = 0; j < 2*T; j++) {
        if(j == 0) {
            strcpy(node->filhos[j], "leaf00.bin"); // O primeiro filho aponta para o próprio nó
        } else {
            strcpy(node->filhos[j], ""); // Outros filhos são strings vazias
        }
    }

    // Escrevendo o nó no arquivo:
    fseek(fi, sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o início do nó
    salvarNo(fi, node, T); // Salva o nó no arquivo da folha

    liberaNo(node, T); // Libera a memória alocada para o nó
    fclose(ffolha); // Fecha o arquivo do nó

    fclose(fi); // Fecha o arquivo após a criação
    printf("Índice inicializado com sucesso.\n\n");
}

//----------------------------------------------------------------------------------------------------------------------------------


//FUNÇOES PARA INSERIR UM JOGADOR NA ÁRVORE B+
//----------------------------------------------------------------------------------------------------------------------------------

// Função para dividir uma folha quando ela está cheia
void DivideFolha(char *folha,char *Novafolha,char *id,int T){
    FILE *ff = fopen(folha,"rb");
    if(!ff){
        printf("Erro ao abrir o arquivo Folha");
        exit(1);
    }

    int numeroFolhas = getNumeroDeFolha("index.bin");//PEGA O NUMERO DE FOLHAS EXISTENTE
    int metade = (2 * T - 1)/2;

    Tplayer *player = malloc(sizeof(Tplayer));
    fseek(ff,metade*sizeof(Tplayer),SEEK_SET);
    fread(player,sizeof(Tplayer),1,ff);

    strcpy(id,player->id);
    
    WriteLeafName(Novafolha,++numeroFolhas);
    updateNumeroDeFolha("index.bin",numeroFolhas);

    FILE *fn = fopen(Novafolha,"wb");
    if(!fn){
        printf("Erro ao abrir o arquivo da nova folha.\n");
        fclose(ff);
        free(player);
        exit(1);
    }

    long limiteLeitura = limiteFolha(folha); // Obtém o tamanho do arquivo menos o espaço para o nome da próxima folha
    
    //ESCREVI FOLHA NOVA COMPLETA
    fseek(ff, metade * sizeof(Tplayer), SEEK_SET);
    while(ftell(ff) < limiteLeitura && fread(player,sizeof(Tplayer),1,ff) == 1) fwrite(player,sizeof(Tplayer),1,fn);   
    fseek(fn,0,SEEK_END);
    char ProximaFolha[ARQ_SIZE];
    getProximaFolha(folha,ProximaFolha);
    fwrite(ProximaFolha,sizeof(char),ARQ_SIZE,fn);
    fclose(fn);
    
    fseek(ff,0,SEEK_SET);
    FILE *fTemp = fopen("temp.bin","wb");
    if(!fTemp){
        printf("Erro ao abrir o arquivo temporário.\n");
        fclose(ff);
        exit(1);
    }

    //ESCREVE A FOLHA ANTIGA ATÉ O JOGADOR QUE FOI DIVIDIDO
    fseek(ff, 0, SEEK_SET);
    for (int i = 0; i < metade; i++) {
        fread(player, sizeof(Tplayer), 1, ff);
        fwrite(player, sizeof(Tplayer), 1, fTemp);
    }
    fseek(fTemp, 0, SEEK_END); // Move o ponteiro do arquivo temporário para o final
    fwrite(Novafolha,sizeof(char),ARQ_SIZE,fTemp); // Escreve o nome da nova folha no arquivo temporário
    
    fclose(ff); // Fecha o arquivo da folha antiga
    fclose(fTemp); // Fecha o arquivo temporário
    if (remove(folha)) {
        printf("Erro ao remover o arquivo antigo.\n");
        free(player);
        exit(1);
    }
    rename("temp.bin", folha);
    free(player); // Libera a memória alocada para o jogador
}

// Função para inserir um nó dividido no arquivo de índice
int insereNo(char *nomeArqIndex,Tnode *noDividido,int T){
    FILE *fi = fopen(nomeArqIndex,"rb+");
    if(!fi){
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }
    printf("ENTREI NO OFSETLOGICO");
    int ofsetLogico = calculaOfsetLogico(nomeArqIndex,T); 
    printf("ofsetLogico calculado: %d\n", ofsetLogico);
    fseek(fi, ofsetLogico * tamanhoNo(T) + sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó correspondente
    salvarNo(fi, noDividido,T); // Salva o nó no arquivo de índice

    fclose(fi);
    return ofsetLogico; // Retorna o ofset lógico do nó inserido
}

// Função para dividir um nó quando ele está cheio (NÃO ESQUECER DE ATUALIZAR O NO DIVIDIDO NO ARQUIVO DE ÍNDICE)
void DivideNoRaiz(char *nomeArqIndex,Tnode *noDividido,int n,int T){
    int metadeKeys = (2*T - 1) / 2; // Divide o número de chaves pela metade
    int metadeFilhos = (2 * T) / 2; // Divide o número de filhos pela metade


    // Cria um novo nó para armazenar as chaves e filhos divididos
    Tnode *FilhoMenor = criaNo(T); // Cria um novo nó para o filho menor
    Tnode *FilhoMaior = criaNo(T); // Cria um novo nó para o filho maior
    // Inicializa o novo nó FilhoMaior  

    if(!FilhoMaior || !FilhoMenor){
        printf("Erro ao alocar memória para o novo nó.\n");
        exit(1);
    }

    char chavePromovida[ID_SIZE];
    strcpy(chavePromovida, noDividido->keys[metadeKeys]);
    printf("Chave promovida: %s\n", chavePromovida); // Imprime a chave promovida


    // Inicializa o novo nó FilhoMenor---------------------------
    // FilhoMenor->isRaiz = 0; // Mantém o estado da raiz
    FilhoMenor->isOcuped = 1; // Marca como ocupado
    // FilhoMenor->isValuesLeaf = noDividido->isValuesLeaf; // Indica se é uma folha de valores
    FilhoMenor->numKeys = metadeKeys;

    for (int i = 0; i < metadeKeys; i++) strcpy(FilhoMenor->keys[i], noDividido->keys[i]); // Copia as chaves para o novo nó

    for (int i = 0; i < metadeFilhos; i++) strcpy(FilhoMenor->filhos[i], noDividido->filhos[i]); // Copia os filhos para o novo nó

    printf("inserindo no index");
    int ofsetFilhoMenor = insereNo(nomeArqIndex, FilhoMenor,T); // Insere o novo nó no arquivo de índice
    printf("ofset filho menor: %d\n",ofsetFilhoMenor);
    printf("numKeys do FilhoMenor: %d\n", FilhoMenor->numKeys);
    printChaves(FilhoMenor); // Imprime as chaves do nó FilhoMenor
    printFilhos(FilhoMenor); // Imprime os filhos do nó FilhoMenor

    liberaNo(FilhoMenor,T); // Libera a memória alocada para o nó FilhoMenor
    //-------------------------------------------------------------


    // Inicializa o novo nó FilhoMaior----------------------------
    // FilhoMaior->isRaiz = 0; //
    FilhoMaior->isOcuped = 1; // Marca como ocupado
    // FilhoMaior->isValuesLeaf = noDividido->isValuesLeaf; //
    FilhoMaior->numKeys = metadeKeys;

    // Copia para FilhoMaior (após a promovida)
    for (int i = 0; i < metadeKeys; i++) {
        strcpy(FilhoMaior->keys[i], noDividido->keys[i + metadeKeys + 1]); 
    }

    // Copia os filhos para o novo nó
    for (int i = 0; i < metadeFilhos; i++) strcpy(FilhoMaior->filhos[i], noDividido->filhos[i + metadeFilhos]); 

    int ofsetFilhoMaior = insereNo(nomeArqIndex, FilhoMaior,T); // Insere o novo nó no arquivo de índice
    printf("ofsetFilhoMaior: %d\n",ofsetFilhoMaior);
    printf("numKeys do FilhoMenor: %d\n", FilhoMenor->numKeys);
    printChaves(FilhoMaior); // Imprime as chaves do nó FilhoMaior
    printFilhos(FilhoMaior); // Imprime os filhos do nó FilhoMaior

    liberaNo(FilhoMaior,T); // Libera a memória alocada para o nó FilhoMaior
    //-------------------------------------------------------------


    //trasforma dividido no em pai dos dois filhos
    //-------------------------------------------------------------
    noDividido->isOcuped = 1; // Marca o nó como
    // noDividido->isValuesLeaf = 0; // Marca o nó como não
    noDividido->numKeys = 1; // Define o número de chaves como 1

    // Copia a chave do meio para o nó dividido
    strcpy(noDividido->keys[0], chavePromovida); 

    // Atualiza o nó dividido para apontar para o novo nó FilhoMenor
    strcpy(noDividido->filhos[0], ""); // Limpa o primeiro filho do nó dividido
    sprintf(noDividido->filhos[0], "%d", ofsetFilhoMenor); // Define o primeiro filho como o ofset do novo nó FilhoMenor
    // Atualiza o nó dividido para apontar para o novo nó FilhoMaior
    strcpy(noDividido->filhos[1], ""); // Limpa o segundo
    sprintf(noDividido->filhos[1], "%d", ofsetFilhoMaior); // Define o segundo filho como o ofset do novo nó FilhoMaior
    //----------------------------------------------------------------

    // Atualiza o arquivo de índice com o nó dividido
    FILE *fi = fopen(nomeArqIndex,"rb+");
    if(!fi){
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }
    // Atualiza o no raiz
    fseek(fi, n * tamanhoNo(T) + sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó n
    salvarNo(fi, noDividido,T); // Salva o nó dividido no arquivo de índice
    fclose(fi); // Fecha o arquivo de índice
    printf("ARQUIVO NO FINAL DO DIVIDE:\n");
    
}

// Função para dividir um no interno da arvore e subir a chave promovida para o pai
int DivideNoInterno(char *nomeArqIndex,char *chavePromovida,char *playerId,Tnode *noDividido,int atual, int *OfsetProximoNo,int T){

    // Pego as referencias
    int metadeKeys = (T*2-1) / 2;
    int metadeFilhos = (2*T) / 2;


    // Cria um novo nó para armazenar as chaves e filhos divididos
    Tnode *FilhoMaior = criaNo(T); // Cria um novo nó para o filho maior
    printf("no criado para FilhoMaior\n");
    if(!FilhoMaior){
        printf("Erro ao alocar memória para o novo nó.\n");
        exit(1);
    }

    //pego a chave do meio que será promovida
    strcpy(chavePromovida, noDividido->keys[metadeKeys]);
    printf("Chave promovida: %s\n", chavePromovida); // Imprime a chave promovida

    // Inicializa o novo nó FilhoMaior----------------------------
    FilhoMaior->isOcuped = 1; // Marca como ocupado
    // FilhoMaior->isValuesLeaf = noDividido->isValuesLeaf; //
    FilhoMaior->numKeys = metadeKeys;
    // Copia para FilhoMaior (após a promovida)
    for (int i = 0; i < metadeKeys; i++) {
        strcpy(FilhoMaior->keys[i], noDividido->keys[i + metadeKeys + 1]); 
    }
    // Copia os filhos para o novo nó
    for (int i = 0; i < metadeFilhos; i++) strcpy(FilhoMaior->filhos[i], noDividido->filhos[i + metadeFilhos]); 

    int OfsetFilhoMaior = insereNo(nomeArqIndex, FilhoMaior,T); // Insere o novo nó no arquivo de índice
    printf("ofsetFilhoMaior: %d\n",OfsetFilhoMaior);
    printf("numKeys do FilhoMenor: %d\n", FilhoMaior->numKeys);
    printChaves(FilhoMaior); // Imprime as chaves do nó FilhoMaior
    printFilhos(FilhoMaior); // Imprime os filhos do nó FilhoMaior
    liberaNo(FilhoMaior,T); // Libera a memória alocada para o nó FilhoMaior
    //-------------------------------------------------------------

    //Para a recursão funcionar, preciso saber qual nó filho ira a proxima pesquisa
    //portanto pesquiso a posição correta para onde o no iria para guiar a recursão
    int i=0;
    while((i < noDividido->numKeys) && (strcmp(noDividido->keys[i], playerId) <= 0)) i++; // Encontra a posição correta para inserir a chave promovida
    if(i < metadeKeys) *OfsetProximoNo = atual; // Se a posição for menor que metade, o nó atual permanece
    else *OfsetProximoNo = OfsetFilhoMaior; // Caso contrário, o próximo nó será o filho maior


    FILE *fi = fopen(nomeArqIndex,"rb+");
    if(!fi){
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }

    //tranforma dividido em filho
    noDividido->numKeys = metadeKeys; // Define o número de chaves como metade
    fseek(fi, atual * tamanhoNo(T) + sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó n
    salvarNo(fi, noDividido,T); // Salva o nó dividido no arquivo de índice

    fclose(fi); // Fecha o arquivo de índice
    return OfsetFilhoMaior; // Retorna o ofset lógico do nó maior criado}
}

//função para atualizar o nó pai com a chave promovida e o ofset do filho maior
void AtualizaPai(char *nomeArqIndex,char *chavePromovida,int filhoMaior,int indicePai,int T){
    FILE *fi = fopen(nomeArqIndex,"rb+");
    if(!fi){
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }


    fseek(fi, indicePai*tamanhoNo(T)+sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó pai
    Tnode *node = lerNo(fi,T); // Lê o nó pai do arquivo de índice

    printf("Atualizando pai no indice %d:\n", indicePai);
    printChaves(node);
    printFilhos(node);
    printf("-> Inserindo chave: %s, filhoMaior: %d\n", chavePromovida, filhoMaior);

    int i = 0;

    while((i < node->numKeys) && (strcmp(node->keys[i], chavePromovida) <= 0)) i++; // Encontra a posição correta para inserir a chave promovida

    for(int j=node->numKeys; j > i; j--){
        strcpy(node->keys[j], node->keys[j-1]); // Move as chaves para a direita
        strcpy(node->filhos[j+1], node->filhos[j]); // Move os filhos para a direita
    }

    node->numKeys++; // Incrementa o número de chaves no nó pai
    strcpy(node->keys[i], chavePromovida); // Insere a chave promovida no nó pai
    sprintf(node->filhos[i+1], "%d", filhoMaior); // Insere o ofset do filho maior no nó pai
    //o outro não e necessario pois tranformei o filho node->filhos[i] em filho menor

    printf("pai ATUALIZADO %d:\n", indicePai);
    printChaves(node);
    printFilhos(node);

    fseek(fi,indicePai*tamanhoNo(T)+sizeof(int), SEEK_SET);
    salvarNo(fi, node,T); // Salva o nó pai atualizado no arquivo de índice

    liberaNo(node,T); // Libera a memória alocada para o nó pai
    fclose(fi); // Fecha o arquivo de índice
    return;
}

// Função para inserir um novo jogador na árvore B+ em MS
void InsereJogador(char *nomeArqIndex,Tplayer *novoJogador,int atual,int Pai,int T){
    FILE *fi = fopen(nomeArqIndex,"rb+");
    if(!fi){
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }


    fseek(fi, atual*tamanhoNo(T)+sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o início
    Tnode *node = lerNo(fi,T); // Lê o nó atual do arquivo de índice

    int i = 0;

    //VERIFICA SE O NO ESTA CHEIO DE CHAVES
    if(node->numKeys == 2*T-1) { // Verifica se o nó está cheio
        if(atual == 0){
            printf("Nó raiz cheio, dividindo...\n");
            fclose(fi); // Fecha o arquivo de índice
            DivideNoRaiz(nomeArqIndex,node,atual,T); // Divide o nó raiz se for o nó atual
            liberaNo(node,T); // Libera a memória alocada para o nó
            InsereJogador(nomeArqIndex, novoJogador, 0,Pai,T); // Chama a função recursivamente para inserir o jogador novamente na raiz
            return; // Retorna após inserir o jogador na raiz
        }else{
            printf("Nó interno cheio, dividindo...\n");
            char *chavePromovida = malloc(ID_SIZE * sizeof(char));
            if(!chavePromovida){
                printf("Erro ao alocar memória para a chave promovida.\n");
                liberaNo(node,T);
                fclose(fi);
                exit(1);
            }
            fclose(fi); // Fecha o arquivo de índice
            int ofsetProximoNo = 0;
            int ofsetFilhoMaior = DivideNoInterno(nomeArqIndex,chavePromovida,novoJogador->id,node,atual,&ofsetProximoNo,T); // Divide o nó interno se não for a raiz
            printf("ofset Atual do nó: %d\n", atual);
            AtualizaPai(nomeArqIndex,chavePromovida,ofsetFilhoMaior,Pai,T); // Atualiza o nó pai com a chave promovida e o ofset do filho maior
            printf("ofset do filho maior: %d\n",ofsetFilhoMaior);
            free(chavePromovida); // Libera a memória alocada para o nó
            liberaNo(node,T); // Libera a memória alocada para o nó
            InsereJogador(nomeArqIndex, novoJogador, ofsetProximoNo,Pai,T); // Chama a função recursivamente para inserir o jogador novamente
            return; // Retorna após inserir o jogador na folha
        }
        printf("Nó dividido com sucesso.\n");
        liberaNo(node,T); // Libera a memória alocada para o nó
        fclose(fi); // Fecha o arquivo de índice
        InsereJogador(nomeArqIndex, novoJogador, atual,Pai,T); // Chama a função recursivamente para inserir o jogador novamente
        return; // Retorna após inserir o jogador na folha
    }

    while((i < node->numKeys) && (strcmp(node->keys[i], novoJogador->id) <= 0)) i++; // Percorre as chaves do nó até encontrar a posição correta para inserir o novo jogador

    printf("o indece da Chave acessada: %i\n", i);
    if(strncmp(node->filhos[i],"leaf",4) == 0) { // Se for uma folha de valores (significa que é um nó folha)
    
        printf("nome do arquivo da folha: %s\n", node->filhos[i]);
        if(buscaBinariaJogadorNaFolha(node->filhos[i], novoJogador->id)) { // Verifica se o jogador já existe na folha
            printf("Jogador com ID %s já existe na folha.\n", novoJogador->id);
            liberaNo(node,T); // Libera a memória alocada para o nó
            fclose(fi); // Fecha o arquivo de índice
            return; // Retorna sem inserir o jogador
        }
        
        int quantidadeElementos = contaElementosFolha(node->filhos[i]); // Conta o número de elementos na folha

        //CASO A FOLHA ESTEJA CHEIA -----------------------------------------------------------------------
        if(quantidadeElementos == (2 * T - 1)){
            printf("Folha %s cheia, dividindo...\n",node->filhos[i]);
            char novaFolha[ARQ_SIZE];
            char idPromovido[ID_SIZE];
            DivideFolha(node->filhos[i], novaFolha,idPromovido,T) ; // Divide a folha se estiver cheia
            printf("folha %s dividida com sucesso!! criada %s\n",node->filhos[i],novaFolha);
            // Insere o novo jogador na folha dividida
            if(strcmp(novoJogador->id, idPromovido) < 0)
                inserirJogadorNaFolhaOrdenado(node->filhos[i], novoJogador);
            else
                inserirJogadorNaFolhaOrdenado(novaFolha, novoJogador);

            for(int j=node->numKeys; j > i; j--){
                strcpy(node->keys[j], node->keys[j-1]); // Move as chaves para a direita
                strcpy(node->filhos[j+1], node->filhos[j]); // Move os filhos para a direita
            }
            printf("ID promovido: %s\n", idPromovido);

            node->numKeys++; // Incrementa o número de chaves no nó
            strcpy(node->keys[i], idPromovido); // Insere o ID do novo
            strcpy(node->filhos[i+1], novaFolha); // Insere o nome da nova folha no filho correspondente
            fseek(fi,atual*tamanhoNo(T)+sizeof(int), SEEK_SET);
            printChaves(node); // Imprime as chaves do nó para depuração
            printFilhos(node); // Imprime os filhos do nó para depuração
            salvarNo(fi, node,T); // Salva o nó atualizado no arquivo de índice
            liberaNo(node,T); // Libera a memória alocada para o nó
            fclose(fi); // Fecha o arquivo de índice
            return; // Retorna após inserir o jogador na folha
        }
        //--------------------------------------------------------------------------------------------------

        // Se não for uma folha cheia, insere o jogador na folha
        printf("Inserindo jogador id:%s na folha: %s\n",novoJogador->id, node->filhos[i]);
        inserirJogadorNaFolhaOrdenado(node->filhos[i],novoJogador);
        fclose(fi); // Fecha o arquivo de índice
        liberaNo(node,T); // Libera a memória alocada para o nó
        printf("Jogador inserido na folha com sucesso.\n\n\n");
        return; // Retorna após inserir o jogador na folha
    }
    // Se não for folha de valores, significa que é um nó interno (tem filhos) ->vai salvar o ofset do filho correspondente
    char nomeFilho[ARQ_SIZE];
    strcpy(nomeFilho, node->filhos[i]);
    int nextNodeIndex = atoi(nomeFilho);
    printf("pulando para o próximo nó: %s\n", nomeFilho);

    liberaNo(node,T);
    fclose(fi);
    Pai = atual; // Atualiza o nó Pai para o nó atual
    InsereJogador(nomeArqIndex, novoJogador, nextNodeIndex,Pai,T); // Chama recursivamente a função para o próximo nó
    return; // Retorna após inserir o jogador na folha
}

//---------------------------------------------------------------------------------------------------------------------------------


//FUNÇOES PARA IMPRIMIR A ÁRVORE B+
//-----------------------------------------------------------------------------------------------------------------------------------
// Função para imprimir a árvore B+ (apenas para depuração)
void imprimirArvoreAux(char *nomeArqIndex, int n, int nivel,int T) {
    FILE *fi = fopen(nomeArqIndex, "rb");
    if (!fi) {
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }

    fseek(fi, sizeof(int) + n * tamanhoNo(T), SEEK_SET); // Lê o nó do arquivo de índice
    Tnode *node = lerNo(fi,T); // Cria um novo nó para armazenar os dados lidos do arquivo
    fclose(fi);
    
    for (int i = node->numKeys; i >= 0; i--) {
        if (strncmp(node->filhos[i], "leaf", 4) == 0) {
            // É folha, imprime os dados da folha
            printFolhaImpressa(node->filhos[i], nivel + 1);
        } else {
            // É nó interno, converte offset para int e chama recursivamente
            int offsetFilho = atoi(node->filhos[i]);
            imprimirArvoreAux(nomeArqIndex, offsetFilho, nivel + 1,T);
        }

        if (i > 0) {
            for (int j = 0; j < nivel; j++) printf("\t");
            printf("chave: %s\n", node->keys[i - 1]);
        }
    }

    liberaNo(node,T); // Libera a memória alocada para o nó
}

void imprimirArvoreB(int T){
    printf("Imprimindo a árvore B+:\n");
    imprimirArvoreAux("index.bin", 0, 0,T); // Começa do nó raiz (índice 0) e nível 0
    printf("Árvore B+ impressa com sucesso.\n");
}
//-----------------------------------------------------------------------------------------------------------------------------------


//FUNÇÃO PARA BUSCAR UM JOGADOR NA Arvore B+
//-----------------------------------------------------------------------------------------------------------------------------------
// FUNÇÃO PARA BUSCAR UM JOGADOR NA ÁRVORE B+
// Parâmetros:
// - id: ID do jogador a ser buscado (formato "1231986BRA")
// - n: índice do nó no arquivo de índice (começando de 0)
// Retorno:
// - Um ponteiro para a estrutura Tplayer contendo os dados do jogador, ou NULL se não encontrado.
// Observação: NA CHAMADA DA FUNÇÃO O n DEVE SER 0 para acesssar o primeiro nó da árvore B+
Tplayer *buscarJogador(char *id,int n,int T) {
    //printf("Buscando jogador com ID: %s\n", id);
    if (id == NULL || n < 0) {
        printf("ID inválido ou índice negativo.\n");
        return NULL;
    }
    FILE *fi = fopen("index.bin","rb"); // Abre o arquivo de índice
    if (fi == NULL) {
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }


    fseek(fi, n * tamanhoNo(T) + sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó n
    Tnode *node = lerNo(fi,T); // Lê o nó do arquivo de índice

    int i = 0;
    while((i < node->numKeys) && (strcmp(node->keys[i], id) <= 0)) { // Percorre as chaves do nó até encontrar a posição correta
        i++;
    }

    //printf("Chave a ser buscada: %i\n", i);
    if(strncmp(node->filhos[i], "leaf", 4) == 0) { // Se for uma folha de valores (significa que é um nó folha)
        //printf("Buscando jogador na folha: %s\n", node->filhos[i]);
        // filhos[i] aqui contém nome do arquivo da folha 
        fclose(fi);
        Tplayer *jogador = buscaBinariaJogadorNaFolha(node->filhos[i], id);
        liberaNo(node,T); // Libera a memória alocada para o nó
        return jogador; // Busca o jogador na folha correspondente
    }

    // Se não for folha de valores, significa que é um nó interno (tem filhos) ->vai salvar o ofset do filho correspondente
    char nomeFilho[ARQ_SIZE];
    strcpy(nomeFilho, node->filhos[i]);
    int nextNodeIndex = atoi(nomeFilho);
    //printf("Próximo nó: %s\n", nomeFilho);

    liberaNo(node,T); // Libera a memória alocada para o nó
    fclose(fi);

    return buscarJogador(id, nextNodeIndex,T); // Chama recursivamente a função para o próximo nó
}

//Funçao EXCLUIR UM JOGADOR DA ARVORE B+


//-----------------------------------------------------------------------------------------------------------------------------------
Tplayer *Retirafolha(char *nomeArqFolha, char *id){
    Tplayer *playerRetirado;
    playerRetirado = buscaBinariaJogadorNaFolha(nomeArqFolha, id); // Busca o jogador com o ID especificado na folha
    if(playerRetirado == NULL) { // Se o jogador não for encontrado
        printf("Jogador com ID %s não encontrado na folha %s.\n", id, nomeArqFolha);
        free(playerRetirado); // Libera a memória alocada para o jogador
        return NULL; // Retorna NULL se o jogador não for encontrado
    }

    //SE ELE TA NA FOLHA,ENTAO PRECISA SER RETIRADO DA FOLHA
    printf("Removendo jogador com ID %s da folha %s.\n", id, nomeArqFolha);

    //SALVA A PROXIMA FOLHA
    char ProximaFolha[ARQ_SIZE];
    getProximaFolha(nomeArqFolha, ProximaFolha); //pega a ultima folha da lista de folhas

    long limiteLeitura = limiteFolha(nomeArqFolha); // Obtém o tamanho do arquivo menos o espaço para o nome da próxima folha

    FILE *ff = fopen(nomeArqFolha, "rb+"); // Abre o arquivo da folha para leitura e escrita
    FILE *fTemp = fopen("temp.bin", "wb"); // Cria um arquivo temporário para salvar os dados da folha
    if (ff == NULL || fTemp == NULL) {
        printf("Erro ao abrir o arquivo da folha ou criar o arquivo temporário.\n");
        exit(1);
    }

    Tplayer *player = malloc(sizeof(Tplayer)); // Aloca memória para o jogador
    // Lê os jogadores da folha e escreve no arquivo temporário, exceto o jogador a ser removido
    while (ftell(ff) < limiteLeitura && fread(player, sizeof(Tplayer), 1, ff) == 1) {
        if (strcmp(player->id, id) != 0) { // Se o ID do jogador não for igual ao ID a ser removido
            fwrite(player, sizeof(Tplayer), 1, fTemp); // Escreve o jogador no arquivo temporário
        } else {
            printf("Jogador com ID %s removido da folha %s.\n", id, nomeArqFolha);
        }
    }
    fclose(ff); // Fecha o arquivo da folha
    free(player); // Libera a memória alocada para o jogador
    
    // Escreve a próxima folha no final do arquivo temporário
    fseek(fTemp,0,SEEK_END);
    fwrite(ProximaFolha, sizeof(char), ARQ_SIZE, fTemp); // Escreve o nome da próxima folha no arquivo temporário
    fclose(fTemp); // Fecha o arquivo temporário
    if (remove(nomeArqFolha) != 0) { // Remove o arquivo da folha original
        printf("Erro ao remover o arquivo da folha.\n");
        free(player); // Libera a memória alocada para o jogador
        exit(1);
    }
    if (rename("temp.bin", nomeArqFolha) != 0) { // Renomeia o arquivo temporário para o nome da folha original
        printf("Erro ao renomear o arquivo temporário.\n");
        free(player); // Libera a memória alocada para o jogador
        exit(1);
    }
    return playerRetirado;
}

Tplayer *Caso3aFolha(char *nomeArqFolha, char *id, char **idPromovido, char *folhaIrmao, int PosicaoFolhaIrmao) {

    Tplayer *playerRetirado = Retirafolha(nomeArqFolha, id); // Remove o jogador com o ID especificado da folha atual

    if(PosicaoFolhaIrmao == 1){//FOLHA IRMA DIREITA
        //Abre o arquivo da folha irma a direita
        FILE *fi = fopen(folhaIrmao, "rb+"); 
        if (fi == NULL) {
            printf("Erro ao abrir o arquivo da folha irmã direita.\n");
            exit(1);
        }

        Tplayer *player = malloc(sizeof(Tplayer)); // Aloca memória para o jogador

        //Salva o id do SEGUNDO jogador da folha irmã direita para promove-lo
        fseek(fi,sizeof(Tplayer), SEEK_SET); // Move o ponteiro do arquivo para a segunda posiçao
        fread(player, sizeof(Tplayer), 1, fi); // Lê o segundo jogador da folha irmã direita
        strcpy(*idPromovido, player->id); // Copia o ID do jogador lido para o ID promovido

        //pego o primerio jogador da folha irmã direita para inserir na folha filho
        fseek(fi, 0, SEEK_SET); // Move o ponteiro do arquivo para o final
        fread(player, sizeof(Tplayer), 1, fi); // Lê o último jogador da folha irmã direita
        fclose(fi); // Fecha o arquivo da folha irmã direita

        Retirafolha(folhaIrmao, player->id); // Remove o jogador lido da folha atual
        inserirJogadorNaFolhaOrdenado(nomeArqFolha, player); // Insere o jogador lido na folha atual

        free(player); // Libera a memória alocada para o jogador
    } else { //O irmao é a esquerda
        //Abre o arquivo da folha irma a esquerda
        FILE *fi = fopen(folhaIrmao, "rb+"); 
        if (fi == NULL) {
            printf("Erro ao abrir o arquivo da folha irmã esquerda.\n");
            exit(1);
        }

        Tplayer *player = malloc(sizeof(Tplayer)); // Aloca memória para o jogador
        //pego o ultimo jogador da folha irmã esquerda para promover
        fseek(fi, -(sizeof(Tplayer)+(sizeof(char)*ARQ_SIZE)), SEEK_END); // Move o ponteiro do arquivo para o último jogador
        fread(player, sizeof(Tplayer), 1, fi); // Lê o último jogador da folha irmã esquerda
        strcpy(*idPromovido, player->id); // Copia o ID do jogador lido para o ID promovido

        fclose(fi); // Fecha o arquivo da folha irmã esquerda
        
        Retirafolha(folhaIrmao,player->id); // Remove o jogador lido da folha atual
        inserirJogadorNaFolhaOrdenado(nomeArqFolha, player); // Insere o jogador lido na folha atual
        free(player); // Libera a memória alocada para o jogador
    }

    return playerRetirado; // Retorna o jogador removido
}

Tplayer *Caso3bFolha (char *nomeArqFolha, char *id, char *folhaIrmao, int PosicaoFolhaIrmao) {
    Tplayer *playerRetirado;
    playerRetirado = Retirafolha(nomeArqFolha, id); // Remove o jogador com o ID especificado da folha atual

    if(PosicaoFolhaIrmao == 1){//FOLHA IRMA DIREITA
        printf("Caso 3b: Folha irmã direita.\n");
        FILE *ff = fopen(nomeArqFolha, "rb+"); // Abre o arquivo da folha irmã direita
        if(ff == NULL) {
            printf("Erro ao abrir o arquivo da folha irmã direita.\n");
            exit(1);
        }


        //PEGO A PROXIMA FOLHA DA FILHA IRMA DIREITA
        char ProximaFolha[ARQ_SIZE];
        getProximaFolha(folhaIrmao, ProximaFolha); // P
        
        long limiteLeituraIrmao = limiteFolha(folhaIrmao); // Obtém o tamanho do arquivo menos o espaço para o nome da próxima folha

        FILE *fi = fopen(folhaIrmao, "rb+"); // Abre o arquivo da folha irmã direita
        if (fi == NULL) {
            printf("Erro ao abrir o arquivo da folha irmã direita.\n");
            exit(1);
        }
        
        Tplayer *player = malloc(sizeof(Tplayer)); // Aloca memória para o jogador

        fseek(ff,-(sizeof(char)*ARQ_SIZE), SEEK_END); // Move o ponteiro do arquivo para o final
        while(ftell(fi) < limiteLeituraIrmao && fread(player, sizeof(Tplayer), 1, fi) == 1) { // Lê os jogadores da folha irmã direita
            fwrite(player, sizeof(Tplayer), 1, ff); // Escreve os jogadores no arquivo da folha filha
        }

        fseek(ff, 0, SEEK_END); // Move o ponteiro do arquivo da folha irmã direita para o final
        fclose(fi); // Fecha o arquivo da folha irmã direita
        free(player); // Libera a memória alocada para o jogador
        fwrite(ProximaFolha, sizeof(char), ARQ_SIZE, ff); // Escreve o nome da próxima folha no final do arquivo da folha filha
        fclose(ff); // Fecha o arquivo da folha filha
        if (remove(folhaIrmao) != 0) { // Remove o arquivo da folha irmã direita
            printf("Erro ao remover o arquivo da folha irmã direita.\n");
            exit(1);
        }

        // Adiciona o número da folha irmã direita ao arquivo NumeroFolhasExcluidas.bin
        int numeroFolha = ReadLeafNumber(nomeArqFolha); // Lê o número da folha filha
        FILE *faux = fopen("NumeroFolhasExcluidas.bin", "ab");
        if(faux == NULL) {
            printf("Erro ao abrir o arquivo NumeroFolha.bin.\n");
            exit(1);
        }
        fwrite(&numeroFolha, sizeof(int), 1, faux); // Escreve
        fclose(faux); // Fecha o arquivo NumeroFolha.bin

        // Renomeia o arquivo da folha filha para o nome da folha irmã direita
        if(rename(nomeArqFolha, folhaIrmao) == 0){
            printf("arquivo folha filha renomeada para o irmao(CASO 3B)");
        }else{
            printf("erro ao renomear");
        }// Renomeia o arquivo da folha filha para o nome da folha irmã direita
        printf("Arquivo da folha filha renomeado para a folha irmã direita com sucesso.\n");


    } else { //O irmao é a esquerda
        printf("Caso 3b: Folha irmã esquerda.\n");
        //Abre o arquivo da folha irma a esquerda
        FILE *fi = fopen(folhaIrmao, "rb+"); 
        if (fi == NULL) {
            printf("Erro ao abrir o arquivo da folha irmã esquerda.\n");
            exit(1);
        }

        //PEGO A PROXIMA FOLHA DA FILHA
        char ProximaFolha[ARQ_SIZE];
        getProximaFolha(nomeArqFolha, ProximaFolha); // Pega a próxima folha da folha filha

        long limiteLeituraFilha = limiteFolha(nomeArqFolha); // Obtém o tamanho do arquivo menos o espaço para o nome da próxima folha

        FILE *ff = fopen(nomeArqFolha, "rb+");
        if(ff == NULL) {
            printf("Erro ao abrir o arquivo da folha filha.\n");
            exit(1);
        }

        Tplayer *player = malloc(sizeof(Tplayer)); // Aloca memória para o jogador
        fseek(fi, -(sizeof(char)*ARQ_SIZE), SEEK_END); // Move o ponteiro do arquivo da folha filha para o final do arquivo irmao
        while(ftell(ff) < limiteLeituraFilha && fread(player, sizeof(Tplayer), 1, ff) == 1) { // Lê os jogadores da folha filha
            fwrite(player, sizeof(Tplayer), 1, fi); // Escreve os jogadores no arquivo da folha irmã esquerda
        }

        fclose(ff); // Fecha o arquivo da folha filha
        free(player); // Libera a memória alocada para o jogador
        fseek(fi,0,SEEK_END);
        fwrite(ProximaFolha, sizeof(char), ARQ_SIZE, fi); // Escreve o nome da próxima folha no final do arquivo da folha irmã esquerda
        fclose(fi); // Fecha o arquivo da folha irmã esquerda

        if (remove(nomeArqFolha) != 0) { // Remove o arquivo da folha filha
            printf("Erro ao remover o arquivo da folha filha.\n");
            exit(1);
        }

        int numeroFolha = ReadLeafNumber(folhaIrmao); // Lê o número da folha irmã esquerda
        
        if(rename(folhaIrmao,nomeArqFolha) == 0){
            printf("arquivo renomeado com sucesso");
        }else{
            printf("erro ao renomear o arquivo");
        }

        // Adiciona o número da folha irmã esquerda ao arquivo NumeroFolha.bin
        FILE *faux = fopen("NumeroFolhasExcluidas.bin", "ab");
        if(faux == NULL) {
            printf("Erro ao abrir o arquivo NumeroFolha.bin.\n");
            exit(1);
        }
        fwrite(&numeroFolha, sizeof(int), 1, faux); // Escreve o número da folha irmã esquerda no arquivo NumeroFolha.bin
        fclose(faux); // Fecha o arquivo NumeroFolha.bin
    }

    return playerRetirado; // Retorna o jogador removido
}

//caso 3B no interno
//erro no retira chave promovida
void Caso3aNoInterno(char *nomeArqIndex,int ofsetFilho, int ofsetIrmao,char **chavePromovida,int T,int PosicaoNoIrmao) {
    //O CHAVE PROMOVIDA INICIALMENTE E A CHAVE DO PAI QUE VAI DESCER PARA O FILHO DPS SERA ALTERADA PARA A CHAVE DO PAI
    if(PosicaoNoIrmao == 1){//Irmao a direita do no filho
        printf("Caso 3A IRMAO A DIREITA: No filho %d e no irmao %d\n", ofsetFilho, ofsetIrmao);
        FILE *fi = fopen(nomeArqIndex, "rb+");
        if (fi == NULL) {
            printf("Erro ao abrir o arquivo de índice.\n");
            exit(1);
        }

        fseek(fi, (ofsetIrmao * tamanhoNo(T))+sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó irmao
        Tnode *noIrmao = lerNo(fi,T); // Lê o nó irmao do arquivo de índice
        fseek(fi, (ofsetFilho * tamanhoNo(T))+sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó filho
        Tnode *noFilho = lerNo(fi,T); // Lê o nó filho
        if (noIrmao == NULL || noFilho == NULL) {
            printf("Erro ao ler os nós do arquivo de índice.\n");
            fclose(fi);
            exit(1);
        }

        //alterando no filho(passando a chave do pai para o filho)
        noFilho->numKeys++; // Incrementa o número de chaves no nó filho
        strcpy(noFilho->keys[noFilho->numKeys - 1], *chavePromovida); // Insere a chave promovida no nó filho
        strcpy(noFilho->filhos[noFilho->numKeys], noIrmao->filhos[0]); // Insere o primeiro filho do nó irmao no nó filho
        noFilho->isOcuped = 1; // Marca o nó filho como ocupado
        //salvando o filho
        fseek(fi, (ofsetFilho * tamanhoNo(T)) + sizeof(int), SEEK_SET); // Move
        salvarNo(fi, noFilho,T); // Salva o nó filho atualizado no arquivo de índice

        //alterando no irmao
        strcpy(*chavePromovida, noIrmao->keys[0]); // Copia a primeira chave do nó irmao para a chave promovida

        for(int i = 0; i < noIrmao->numKeys; i++){
            if(i != noIrmao->numKeys - 1) // Se nao for a última chave
                strcpy(noIrmao->keys[i], noIrmao->keys[i + 1]); // Move as chaves para a esquerda

            strcpy(noIrmao->filhos[i], noIrmao->filhos[i + 1]); // Move os filhos para a esquerda
        }

        noIrmao->numKeys--; // Decrementa o número de chaves no nó irmao
        noIrmao->isOcuped = 1; // Marca o nó irmao como ocupado
        //salvando o irmao
        fseek(fi, (ofsetIrmao * tamanhoNo(T))+sizeof(int), SEEK_SET); //
        salvarNo(fi, noIrmao,T); // Salva o nó irmao atualizado no arquivo de índice
        liberaNo(noFilho,T); // Libera a memória alocada para o nó filho
        liberaNo(noIrmao,T); // Libera a memória alocada para o nó
        fclose(fi); // Fecha o arquivo de índice
        printf("Caso 3A concluído: Nó filho atualizado com a chave promovida e o filho do nó irmao.\n");
        return;
    }else{ // irmao à esquerda do filho
        printf("Caso 3A IRMAO A ESQUERDA: No filho %d e no irmao %d\n", ofsetFilho, ofsetIrmao);
        FILE *fi = fopen(nomeArqIndex, "rb+");
        if (fi == NULL) {
            printf("Erro ao abrir o arquivo de índice.\n");
            exit(1);
        }

        fseek(fi, (ofsetIrmao * tamanhoNo(T)) + sizeof(int), SEEK_SET);
        Tnode *noIrmao = lerNo(fi,T);
        fseek(fi, (ofsetFilho * tamanhoNo(T)) + sizeof(int), SEEK_SET);
        Tnode *noFilho = lerNo(fi,T);

        if (noIrmao == NULL || noFilho == NULL) {
            printf("Erro ao ler os nós do arquivo de índice.\n");
            fclose(fi);
            exit(1);
        }
        printf("Chave despromovida do pai: %s\n", *chavePromovida);

        // NOVA chave promovida
        char novaPromovida[ID_SIZE];
        strcpy(novaPromovida, noIrmao->keys[noIrmao->numKeys - 1]);
        printf("Chave promovida: %s\n", novaPromovida);

        // Deslocar filhos primeiro, de i = numKeys até 1 (inclusive)
        for (int i = noFilho->numKeys+1; i > 0; i--) {
            strcpy(noFilho->filhos[i], noFilho->filhos[i - 1]);
        }

        // Agora deslocar chaves de i = numKeys - 1 até 0
        for (int i = noFilho->numKeys - 1; i >= 0; i--) {
            strcpy(noFilho->keys[i + 1], noFilho->keys[i]);
        }

        // Insere chave promovida e ponteiro do irmao
        strcpy(noFilho->keys[0], *chavePromovida);
        strcpy(noFilho->filhos[0], noIrmao->filhos[noIrmao->numKeys]);

        noFilho->numKeys++;
        noFilho->isOcuped = 1;

        // Atualiza chave promovida para o pai
        strcpy(*chavePromovida, novaPromovida);

        // Remove chave e ponteiro do irmao
        noIrmao->numKeys--;
        noIrmao->isOcuped = 1;

        // Salva
        fseek(fi, (ofsetFilho * tamanhoNo(T)) + sizeof(int), SEEK_SET);
        salvarNo(fi, noFilho, T);
        fseek(fi, (ofsetIrmao * tamanhoNo(T)) + sizeof(int), SEEK_SET);
        salvarNo(fi, noIrmao, T);

        liberaNo(noFilho, T);
        liberaNo(noIrmao, T);
        fclose(fi);

        printf("Caso 3A concluído (irmao à esquerda): nó filho atualizado com chave promovida e ponteiro do irmão.\n");
        return;
    }   
}

void Caso3bNoInternoPai1chave(char *nomeArqIndex, int ofsetEsquerda, int ofsetDireita, int ofsetPai, int T) {
    FILE *fi = fopen(nomeArqIndex, "rb+");
    if (fi == NULL) {
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }

    Tnode *noEsquerda, *noDireita, *noPai;

    fseek(fi, (ofsetDireita * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    noDireita = lerNo(fi, T);
    fseek(fi, (ofsetEsquerda * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    noEsquerda = lerNo(fi, T);
    fseek(fi, (ofsetPai * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    noPai = lerNo(fi, T);

    if (!noEsquerda || !noDireita || !noPai) {
        printf("Erro ao ler os nós do arquivo de índice.\n");
        fclose(fi);
        exit(1);
    }

    // Verificação de overflow
    if (noEsquerda->numKeys + 1 + noDireita->numKeys > 2*T - 1) {
        printf("Erro: Fusão causaria overflow no nó interno.\n");
        fclose(fi);
        exit(1);
    }

    Tnode *noNovo = criaNo(T);
    noNovo->isOcuped = 1;

    // Copia chaves e filhos do esquerda
    for (int i = 0; i < noEsquerda->numKeys; i++) {
        strcpy(noNovo->keys[i], noEsquerda->keys[i]);
        strcpy(noNovo->filhos[i], noEsquerda->filhos[i]);
    }
    // Copia o ponteiro extra do esquerda
    strcpy(noNovo->filhos[noEsquerda->numKeys], noEsquerda->filhos[noEsquerda->numKeys]);
    noNovo->numKeys = noEsquerda->numKeys;

    // Insere chave do pai
    strcpy(noNovo->keys[noNovo->numKeys], noPai->keys[0]);
    noNovo->numKeys++;

    // Copia chaves e filhos do direita
    for (int j = 0; j < noDireita->numKeys; j++) {
        strcpy(noNovo->keys[noNovo->numKeys + j], noDireita->keys[j]);
        strcpy(noNovo->filhos[noNovo->numKeys + j], noDireita->filhos[j]);
    }

    // Ponteiro extra final
    strcpy(noNovo->filhos[noNovo->numKeys + noDireita->numKeys], noDireita->filhos[noDireita->numKeys]);

    noNovo->numKeys += noDireita->numKeys;

    // Salva o novo nó no lugar do pai
    fseek(fi, (ofsetPai * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    salvarNo(fi, noNovo, T);

    liberaNo(noNovo, T);
    liberaNo(noPai, T);

    // Limpa esquerda
    noEsquerda->isOcuped = 0;
    noEsquerda->numKeys = 0;
    fseek(fi, (ofsetEsquerda * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    salvarNo(fi, noEsquerda, T);
    liberaNo(noEsquerda, T);

    // Limpa direita
    noDireita->isOcuped = 0;
    noDireita->numKeys = 0;
    fseek(fi, (ofsetDireita * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    salvarNo(fi, noDireita, T);
    liberaNo(noDireita, T);

    fclose(fi);
    printf("Caso 3B (única chave no pai) concluído com sucesso.\n");
}

//caso 3B no interno
void Caso3bNoInterno(char *nomeArqIndex,int ofsetEsquerda, int ofsetDireita, int ofsetPai, char *nomeEsquerda, int i, int T, int PosicaoNoIrmao){
    FILE *fi = fopen(nomeArqIndex,"rb+");
    if (fi == NULL) {
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }

    Tnode *noEsquerda, *noDireita, *noPai;

    fseek(fi, (ofsetDireita * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    noDireita = lerNo(fi, T);
    fseek(fi, (ofsetEsquerda * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    noEsquerda = lerNo(fi, T);
    fseek(fi, (ofsetPai * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    noPai = lerNo(fi, T);

    char idDespromovido[ID_SIZE];
    int inicioShift = (PosicaoNoIrmao == 1) ? i : i - 1;

    strcpy(idDespromovido, noPai->keys[inicioShift]);

    // Shifta chaves
    for (int j = inicioShift; j < noPai->numKeys - 1; j++) {
        strcpy(noPai->keys[j], noPai->keys[j + 1]);
    }

    // Shifta filhos
    for (int j = inicioShift + 1; j < noPai->numKeys; j++) {
        strcpy(noPai->filhos[j], noPai->filhos[j + 1]);
    }

    // Copia o último ponteiro (posição numKeys)
    strcpy(noPai->filhos[noPai->numKeys - 1], noPai->filhos[noPai->numKeys]);
    noPai->numKeys--; // Decrementa o número de chaves no nó pai

    // Adiciona a chave despromovida no final do nó esquerdo
    strcpy(noEsquerda->keys[noEsquerda->numKeys], idDespromovido);
    noEsquerda->numKeys++;

    int inicio = noEsquerda->numKeys;

    for (int j = 0; j < noDireita->numKeys; j++) {
        strcpy(noEsquerda->keys[inicio + j], noDireita->keys[j]);
        strcpy(noEsquerda->filhos[inicio + j], noDireita->filhos[j]);
    }
    strcpy(noEsquerda->filhos[inicio + noDireita->numKeys], noDireita->filhos[noDireita->numKeys]);

    noEsquerda->numKeys += noDireita->numKeys;

    // Zera e salva nó direito
    noDireita->isOcuped = 0;
    noDireita->numKeys = 0;
    fseek(fi, (ofsetDireita * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    salvarNo(fi, noDireita, T);
    liberaNo(noDireita, T);

    // Salva nó esquerdo
    noEsquerda->isOcuped = 1;
    fseek(fi, (ofsetEsquerda * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    salvarNo(fi, noEsquerda, T);
    liberaNo(noEsquerda, T);

    // Atualiza ponteiro do pai
    int posFilhoQueFicou = (PosicaoNoIrmao == 1) ? i : i - 1;
    strcpy(noPai->filhos[posFilhoQueFicou], nomeEsquerda);
    noPai->isOcuped = 1;
    fseek(fi, (ofsetPai * tamanhoNo(T)) + sizeof(int), SEEK_SET);
    salvarNo(fi, noPai, T);
    liberaNo(noPai, T);

    fclose(fi);
    printIndexTotal(nomeArqIndex, T); // Imprime o índice atualizado
    printf("Caso 3B concluído: Nó esquerdo atualizado com a chave despromovida e os filhos do nó direito.\n");
}

//caso 3B no interno
Tplayer *Removerjogador(char *nomeArqIndex, char *id, int T,int atual, int Pai) {
    printf("Removendo jogador com ID: %s\n", id);
    FILE *fi = fopen(nomeArqIndex, "rb+"); // Abre o arquivo de índice para leitura e escrita
    if (fi == NULL) {
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }

    fseek(fi,(atual * tamanhoNo(T)) + sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó atual
    Tnode *nodePai = lerNo(fi,T); // Lê o nó do arquivo de índice
    if(nodePai == NULL) { // Verifica se o nó foi lido corretamente
        printf("Erro ao ler o nó do arquivo de índice.\n");
        fclose(fi);
        exit(1);
    }
    int i = 0;
    while((i < nodePai->numKeys) && (strcmp(nodePai->keys[i], id) <= 0)) i++; // Percorre as chaves do nó até encontrar a posição
    printf("Chave a ser removida: %i\n", i);
    printf("Número de chaves no nó pai: %d\n", nodePai->numKeys);
    printf("o filho a ser acessado é: %s\n", nodePai->filhos[i]);

    //CASO FOR FOLHA
    if(strncmp(nodePai->filhos[i],"leaf",4) == 0){
        printf("O nó atual é uma folha: %s\n", nodePai->filhos[i]);
        Tplayer *jogadorRemovido;
        printf("quanto é o T: %d\n", (T-1));
        if(contaElementosFolha(nodePai->filhos[i]) == (T-1)) { // Se a folha tiver apenas um elemento
            printf("A folha %s tem apenas %d elementos, removendo jogador %s.\n", nodePai->filhos[i],T-1,id);
            char FolhaIrmao[ARQ_SIZE];
            int posicaoFolhaIrmao = 0; // Variável para armazenar a posição da folha irmã
            int caso = 0;
            printf("o i é: %d\n", i);
            if(i == 0){
                printf("Folha irmã à direita: %s\n", nodePai->filhos[i+1]);
                strcpy(FolhaIrmao, nodePai->filhos[i+1]); // Folha irmã à direita
                posicaoFolhaIrmao = i+1; // Folha irmã à direita
                if(contaElementosFolha(nodePai->filhos[i+1]) >= T){
                    caso = 0; // Caso 3A
                }else if(contaElementosFolha(nodePai->filhos[i+1]) == (T-1)){
                    caso = 1; // Caso 3B
                }
                printf("Folha irmã à direita: %s\n", FolhaIrmao);
            }else if(i == nodePai->numKeys) {
                strcpy(FolhaIrmao, nodePai->filhos[i-1]); // Folha irmã à esquerda
                posicaoFolhaIrmao = i-1; // Folha irmã à esquerda
                if(contaElementosFolha(nodePai->filhos[i-1]) >= T){
                    caso = 0; // Caso 3A
                }else if(contaElementosFolha(nodePai->filhos[i-1]) == (T-1)){
                    caso = 1; // Caso 3B
                }
                printf("Folha irmã à esquerda: %s\n", FolhaIrmao);
            } else {

                if(contaElementosFolha(nodePai->filhos[i-1]) >= T) {
                    strcpy(FolhaIrmao, nodePai->filhos[i-1]); // Folha irmã à esquerda
                    posicaoFolhaIrmao = i-1; // Folha irmã à esquerda
                    caso = 0;
                    printf("Folha irmã à esquerda: %s\n", FolhaIrmao);  
                }else if(contaElementosFolha(nodePai->filhos[i+1]) >= T) {
                    strcpy(FolhaIrmao, nodePai->filhos[i+1]); // Folha irmã à direita
                    posicaoFolhaIrmao = i+1; // Folha irmã à direita
                    caso = 0;
                    printf("Folha irmã à direita: %s\n", FolhaIrmao);
                }else if(contaElementosFolha(nodePai->filhos[i-1]) == (T-1)){
                    strcpy(FolhaIrmao, nodePai->filhos[i-1]); // Folha irmã à esquerda
                    posicaoFolhaIrmao = i-1; // Folha irmã à esquerda
                    caso = 1;
                    printf("Folha irmã à esquerda: %s\n", FolhaIrmao);
                }else{
                    strcpy(FolhaIrmao, nodePai->filhos[i+1]); // Folha irmã à direita
                    posicaoFolhaIrmao = i+1; // Folha irmã à direita
                    caso = 1;
                    printf("Folha irmã à direita: %s\n", FolhaIrmao);
                }
            }

            if(caso == 0){//CASO 3A
                printf("Caso 3A: Folha irmã à esquerda ou direita.\n");
                char *idPromovido = malloc(sizeof(char) * ID_SIZE); // Aloca memória para o ID promovido
                jogadorRemovido = Caso3aFolha(nodePai->filhos[i],id,&idPromovido, FolhaIrmao, posicaoFolhaIrmao-i); // Remove o jogador da folha
                if(posicaoFolhaIrmao == i+1) {
                    strcpy(nodePai->keys[i], idPromovido); // Atualiza a chave do pai
                } else {
                    strcpy(nodePai->keys[i-1], idPromovido); // Atualiza a chave do pai
                }
                free(idPromovido); // Libera a memória alocada para o ID promovido
                fseek(fi, (atual * tamanhoNo(T)) + sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó pai
                salvarNo(fi, nodePai,T); // Salva o nó pai atualizado no arquivo de índice
                liberaNo(nodePai,T); // Libera a memória alocada para o nó pai
                fclose(fi); // Fecha o arquivo de índice
                return jogadorRemovido; // Retorna o jogador removido
            } else {//CASO 3B
                printf("Caso 3B: Folha irmã à esquerda ou direita.\n");
                jogadorRemovido = Caso3bFolha(nodePai->filhos[i], id,FolhaIrmao, posicaoFolhaIrmao-i); // Remove o jogador da folha
                if (posicaoFolhaIrmao - i == 1) {
                    for (int j = i; j < nodePai->numKeys - 1; j++) {
                        strcpy(nodePai->keys[j], nodePai->keys[j + 1]);
                        strcpy(nodePai->filhos[j], nodePai->filhos[j + 1]);
                    }
                    strcpy(nodePai->filhos[nodePai->numKeys - 1], nodePai->filhos[nodePai->numKeys]);
                } else {
                    for (int j = i - 1; j < nodePai->numKeys - 1; j++) {
                        strcpy(nodePai->keys[j], nodePai->keys[j + 1]);
                        strcpy(nodePai->filhos[j], nodePai->filhos[j + 1]);
                    }
                    strcpy(nodePai->filhos[nodePai->numKeys - 1], nodePai->filhos[nodePai->numKeys]);
                }
                nodePai->numKeys--; // Decrementa o número de chaves no nó pai
                nodePai->isOcuped = 1; // Marca o nó pai como ocupado
                fseek(fi, (atual * tamanhoNo(T)) + sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó pai
                salvarNo(fi, nodePai,T); // Salva o nó pai atualizado no arquivo de índice
                liberaNo(nodePai,T); // Libera a memória alocada para o nó pai
                fclose(fi); // Fecha o arquivo de índice
                return jogadorRemovido; // Retorna o jogador removido
            }
                
        }
        printf("Removendo jogador %s da folha %s.\n", id, nodePai->filhos[i]);
        jogadorRemovido = Retirafolha(nodePai->filhos[i], id); // Remove o jogador da folha
        fclose(fi); // Fecha o arquivo de índice
        liberaNo(nodePai,T); // Libera a memória alocada para o nó pai
        return jogadorRemovido; // Retorna o jogador removido
    }
    
    
    // Se não for uma folha, significa que é um nó interno (tem filhos)
    char nomeFilho[ARQ_SIZE];
    strcpy(nomeFilho, nodePai->filhos[i]); // Copia o nome do filho correspondente
    int ofsetFilho = atoi(nomeFilho); // Converte o nome do filho para
    fseek(fi, ((ofsetFilho * tamanhoNo(T))+sizeof(int)), SEEK_SET); // Move o ponteiro do arquivo para o filho correspondente
    Tnode *filho = lerNo(fi,T); // Lê o nó filho
    if (filho == NULL) {
        printf("Erro ao ler o nó filho do arquivo de índice.\n");
        fclose(fi);
        liberaNo(nodePai,T); // Libera a memória alocada para o nó pai
        exit(1);
    }

    // Verifica se o nó filho tem apenas T-1 chaves
    if(filho->numKeys == (T-1)){
        printf("O nó filho %s tem apenas %d elementos, removendo jogador %s.\n", nomeFilho, T-1, id);
        char nomeIrmao[ARQ_SIZE];
        Tnode *noIrmaoEsquerda, *noIrmaoDireita;
        int posicaoIrmao = 0;
        int ofsetIrmao = 0;
        int caso = 0;
        //caso for o primeiro filho entao o unico irmao possivel é o da direita
        if(i == 0) {
            strcpy(nomeIrmao, nodePai->filhos[i+1]); // Folha irmã
            posicaoIrmao = i+1; // Folha irmã à direita
            ofsetIrmao = atoi(nomeIrmao); // Converte o nome do filho para
            fseek(fi, ((ofsetIrmao * tamanhoNo(T))+ sizeof(int)), SEEK_SET); // Move o ponteiro do arquivo para o nó irmão
            noIrmaoDireita = lerNo(fi,T); // Lê o nó irmão do arquivo de indice
            if(noIrmaoDireita->numKeys >= T){
                caso = 0; // Caso 3A
            }else{
                caso = 1; // Caso 3B
            }
            liberaNo(noIrmaoDireita,T); // Libera a memória alocada para o nó irmão direito
        } else if(i == nodePai->numKeys) {//caso for o ultimo filho entao o unico irmao possivel é o da esquerda
            strcpy(nomeIrmao, nodePai->filhos[i-1]); // Folha irmã
            posicaoIrmao = i-1; // Folha irmã à esquerda
            ofsetIrmao = atoi(nomeIrmao); // Converte o nome do filho para
            fseek(fi, (ofsetIrmao * tamanhoNo(T)) + sizeof(int), SEEK_SET); // Move
            noIrmaoEsquerda = lerNo(fi,T); // Lê o nó irmão do arquivo de indice
            if(noIrmaoEsquerda->numKeys >= T){
                caso = 0; // Caso 3A
            }else{
                caso = 1; // Caso 3B
            }
            liberaNo(noIrmaoEsquerda,T); // Libera a memória alocada para o nó irmão esquerdo
        }else {//caso for um filho do meio entao o irmao pode ser a esquerda ou direita
            int ofsetIrmaoEsquerda, ofsetIrmaoDireita;
            strcpy(nomeIrmao, nodePai->filhos[i-1]); // Folha irmã à esquerda
            ofsetIrmaoEsquerda = atoi(nomeIrmao); // Converte o nome do filho para
            fseek(fi, (ofsetIrmaoEsquerda * tamanhoNo(T))+sizeof(int), SEEK_SET); // Move
            noIrmaoEsquerda = lerNo(fi,T); // Lê o nó irmão do arquivo de

            strcpy(nomeIrmao, nodePai->filhos[i+1]); // Folha irmã à direita
            ofsetIrmaoDireita = atoi(nomeIrmao); // Converte o nome do filho para
            fseek(fi, (ofsetIrmaoDireita * tamanhoNo(T))+sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó irmão
            noIrmaoDireita = lerNo(fi,T); // Lê o nó irmão do
            printf("Número de chaves no nó irmão esquerdo: %d\n", noIrmaoEsquerda->numKeys);
            printf("Número de chaves no nó irmão direito: %d\n", noIrmaoDireita->numKeys);
            if(noIrmaoEsquerda->numKeys >= T){
                caso = 0; // Caso 3A
                posicaoIrmao = i-1; // Folha irmã à esquerda
                ofsetIrmao = ofsetIrmaoEsquerda;
                strcpy(nomeIrmao, nodePai->filhos[i-1]); // Folha irmã à esquerda
            }else if(noIrmaoDireita->numKeys >= T){
                caso = 0; // Caso 3A
                posicaoIrmao = i+1; // Folha irmã à direita
                ofsetIrmao = ofsetIrmaoDireita;
                strcpy(nomeIrmao, nodePai->filhos[i+1]); // Folha irmã à direita
            }else if(noIrmaoEsquerda->numKeys == (T-1)){
                caso = 1; // Caso 3B
                posicaoIrmao = i-1; // Folha irmã à esquerda
                ofsetIrmao = ofsetIrmaoEsquerda; // Converte o nome do filho para
                strcpy(nomeIrmao, nodePai->filhos[i-1]); // Folha irmã à esquerda
            }else{
                caso = 1; // Caso 3B
                posicaoIrmao = i+1; // Folha irmã à direita
                ofsetIrmao = ofsetIrmaoDireita; // Converte o nome do filho para
                strcpy(nomeIrmao, nodePai->filhos[i+1]); // Folha irmã à direita
            }
            liberaNo(noIrmaoEsquerda,T); // Libera a memória alocada para o nó irmão esquerdo
            liberaNo(noIrmaoDireita,T); // Libera a memória alocada para
        }
        
        printf("Posicão do nó irmão: %d\n", posicaoIrmao);
        printf("posiçao no filho: %d\n", i);

        //caso 3a no interno
        if(caso == 0){
            printf("Caso 3A: Nó filho %d e nó irmão %d.\n", ofsetFilho, ofsetIrmao);
            char *chavePromovida = malloc(sizeof(char) * ID_SIZE); // Aloca memória para a chave promovida
            if(posicaoIrmao-i == 1){
                strcpy(chavePromovida, nodePai->keys[i]);
            }else{
                strcpy(chavePromovida, nodePai->keys[i-1]); // Copia a chave do pai para a chave promovida
            } // Copia a chave do pai para a chave promovida
            fclose(fi); // Fecha o arquivo de índice
            Caso3aNoInterno(nomeArqIndex, ofsetFilho, ofsetIrmao, &chavePromovida, T, posicaoIrmao-i); // Chama a função para o caso 3A irmão
            fi = fopen(nomeArqIndex, "rb+"); // Reabre o arquivo de índice para escrita
            if (fi == NULL) {
                printf("Erro ao abrir o arquivo de índice.\n");
                exit(1);
            }
            fseek(fi, (atual * tamanhoNo(T)) + sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó pai
            nodePai = lerNo(fi,T); // Lê o nó pai atualizado do arquivo de índice
            if((posicaoIrmao - i) == 1) {
                strcpy(nodePai->keys[i], chavePromovida); // Atualiza a chave do pai
            } else {
                strcpy(nodePai->keys[i-1], chavePromovida); // Atualiza a chave do pai
            }
            fseek(fi, (atual * tamanhoNo(T)) + sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o nó pai
            salvarNo(fi, nodePai,T); // Salva o nó pai atualizado no arquivo de
            fclose(fi); // Fecha o arquivo de índice
            Pai = atual;
            atual = ofsetFilho; // Atualiza o índice do nó atual para o filho
            free(chavePromovida); // Libera a memória alocada para a chave promovida
            liberaNo(nodePai,T); // Libera a memória alocada para o nó pai
            liberaNo(filho,T); // Libera a memória alocada para o nó filho
            return Removerjogador(nomeArqIndex,id,T,atual,Pai); // Retorna NULL pois não há jogador removido neste caso
        } else {//caso 3b no interno
            printf("Caso 3B: Nó filho %d e nó irmão %d.\n", ofsetFilho, ofsetIrmao);
            fclose(fi); // Fecha o arquivo de índice
            if(nodePai->numKeys == 1) {
                if((posicaoIrmao - i) == 1) {
                    Caso3bNoInternoPai1chave(nomeArqIndex, ofsetFilho, ofsetIrmao, atual, T); // Chama a função para o caso 3B no nó interno raiz
                } else {
                    Caso3bNoInternoPai1chave(nomeArqIndex, ofsetIrmao, ofsetFilho, atual, T); // Chama a função para o caso 3B no nó interno raiz
                }
            } else {
                if((posicaoIrmao - i) == 1) {
                    printf("Caso 3B irmao direita: Nó filho %d e nó irmão %d.\n", ofsetFilho, ofsetIrmao);
                    Caso3bNoInterno(nomeArqIndex, ofsetFilho, ofsetIrmao, atual, nomeFilho, i, T, posicaoIrmao-i); // Chama a função para o caso 3B no nó interno
                } else {
                    printf("Caso 3B irmao esquerda: Nó filho %d e nó irmão %d.\n", ofsetFilho, ofsetIrmao);
                    Caso3bNoInterno(nomeArqIndex, ofsetIrmao, ofsetFilho, atual, nomeIrmao, i, T, posicaoIrmao-i); // Chama a função para o caso 3B no nó interno
                }
            }
            liberaNo(nodePai,T); // Libera a memória alocada para o nó pai
            liberaNo(filho,T); // Libera a memória alocada para o nó filho
            return Removerjogador(nomeArqIndex,id,T,atual,Pai); // Retorna NULL pois não há jogador removido neste caso
        }
    }

    
    //Se nao tem problemas ir para o proximo no
    printf("O nó filho %s tem mais de %d elementos, removendo jogador %s.\n", nomeFilho, T-1, id);
    Pai = atual; // Atualiza o índice do nó pai
    atual = ofsetFilho; // Atualiza o índice do nó atual para o filho
    fclose(fi); // Fecha o arquivo de índice
    liberaNo(nodePai,T); // Libera a memória alocada para o nó pai
    liberaNo(filho,T); // Libera a memória alocada para o nó filho
    return Removerjogador(nomeArqIndex,id,T,atual,Pai);
}

//-----------------------------------------------------------------------------------------------------------------------------------

//FUNÇÃO PARA MATAR A ARVORE B+

//mata folhas
void killFolha(char *nomeFolha) {
    if (remove(nomeFolha) != 0) {
        printf("Erro ao remover a folha: %s\n", nomeFolha);
    } else {
        printf("Folha %s removida com sucesso.\n", nomeFolha);
    }
    
}

void KillArvoreBaux(char *nomeArqIndex,int T) {
    FILE *fi = fopen(nomeArqIndex, "rb");
    if (fi == NULL) {
        printf("Erro ao abrir o arquivo de índice.\n");
        exit(1);
    }

    Tnode *node;
    fseek(fi, sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para
    while ((node = lerNo(fi,T)) != NULL) { // Lê todos os nós do arquivo
        for (int i = 0; i < node->numKeys+1; i++) {
            if(strncmp(node->filhos[i], "leaf", 4) == 0) { // Verifica se é uma folha
                printf("Removendo folha: %s\n", node->filhos[i]);
                killFolha(node->filhos[i]); // Mata a folha correspondente
            }
            
        }
        liberaNo(node,T); // Libera a memória alocada para o nó
    }

    fclose(fi); // Fecha o arquivo de índice
}

void KillArvoreB(int T) {
    KillArvoreBaux("index.bin",T); // Mata as folhas da árvore B+
    if (remove("index.bin") != 0) { // Remove o arquivo de índice
        printf("Erro ao remover o arquivo de índice.\n");
    } else {
        printf("Arquivo de índice removido com sucesso.\n");
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------