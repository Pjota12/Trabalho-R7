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
        return NULL;
    }

    for (int i = 0; i < 2 * T - 1; i++) {
        if (fread(node->keys[i], sizeof(char), ID_SIZE, fi) != ID_SIZE) {
            printf("Erro ao ler as chaves do nó.\n");
            return NULL;
        }
    }

    for (int i = 0; i < 2 * T; i++) {
        if (fread(node->filhos[i], sizeof(char), ARQ_SIZE, fi) != ARQ_SIZE) {
            printf("Erro ao ler os filhos do nó.\n");
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
    printf("Buscando jogador com ID: %s\n", id);
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

    printf("Chave a ser buscada: %i\n", i);
    if(strncmp(node->filhos[i], "leaf", 4) == 0) { // Se for uma folha de valores (significa que é um nó folha)
        printf("Buscando jogador na folha: %s\n", node->filhos[i]);
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
    printf("Próximo nó: %s\n", nomeFilho);

    liberaNo(node,T); // Libera a memória alocada para o nó
    fclose(fi);

    return buscarJogador(id, nextNodeIndex,T); // Chama recursivamente a função para o próximo nó
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