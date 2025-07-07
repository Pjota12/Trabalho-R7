#include "THMSnome.h"

int hash_nome(char *Nome){
  unsigned long hash = 3203; // Inicializa o hash com um valor primo
  int c;
  while ((c = *Nome++)) {
    hash = ((hash << 5) + hash) + tolower((unsigned char)c); // Multiplica por 31 e adiciona o caractere
  }
  return hash % TAM_HASH; // Retorna o índice na tabela hash
}

void minuscula(char *str) {
  while (*str) {
    *str = tolower((unsigned char)*str);
    str++;
  }
}

void THnome_inicializa(char *tabHash, char *dados) {
  FILE *fd = fopen(dados, "wb");
  if (fd == NULL) {
    printf("Erro ao abrir o arquivo de hash.\n");
    exit(1);
  }
  fclose(fd);
  FILE *fp = fopen(tabHash, "wb");
  if (fp == NULL) {
    printf("Erro ao abrir o arquivo de tabela hash.\n");
    exit(1);
  }
  int elem = -1;
  for (int i = 0; i < TAM_HASH; i++) {
    fwrite(&elem, sizeof(int), 1, fp); // Inicializa a tabela hash com -1
  }  
  fclose(fp);
}

char *THnome_busca(char *tabHash, char *dados, char *nome) {
  FILE *fp = fopen(tabHash, "rb");
  if (fp == NULL) {
    printf("Erro ao abrir o arquivo de tabela hash.\n");
    exit(1);
  }
  int index = hash_nome(nome); // Calcula o índice na tabela hash
  int pos;
  fseek(fp, index * sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fp); // Lê a posição do registro
  fclose(fp);

  if (pos == -1) return NULL; // Registro não encontrado

  FILE *fd = fopen(dados, "rb");
  if (fd == NULL) {
    printf("Erro ao abrir o arquivo de dados.\n");
    exit(1);
  }

  minuscula(nome); // Converte o nome para minúsculas para comparação

 // Move para a posição do registro
  THnomeToid registro;

  while(1)
  {
    fseek(fd,pos,SEEK_SET);
    fread(&registro, sizeof(THnomeToid), 1, fd); // Lê o registro na posição atual
    if((registro.status == 1) && (strcmp(registro.nome, nome) == 0)) {
      char *id = malloc(ID_SIZE * sizeof(char)); // Aloca memória para o ID
      strcpy(id, registro.id); // Copia o ID do registro encontrado
      fclose(fd);
      return id; // Retorna o ID se o nome for encontrado
    }
    if(registro.proximo == -1) {
      fclose(fd);
      return NULL; // Retorna NULL se não encontrar o nome
    }
    pos = registro.proximo; // Atualiza a posição para o próximo registro
  }
  fclose(fd);
}

void THnome_insere(char *tabHash, char *dado, char *nome, char *id){
  FILE *fph = fopen(tabHash, "rb+");
  if(!fph) exit(1);
  int pos, h = hash_nome(nome); // Calcula o índice na tabela hash
  fseek(fph, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph);
  int ant, prim_pos_livre;
  ant = prim_pos_livre = -1;
  FILE *fp = fopen(dado,"rb+");
  if(!fp){
    fclose(fph);
    exit(1);
  }

  THnomeToid aux;
  minuscula(nome); // Converte o nome para minúsculas para comparação

  while(pos != -1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THnomeToid), 1, fp);
    if((aux.status == 1) && (strcmp(aux.nome, nome) == 0)) {
      aux.status = 1; // Marca o registro como ocupado
      strcpy(aux.id, id); // Atualiza o ID do registro
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(THnomeToid), 1, fp);
      fclose(fp);
      fclose(fph);
      return;
    }
    if((!aux.status) && (prim_pos_livre == -1))prim_pos_livre=pos;
    ant = pos;
    pos = aux.proximo; // Atualiza a posição para o próximo registro
  }
  if(prim_pos_livre == -1){
    strcpy(aux.nome, nome);
    strcpy(aux.id, id);
    aux.proximo = -1;
    aux.status = 1;
    fseek(fp, 0L, SEEK_END);
    pos = ftell(fp);
    fwrite(&aux, sizeof(THnomeToid), 1, fp);
    if(ant != -1){
      fseek(fp, ant, SEEK_SET);
      fread(&aux, sizeof(THnomeToid), 1, fp);
      aux.proximo = pos;
      fseek(fp, ant, SEEK_SET);
      fwrite(&aux, sizeof(THnomeToid), 1, fp);
    }
    else{
      fseek(fph, h*sizeof(int), SEEK_SET);
      fwrite(&pos, sizeof(int), 1, fph);
    }
    fclose(fp);
    fclose(fph);
    return;
  }
  fseek(fp, prim_pos_livre, SEEK_SET);
  fread(&aux, sizeof(THnomeToid), 1, fp);
  strcpy(aux.nome, nome);
  strcpy(aux.id, id);
  aux.status = 1;
  fseek(fp, prim_pos_livre, SEEK_SET);
  fwrite(&aux, sizeof(THnomeToid), 1, fp);
  fclose(fp);
  fclose(fph);
  return;
}

void THnome_imprime(char *nome_hash, char *nome_dados, int m) {
  FILE *fp = fopen(nome_hash, "rb");
  if (fp == NULL) {
    printf("Erro ao abrir o arquivo de tabela hash.\n");
    exit(1);
  }
  int pos;
  for (int i = 0; i < m; i++) {
    fseek(fp, i * sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp); // Lê a posição do registro
    if (pos != -1) {
      FILE *fd = fopen(nome_dados, "rb");
      if (fd == NULL) {
        printf("Erro ao abrir o arquivo de dados.\n");
        fclose(fp);
        exit(1);
      }
      THnomeToid registro;
      while (pos != -1) {
        fseek(fd, pos, SEEK_SET);
        fread(&registro, sizeof(THnomeToid), 1, fd); // Lê o registro na posição atual
        if (registro.status == 1) {
          printf("Nome: %s, ID: %s\n", registro.nome, registro.id); // Imprime o nome e ID
        }
        pos = registro.proximo; // Atualiza a posição para o próximo registro
      }
      fclose(fd);
    }
  }
  fclose(fp);
}

void THnome_retira(char *tabHash, char *dado, char *nome) {
  FILE *fph = fopen(tabHash, "rb+");
  if (!fph) {
    printf("Erro ao abrir o arquivo de tabela hash.\n");
    exit(1);
  }
  int h = hash_nome(nome); // Calcula o índice na tabela hash
  int pos;
  fseek(fph, h * sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph); // Lê a posição do registro

  FILE *fp = fopen(dado, "rb+");
  if (!fp) {
    fclose(fph);
    printf("Erro ao abrir o arquivo de dados.\n");
    exit(1);
  }

  THnomeToid aux;
  minuscula(nome); // Converte o nome para minúsculas para comparação

  while (pos != -1) {
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THnomeToid), 1, fp);
    if ((aux.status == 1) && (strcmp(aux.nome, nome) == 0)){
      aux.status = 0; // Marca o registro como não ocupado
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(THnomeToid), 1, fp);
      fclose(fp);
      fclose(fph);
      return;
    }
    pos = aux.proximo; // Atualiza a posição para o próximo registro
  }
  
  fclose(fp);
  fclose(fph);
}