#include "THMSativos.h"

void THativos_inicializa(char *arqHash, char *arqDados) {
    FILE *fh = fopen(arqHash,"wb"), *fd = fopen(arqDados,"wb");
    if((!fh) || (!fd)) exit(1);
    // Incializa hash
    int end = -1;
    for(int i = 0; i < 2; i++) fwrite(&end,sizeof(int),1,fh);
    fclose(fh);
    fclose(fd);
}

void THativos_insere(char *arqHash, char *arqDados, char *id) {

    FILE *fh = fopen(arqHash,"rb+"), *fd = fopen(arqDados,"rb+");
    if((!fh) || (!fd)) exit(1);

    int hash = 1, pos, ant = -1, ppl = -1, anoNascimento;
    char strAnoNascimento[4];

    strncpy(strAnoNascimento,&id[1],4);
    anoNascimento = atoi(strAnoNascimento);

    if(ANO_ATUAL - anoNascimento >= 39) {
        hash = 0;
    }
    //printf("%d ",hash);

    fseek(fh, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fh);
    //printf("%d ",pos);

    THativos aux;

    while(pos != -1) {
        fseek(fd, pos, SEEK_SET);
        fread(&aux, sizeof(THativos), 1, fd);
        if(!strcmp(aux.id,id)) {
            fclose(fh);
            fclose(fd);
            return;
        }
        
        if((!aux.status) && (ppl == -1)) ppl = pos;
        ant = pos;
        pos = aux.proximo;
    }

    THativos elem;
    strcpy(elem.id,id);
    //printf("%s ",elem.id);
    elem.status = 1;

    if(ppl == -1) {
        elem.proximo = -1;
        fseek(fd, 0L, SEEK_END);
        pos = ftell(fd);
        fwrite(&elem, sizeof(THativos), 1, fd);
        if(ant == -1) {
            fseek(fh, hash * sizeof(int), SEEK_SET);
            fwrite(&pos, sizeof(int), 1, fh);
        }
        else {
            aux.proximo = pos;
            fseek(fd, ant, SEEK_SET);
            fwrite(&aux, sizeof(THativos), 1, fd);
        }
        fclose(fh);
        fclose(fd);
        return;
    }

    fseek(fd, ppl, SEEK_SET);
    fread(&aux, sizeof(THativos), 1, fd);
    elem.proximo = aux.proximo;
    fseek(fd, ppl, SEEK_SET);
    fwrite(&elem, sizeof(THativos), 1, fd);
    
    fclose(fh);
    fclose(fd);
}

void THativos_construcao(char *arqPlayers, char *arqHash, char *arqDados) {
    
    FILE *fp = fopen(arqPlayers,"r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo de jogadores.\n");
        exit(1);
    }

    char linha[256];

    THativos_inicializa(arqHash,arqDados);

    while (fgets(linha, sizeof(linha), fp)) {
        Tplayer j;

        char *token = strtok(linha, "\\");
        if (token != NULL) strcpy(j.nome, token);

        if(strcmp(j.nome, "Name") == 0) continue; // Pula o cabeçalho

        token = strtok(NULL, "\\");
        j.anoNascimento = token ? atoi(token) : -1;

        char *id;
        id = THnome_busca("hash_nome.bin", "dados_nome.bin", j.nome);
          
        THativos_insere("hash_ativos.bin","dados_ativos.bin",id);
        free(id); // Libera a memória alocada para o ID do jogador
    }
    
    fclose(fp);
}

void THativos_retira(char *arqHash, char *arqDados, char *id) {
    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);
    int hash = 1, anoNascimento, pos;
    char strAnoNascimento[4];

    strncpy(strAnoNascimento,&id[1],4);
    anoNascimento = atoi(strAnoNascimento);

    if(ANO_ATUAL - anoNascimento >= 39) {
        hash = 0;
    }
    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1) return;
    fp = fopen(arqDados,"rb+");
    if(!fp) exit(1);
    THativos aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THativos), 1, fp);
        if(!strcmp(aux.id,id)) {
            if(!aux.status) {
                fclose(fp);
                return;
            }
            aux.status = 0;
            fseek(fp, pos, SEEK_SET);
            fwrite(&aux,sizeof(THativos),1,fp);
            fclose(fp);
            return;
        }
        pos = aux.proximo;
    }
    fclose(fp);
    return;
}

char *THativos_busca(char *arqHash, char *arqDados, char *id) {
    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);
    int hash = 1, anoNascimento, pos;
    char strAnoNascimento[4];

    strncpy(strAnoNascimento,&id[1],4);
    anoNascimento = atoi(strAnoNascimento);

    if(ANO_ATUAL - anoNascimento >= 39) {
        hash = 0;
    }
    fseek(fp, hash * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1) return "\0";
    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);
    THativos aux;
    while(pos != -1) {
        fseek(fp, pos, SEEK_SET);
        fread(&aux, sizeof(THativos), 1, fp);
        if(!strcmp(aux.id,id)) {
            if(!aux.status) {
                fclose(fp);
                return "\0";
            }
            char *idRetorno = malloc(ID_SIZE * sizeof(char));
            strcpy(idRetorno, id);
            fclose(fp);
            return idRetorno;
        }
        pos = aux.proximo;
    }
    fclose(fp);
    return "\0";
}

// int main() {

//     THativos_construcao("tennis_players.txt","hash_ativos.bin","dados_ativos.bin");
// }
