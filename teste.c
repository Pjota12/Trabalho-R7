#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define bloco 32
// bloco = 1 int + (2t - 1) int + 2t char[4]

int qs(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void dividir(char *indice, int t, int *folhas, int *nos, char *folha, int no) {
    char novaFolha[10] = "f", strFolhas[10];
    sprintf(strFolhas, "%d", *folhas + 1);
    strcat(novaFolha,strFolhas);
    strcat(novaFolha,".bin");

    FILE *fi, *ff1 = fopen(folha,"rb+"), *ff2 = fopen(novaFolha,"wb"), *temp = fopen("temp.bin","wb");
    if((!ff1) || (!ff2) || (!temp)) exit(1);

    int vet[(2*t)-1], elem, i;
    fread(vet,sizeof(vet),1,ff1);
    char prox[4] = "";
    fread(prox,sizeof(prox),1,ff1);

    char nome[4] = "";

    if(*folhas == 1) {
        fi = fopen(indice,"wb");
        if((!fi)) exit(1);

        // cria arq fi
        (*nos)++;
        int chaves = 1, aux = 0;
        fwrite(&chaves,sizeof(int),1,fi);
        elem = vet[t-1];
        fwrite(&elem,sizeof(int),1,fi);
        for(i = 0; i < (2*t)-2; i++) fwrite(&aux,sizeof(int),1,fi); // chaves
        strcpy(nome,"f1");
        
        fwrite(nome,sizeof(nome),1,fi); 
        strcpy(nome,"f2");
        
        fwrite(nome,sizeof(nome),1,fi);
        strcpy(nome,"-");
        
        for(i = 0; i < (2*t)-2; i++) fwrite(nome,sizeof(nome),1,fi); // filhos
    }
    else {
        fi = fopen(indice, "rb+");
        if((!fi)) exit(1);
        
        // arq fi
        fseek(fi,(no - 1) * bloco,SEEK_SET);
        int nChaves;
        fread(&nChaves,sizeof(int),1,fi);
        nChaves++;
        int chaves[nChaves];
        fread(chaves,sizeof(chaves),1,fi);
        chaves[nChaves-1] = vet[t-1];
        qsort(chaves,nChaves,sizeof(int),qs);
        fseek(fi,(no - 1) * bloco,SEEK_SET);
        fwrite(&nChaves,sizeof(int),1,fi);
        fwrite(chaves,sizeof(chaves),1,fi);

        // filhos
        char filhos[2*t][4], novo[4] = "";
        strncpy(novo,novaFolha,2);
        fread(filhos,sizeof(filhos),1,fi);
        for(i = (2*t)-1; i > -1; i--) {
            strcpy(filhos[i],filhos[i-1]);
            if(!strncmp(filhos[i],folha,2)) break;
        }
        strcpy(filhos[i],novo);
        fseek(fi,-sizeof(filhos),SEEK_CUR);
        fwrite(filhos,sizeof(filhos),1,fi);
    }
    // arq f1
    int max = INT_MAX;
    for(i = 0; i < (t-1); i++) {
        elem = vet[i];
        fwrite(&elem,sizeof(int),1,temp);
    }

    // arq f2
    (*folhas)++;
    for(i = t-1; i < (2*t)-1; i++) {
        elem = vet[i];
        fwrite(&elem,sizeof(int),1,ff2);
        fwrite(&max,sizeof(int),1,temp);
    }
    for(i = 0; i < (t-1); i++) fwrite(&max,sizeof(int),1,ff2);
    strcpy(nome,prox);
    fwrite(nome,sizeof(nome),1,ff2);
    
    // ar1 f1
    strncpy(nome,novaFolha,2);
    fwrite(nome,sizeof(nome),1,temp);

    fclose(fi);
    fclose(ff1);
    fclose(ff2);
    fclose(temp);
    if (rename(folha,"lixo.bin") != 0) perror("Erro ao renomear");
    if (rename("temp.bin", folha) != 0) perror("Erro ao renomear");
    if (remove("lixo.bin") != 0) perror("Erro ao remover lixo.bin");
}

void dividir_no(char *indice, int t, int *folhas, int *nos, int no) {
    FILE *fi = fopen(indice,"rb+");
    fseek(fi,((no - 1) * bloco) + sizeof(int),SEEK_SET);
    int chaves[(2*t)-1], filhos[2*t], aux = 1, elem, i;
    char novoNo[4] = "n", strNos[4] = "";
    fread(chaves,sizeof(chaves),1,fi);
    fread(filhos,sizeof(filhos),1,fi);

    // pai
    fseek(fi,(no - 1) * bloco,SEEK_SET);
    fwrite(&aux,sizeof(int),1,fi);
    elem = chaves[t-1];
    fwrite(&elem,sizeof(int),1,fi);
    aux = 0;
    for(i = 0; i < (2*t)-2; i++) fwrite(&aux,sizeof(int),1,fi);
    sprintf(strNos, "%d", *nos + 1);
    strcat(novoNo,strNos);
    fwrite(novoNo,sizeof(novoNo),1,fi);
    strcpy(novoNo,"n");
    sprintf(strNos, "%d", *nos + 2);
    strcat(novoNo,strNos);
    fwrite(novoNo,sizeof(novoNo),1,fi);
    strcpy(novoNo,"-");
    for(i = 0; i < (2*t)-2; i++) fwrite(novoNo,sizeof(int),1,fi);
    
    // no esq
    (*nos)++;
    aux = t - 1;
    fwrite(&aux,sizeof(int),1,fi);
    for(i = 0; i < (t-1); i++) {
        elem = chaves[i];
        fwrite(&elem,sizeof(int),1,fi);
    }
    aux = 0;
    for(i = 0; i < ((2*t)-1) - (t-1); i++) fwrite(&aux,sizeof(int),1,fi);
    for(i = 0; i < t; i++) {
        elem = filhos[i];
        fwrite(&elem,sizeof(int),1,fi);
    }
    for(i = 0; i < (2*t) - t; i++) fwrite(novoNo,sizeof(int),1,fi);

    // no dir
    (*nos)++;
    aux = t - 1;
    fwrite(&aux,sizeof(int),1,fi);
    for(i = t; i < (2*t)-1; i++) {
        elem = chaves[i];
        fwrite(&elem,sizeof(int),1,fi);
    }
    aux = 0;
    for(i = 0; i < t; i++) fwrite(&aux,sizeof(int),1,fi);
    for(i = t; i < (2*t); i++) {
        elem = filhos[i];
        fwrite(&elem,sizeof(int),1,fi);
    }
    for(i = 0; i < t; i++) fwrite(novoNo,sizeof(int),1,fi);
    fclose(fi);
}

void insere_aux(char *indice, int id, int t, int *folhas, int *nos) {
    FILE *fi = fopen(indice, "rb+");
    if(!fi) exit(1);
    int chaves, chave, i, no = 1, nBloco;
    fread(&chaves,sizeof(int),1,fi);
    // if(chaves == (2*t)-1) {
    //     //dividir 
    //     printf("div");
    // }
    // else {
        for(i = 0; i < chaves; i++) {
            fread(&chave,sizeof(int),1,fi);
            if(id < chave) break;
        }
        char aux[4], filho[8];
        fseek(fi,0L,SEEK_SET);
        fseek(fi,sizeof(int) + (((2 * t) - 1) * sizeof(int)) + (i * sizeof(aux)),SEEK_SET);
        fread(filho,sizeof(filho),1,fi);

        while(filho[0] == 'n') {
            char numero[2] = "";
            strncpy(numero,&filho[1],2);
            nBloco = atoi(numero);
            fseek(fi,bloco * nBloco,SEEK_SET);
            fread(&chaves,sizeof(int),1,fi);
            // dividir?
            for(i = 0; i < chaves; i++) {
                fread(&chave,sizeof(int),1,fi);
                if(id < chave) break;
            }
            char aux[4], filho[8];
            fseek(fi,0L,SEEK_SET);
            fseek(fi,sizeof(int) + (((2 * t) - 1) * sizeof(int)) + (i * sizeof(aux)),SEEK_SET);
            fread(filho,sizeof(filho),1,fi);
        }
        if(filho[0] == 'f') {
            strcat(filho,".bin");
            FILE *ff = fopen(filho,"rb+");
            int vet[(2*t)-1], elems, elem;
            fread(vet,sizeof(vet),1,ff);
            for(elems = 0; elems < (2*t)-1; elems++) {
                if(vet[elems] == INT_MAX) break; 
            }
            if(elems < (2*t)-1) {
                // inserindo ordenado
                vet[(2*t)-2] = id;
                qsort(vet,(2*t)-1,sizeof(int),qs);
                fseek(ff,0L,SEEK_SET);
                fwrite(vet,sizeof(vet),1,ff);
                fclose(ff);
            }
            else {
                fclose(ff);
                dividir(indice, t, folhas, nos, filho, no);
                fseek(ff,0L,SEEK_SET);
                insere_aux(indice, id, t, folhas, nos);
            }
        }
    //}
    fclose(fi);
}

void insere(char *indice, int id, int t, int *folhas, int *nos) {
    FILE *fi = fopen(indice, "rb+");
    if(!fi) exit(1);
    if(*folhas == 0) {
        (*folhas)++;
        // cria arq f1
        FILE *ff = fopen("f1.bin", "wb");
        int max = INT_MAX, i;
        char prox[4] = "-";
        fwrite(&id,sizeof(int),1,ff);
        for(i = 0; i < (2*t)-2; i++) fwrite(&max,sizeof(int),1,ff);
        fwrite(prox,sizeof(prox),1,ff);
        fclose(ff);
    }
    else {
        int chaves;
        fread(&chaves,sizeof(int),1,fi);
        fclose(fi);
        if(chaves == 0) {
            FILE *ff1 = fopen("f1.bin", "rb+");
            int vet[(2*t)-1], elems, elem;
            fread(vet,sizeof(vet),1,ff1);
            for(elems = 0; elems < (2*t)-1; elems++) {
                if(vet[elems] == INT_MAX) break; 
            }
            if(elems < (2*t)-1) {
                // inserindo ordenado
                vet[(2*t)-2] = id;
                qsort(vet,(2*t)-1,sizeof(int),qs);
                fseek(ff1,0L,SEEK_SET);
                fwrite(vet,sizeof(vet),1,ff1);
                fclose(ff1);
            }
            else {
                fclose(ff1);
                dividir(indice, t, folhas, nos, "f1.bin", 0);
                insere_aux(indice, id, t, folhas, nos);
            }
        }
        else if(chaves == (2*t)-1) {
            dividir_no(indice, t, folhas, nos, 1);
            insere_aux(indice, id, t, folhas, nos);
        }
        else insere_aux(indice, id, t, folhas, nos);
    }
}

int main(){
    //int argc, char**argv
    // if(argc != 3) printf("Erro na chamada: ./<exec> <players> <champions>\n");
    // else {
        
    // }
    // char players[40] = argv[1], char champions[40] = argv[2], indice[40] = "indice.bin";
    char indice[40] = "indice.bin";
    int t = 2, aux = 0, id, folhas = 0, nos = 0;
    FILE *fi = fopen(indice,"wb");
    if(!fi) return 0;
    fwrite(&aux,sizeof(int),1,fi);
    fclose(fi);
    scanf("%d",&id);
    while(id != -1) {
        insere(indice, id, t, &folhas, &nos);
        scanf("%d",&id);
    }
    return 0;
}
