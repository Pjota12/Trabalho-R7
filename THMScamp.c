#include "THMScamp.h"

int hash_camp(int coluna){
  return coluna % TAM_HASH_CAMP;
}

void THcamp_inicializa(char *tabHash, char *dados){
    FILE *fh = fopen(tabHash,"wb"), *fd = fopen(dados,"wb");
    if((!fh) || (!fd)) exit(1);
    // Incializa hash
    int end = -1;
    for(int i = 0; i < TAM_HASH_CAMP; i++) fwrite(&end,sizeof(int),1,fh);
    fclose(fh);
    fclose(fd);
}
  
char *THcamp_busca_sobrenome(char *tabHashNome, char *dadosNome, char *sobrenome) {
    FILE *fp = fopen(tabHashNome, "rb");
    if (!fp) exit(1);

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
                if (strcasecmp(sobrenome_jogador, sobrenome) == 0) {
                    char *id = malloc(ID_SIZE * sizeof(char));
                    strcpy(id, jogador.id);
                    fclose(fd);
                    fclose(fp);
                    return id;
                }
            }
            pos = jogador.proximo;
        }
        fclose(fd);
    }
    fclose(fp);
    return NULL;
}

void THcamp_insere(char *arqHash, char *arqDados, char *id, int coluna, int ano) {

    FILE *fh = fopen(arqHash,"rb+"), *fd = fopen(arqDados,"rb+");
    if((!fh) || (!fd)) exit(1);

    int hash = hash_camp(coluna);
    int pos, ant = -1, ppl = -1;

    fseek(fh, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fh);

    THcamp aux;

    while(pos != -1) {
        fseek(fd, pos, SEEK_SET);
        fread(&aux, sizeof(THcamp), 1, fd);
        
        if((!aux.status) && (ppl == -1)) ppl = pos;
        ant = pos;
        pos = aux.prox;
    }

    printf("\ncoluna [%d]: \n", coluna);
    THcamp elem;

    elem.ano = ano;
    strcpy(elem.id,id);
    printf("%s ",elem.id);
    elem.status = 1;

    if(ppl == -1) {
        elem.prox = -1;
        fseek(fd, 0L, SEEK_END);
        pos = ftell(fd);
        fwrite(&elem, sizeof(THcamp), 1, fd);
        if(ant == -1) {
            fseek(fh, hash * sizeof(int), SEEK_SET);
            fwrite(&pos, sizeof(int), 1, fh);
        }
        else {
            aux.prox = pos;
            fseek(fd, ant, SEEK_SET);
            fwrite(&aux, sizeof(THcamp), 1, fd);
        }
        fclose(fh);
        fclose(fd);
        return;
    }

    fseek(fd, ppl, SEEK_SET);
    fread(&aux, sizeof(THcamp), 1, fd);
    elem.prox = aux.prox;
    fseek(fd, ppl, SEEK_SET);
    fwrite(&elem, sizeof(THcamp), 1, fd);
    
    fclose(fh);
    fclose(fd);
}

void THcamp_construcao(char *arqChampions, char *arqHash, char *arqDados) {
    FILE *fp = fopen(arqChampions,"r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de campiões.\n");
        exit(1);
    }

    THcamp_inicializa(arqHash,arqDados);

    char linha[1024];
    fgets(linha, sizeof(linha), fp); // pula cabeçalho

    while (fgets(linha, sizeof(linha), fp)) {
        char *token = strtok(linha, "\\");
        int ano = atoi(token);
        int coluna = -1; // começa com -1 para ignorar o ano

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
                    char *id = THcamp_busca_sobrenome("hash_nome.bin", "dados_nome.bin", token);
                    if (id) {
                        THcamp_insere(arqHash, arqDados, id, coluna, ano);
                        free(id);
                    }
                }
            }

            token = strtok(NULL, "\\");
            coluna++;
        }
    }
}

char *THcamp_ano_busca(char *arqHash, char *arqDados, char *id, int coluna) {
    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);

    int pos, hash = hash_camp(coluna);
    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1) return "\0";

    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);
    THcamp aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THcamp), 1, fp);
        if(!strcmp(aux.id,id) && aux.status) {
            if(!aux.status) {
                fclose(fp);
                return "\0";
            }
            char* idRetorno = malloc(ID_SIZE * sizeof(char));
            strcpy(idRetorno, aux.id);
            fclose(fp);
            return idRetorno;
        }
        pos = aux.prox;
    }
    fclose(fp);
    return "\0";
}

void THcamp_ano_remove(char *arqHash, char *arqDados, char *id, int coluna) {
    FILE *fh = fopen(arqHash, "rb"), *fd = fopen(arqDados, "rb+");
    if (!fh || !fd) exit(1);

    int hash = hash_camp(coluna), pos;

    // Busca o início da lista encadeada para o ano
    fseek(fh, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fh);
    fclose(fh);

    THcamp aux;
    while (pos != -1) {
        fseek(fd, pos, SEEK_SET);
        fread(&aux, sizeof(THcamp), 1, fd);

        if (strcmp(aux.id, id) == 0 && aux.status == 1) {
            aux.status = 0; 
            fseek(fd, pos, SEEK_SET);
            fwrite(&aux, sizeof(THcamp), 1, fd);
            break;
        }
        pos = aux.prox;
    }
    fclose(fd);
}

int main() {
    THcamp_construcao("champions.txt","hash_campeonatos.bin","dados_campeonatos.bin");
}
