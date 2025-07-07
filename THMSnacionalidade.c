#include "THMSnacionalidade.h"

void maiuscula(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

unsigned int hash_nacionalidade(const char* str) {
    unsigned int hash = 0;
    while (*str)
        hash = (hash * 31 + *str++) % N_NAC;
    return hash;
}

void THnacionalidade_inicializa(char *arqHash, char *arqDados) {
    FILE *fh = fopen(arqHash,"wb"), *fd = fopen(arqDados,"wb");
    if((!fh) || (!fd)) exit(1);
    // Incializa hash
    int end = -1;
    for(int i = 0; i < N_NAC; i++) fwrite(&end,sizeof(int),1,fh);
    fclose(fh);
    fclose(fd);
}

void THnacionalidade_insere(char *arqHash, char *arqDados, char *id) {

    FILE *fh = fopen(arqHash,"rb+"), *fd = fopen(arqDados,"rb+");
    if((!fd) || (!fh)) exit(1);

    int hash, pos, ant = -1, ppl = -1;
    char nacionalidade[4];

    strncpy(nacionalidade,&id[5],3);
    nacionalidade[3] = '\0';

    hash = hash_nacionalidade(nacionalidade);
    //printf("%d ",hash);

    fseek(fh, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fh);
    //printf("%d ",pos);

    THnacionalidade aux;

    while(pos != -1) {
        fseek(fd, pos, SEEK_SET);
        fread(&aux, sizeof(THnacionalidade), 1, fd);
        if(!strcmp(aux.id,id)) {
            fclose(fh);
            fclose(fd);
            return;
        }
        
        if((!aux.status) && (ppl == -1)) ppl = pos;
        ant = pos;
        pos = aux.proximo;
    }

    THnacionalidade elem;
    strcpy(elem.id,id);
    //printf("%s ",elem.id);
    elem.status = 1;

    if(ppl == -1) {
        elem.proximo = -1;
        fseek(fd, 0L, SEEK_END);
        pos = ftell(fd);
        fwrite(&elem, sizeof(THnacionalidade), 1, fd);
        if(ant == -1) {
            fseek(fh, hash * sizeof(int), SEEK_SET);
            fwrite(&pos, sizeof(int), 1, fh);
        }
        else {
            aux.proximo = pos;
            fseek(fd, ant, SEEK_SET);
            fwrite(&aux, sizeof(THnacionalidade), 1, fd);
        }
        fclose(fh);
        fclose(fd);
        return;
    }

    fseek(fd, ppl, SEEK_SET);
    fread(&aux, sizeof(THnacionalidade), 1, fd);
    elem.proximo = aux.proximo;
    fseek(fd, ppl, SEEK_SET);
    fwrite(&elem, sizeof(THnacionalidade), 1, fd);
    
    fclose(fh);
    fclose(fd);
}

void THnacionalidade_construcao(char *arqPlayers, char *arqHash, char *arqDados) {
    FILE *fp = fopen(arqPlayers,"r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de jogadores.\n");
        exit(1);
    }

    char linha[256];

    THnacionalidade_inicializa(arqHash, arqDados);

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
        if (token) {
            char nacionalidade[4];
            strcpy(nacionalidade, token);
            if(!strcmp(nacionalidade,"Australia")) strcpy(j.nacionalidade,"AUT\0");
            else if(!strcmp(nacionalidade,"Belarus")) strcpy(j.nacionalidade,"BLR\0");
            else if(!strcmp(nacionalidade,"Great Britain")) strcpy(j.nacionalidade,"GBR\0");
            else if(!strcmp(nacionalidade,"Greece")) strcpy(j.nacionalidade,"GRC\0");
            else {
                strncpy(j.nacionalidade, token,3);
                maiuscula(j.nacionalidade); // Converte nacionalidade para maiúsculas
            }
        }
        j.nacionalidade[3] = '\0'; // Garante que a string de nacionalidade tenha o tamanho correto
        //printf("%s ",j.nacionalidade);

        char *id;
        id = THnome_busca("hash_nome.bin", "dados_nome.bin", j.nome);

        THnacionalidade_insere(arqHash, arqDados, id);
        free(id); // Libera a memória alocada para o ID do jogador
    
    }

    fclose(fp);
}

void THnacionalidade_retira(char *arqHash, char *arqDados, char *id) {
    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);
    int hash, pos;
    char nacionalidade[4];

    strncpy(nacionalidade,&id[5],3);
    nacionalidade[3] = '\0';

    hash = hash_nacionalidade(nacionalidade);

    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1) return;
    fp = fopen(arqDados,"rb+");
    if(!fp) exit(1);
    THnacionalidade aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THnacionalidade), 1, fp);
        if(!strcmp(aux.id,id)) {
            if(!aux.status) {
                fclose(fp);
                return;
            }
            aux.status = 0;
            fseek(fp, pos, SEEK_SET);
            fwrite(&aux,sizeof(THnacionalidade),1,fp);
            fclose(fp);
            return;
        }
        pos = aux.proximo;
    }
    fclose(fp);
    return;   
}

char *THnacionalidade_busca(char *arqHash, char *arqDados, char *id) {
    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);
    int hash, pos;
    char nacionalidade[4];

    strncpy(nacionalidade,&id[5],3);
    nacionalidade[3] = '\0';

    hash = hash_nacionalidade(nacionalidade);

    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1) return "\0";
    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);
    THnacionalidade aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THnacionalidade), 1, fp);
        if(!strcmp(aux.id,id)) {
            fclose(fp);
            if(!aux.status) {
                return "\0";
            }
            char *idRetorno = malloc(ID_SIZE * sizeof(char));
            strcpy(idRetorno, id);
            return idRetorno;
        }
        pos = aux.proximo;
    }
    fclose(fp);
    return "\0";
}

// int main() {

//     THnacionalidade_construcao("tennis_players.txt", "hash_nacionalidade.bin","dados_nacionalidade.bin");
// }