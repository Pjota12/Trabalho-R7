#include "TABM.h"
#include "TABMaux.h"
#include "Players.h"
#include "THMSnome.h"
#include "THMSativos.h"
#include "THMSnacionalidade.h"
#include "THMSranking.h"
#include "THMScamp_ano.h"
#include "THMScamp.h"
#include "THMStemporada.h"
#include "q1.h"
#include "q2.h"
#include "q3.h"
#include "q4.h"
#include "q5.h"
#include "q6.h"
#include "q7.h"
#include "q8.h"
#include "q9.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*gcc main.c TABM.h TABM.c TABMaux.h TABMaux.c Players.h Players.c THMSnome.h THMSnome.c THMSativos.h THMSativos.c THMSnacionalidade.h THMSnacionalidade.c THMSranking.h THMSranking.c THMScamp_ano.h THMScamp_ano.c THMScamp.h THMScamp.c THMStemporada.h THMStemporada.c q1.h q1.c q2.h q2.c q3.h q3.c q4.h q4.c q5.h q5.c q6.h q6.c q7.h q7.c q8.h q8.c q9.h q9.c -o TE*/

void menu(){
    printf("Menu de Opções:\n");
    printf("1. Inserir Jogador\n");
    printf("2. Buscar Jogador\n");
    printf("3. Buscar Jogador ativo\n");
    printf("4. Buscar Jogador por Pais\n");
    printf("5. Remover Jogador\n");
    printf("6. Imprimir Árvores\n");
    printf("7. questão 1 -Titulos de ativos e aposentados-\n");
    printf("8. questão 2 -Compatriota de quem ganhou granSlam-\n");
    printf("9. questão 3 -Pontuação acumulada-\n");
    printf("10. questão 4 -Top 25 por ano-\n");
    printf("11. questão 5 -Vencedores de todos os granSlam-\n");
    printf("12. questão 6 -Furadores de ranking-\n");
    printf("13. questão 7 -Retirar jogadores de pais-\n");
    printf("14. questão 8 -Top 8 por ano-\n");
    printf("15. questão 9 -Vencedores por torneio-\n");
    printf("16. SAIR\n");
}

int checkIfBackupExists(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return 1; // Backup existe
    }
    return 0; // Backup não existe
}

void inicializarBackup(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Erro ao criar o arquivo de backup.\n");
        exit(1);
    }
    fclose(file);
}

int getTanterior(const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }
    int Tanterior;
    fseek(fp, 0, SEEK_SET); // Move o ponteiro do arquivo para o início
    fread(&Tanterior, sizeof(int), 1, fp); // Lê o número de folhas
    fclose(fp);
    return Tanterior; // Retorna o número de folhas
}

int getNumeroDeJogadores(const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "rb");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }
    int numeroDeJogadores;
    fseek(fp, sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o segundo termo
    // Lê o número de jogadores a partir do segundo termo
    fread(&numeroDeJogadores, sizeof(int), 1, fp); // Lê o número de jogadores
    fclose(fp);
    return numeroDeJogadores; // Retorna o número de jogadores
}

int setTeNumeroDeJogadores(const char *nomeArquivo, int numeroDeJogadores,int T) {
    FILE *fp = fopen(nomeArquivo, "rb+");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(1);
    }
    fseek(fp, 0, SEEK_SET); // Move o ponteiro do arquivo para o segundo termo
    fwrite(&T, sizeof(int), 1, fp); // Escreve o número de jogadores
    fseek(fp, sizeof(int), SEEK_SET); // Move o ponteiro do arquivo para o segundo termo
    fwrite(&numeroDeJogadores, sizeof(int), 1, fp); // Escre
    fclose(fp);
}

void printInsereJogador(int T, int numeroDeJogadores) {
    char *nomes_torneios[15] = {"Australian Open", "Roland Garros", "Wimbledon", "US Open", "ATP Finals", "Olimpíadas", "Indian Wells", "Miami", "Monte Carlo", "Madrid", "Rome", "Canada", "Cincinnati", "Shanghai", "Paris"};
    printf("digite o nome do jogador: ");
    Tplayer player;
    scanf(" %[^\n]", player.nome); // Lê o nome do jogador
    printf("\ndigite o pais do jogador: ");
    scanf(" %[^\n]", player.nacionalidade); // Lê a nacionalidade do jogador
    printf("\ndigite o ano de nascimento do jogador: ");
    scanf("%d", &player.anoNascimento); // Lê o ano de nascimento do
    printf("\ndigite o ano de morte do jogador (0 se vivo): ");
    scanf("%d", &player.anoMorte); // Lê o ano de morte
    printf("\ndigite o ranking do jogador: ");
    scanf("%d", &player.ranking); // Lê o ranking do jogador
    printf("\ndigite o ano do melhor ranking do jogador: ");
    scanf("%d", &player.anoRanking); // Lê o ano do melhor ranking
    printf("\ndigite o numero de semanas no topo do ranking: ");
    scanf("%d", &player.semanasNoTopo); // Lê o número de
    printf("\ndigite o numero de pontos totais do jogador: ");
    scanf("%d", &player.pontosTotais); // Lê o número de pontos
    for(int i = 0; i < 15; i++) {
        printf("\ndigite o numero de titulos do torneios do tipo %s: ", nomes_torneios[i]);
        scanf("%d", &player.quantidadeTorneios[i]); // Lê o número
    }

    char nacionalidade[4];

    if(!strcmp(player.nacionalidade,"Australia")) strcpy(nacionalidade,"AUT");
    else if(!strcmp(player.nacionalidade,"Belarus")) strcpy(nacionalidade,"BLR");
    else if(!strcmp(player.nacionalidade,"Great Britain")) strcpy(nacionalidade,"GBR");
    else if(!strcmp(player.nacionalidade,"Greece")) strcpy(nacionalidade,"GRC");
    else {
        strncpy(nacionalidade,player.nacionalidade,3);
        maiusculaP(nacionalidade); // Converte nacionalidade para maiúsculas
    }

    nacionalidade[3] = '\0';
    geradorIdJogador(player.id, player.nome, player.anoNascimento,nacionalidade, numeroDeJogadores); // Gera o ID do jogador
    printf("ID gerado: %s\n", player.id); // Imprime o ID
    InsereJogador("index.bin",&player,0,0,T); // Insere um jogador na árvore B+

    THnome_insere("hash_nome.bin", "dados_nome.bin",player.nome,player.id); // Insere o jogador na tabela hash de nomes

    THativos_insere("hash_ativos.bin","dados_ativos.bin", player.id); // Insere o jogador na tabela hash de jogadores ativos

    THnacionalidade_insere("hash_nacionalidade.bin", "dados_nacionalidade.bin",player.id); // Insere o jogador na tabela hash de nacionalidade

}

int buscaAtivosPais(char *arqHash, char*arqDados, char *nacionalidade, int t) {

    int hash = hash_nacionalidade(nacionalidade), pos;

    FILE *fp = fopen(arqHash,"rb");
    if(!fp) exit(1);

    fseek(fp,hash * sizeof(int),SEEK_SET);
    fread(&pos,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen(arqDados,"rb");
    if(!fp) exit(1);

    while(pos != -1) {
        THnacionalidade aux;
        
        fseek(fp,pos,SEEK_SET);
        fread(&aux,sizeof(THnacionalidade),1,fp);

        Tplayer *jogador = buscarJogador(aux.id, 0, t);

        if ((ANO_ATUAL - jogador->anoNascimento <= 39) && (!strcmp(nacionalidade, jogador->nacionalidade))) {
            printf("nome:%s id:%s\n", jogador->nome,jogador->id); // Imprime o nome e ID do jogador ativo encontrado
        }

        free(jogador);
        pos = aux.proximo; // Move para o próximo jogador
    }
    fclose(fp);
    return 0;
}

int main(){
    int opcao=0; // Variável para armazenar a opção do usuário
    int T = 0, numeroDeJogadores = 0;
    if(checkIfBackupExists("Backup.bin")) {
        printf("Backup encontrado. Deseja mante-lo? (1-Sim, 0-Não): ");
        scanf("%d", &opcao);
        if(opcao == 1) {
            T = getTanterior("Backup.bin");
            numeroDeJogadores = getNumeroDeJogadores("Backup.bin");
            printf("Backup mantido.\n");
        } else {
            T = getTanterior("Backup.bin");
            KillArvoreB(T);
        }
    } else {
        inicializarBackup("Backup.bin");
    }
    

    if(T == 0){
        printf("Digite o valor de T (grau da árvore B+): ");
        scanf("%d", &T); // Lê o valor de T do usuário
        if(T < 2) {
            T = 2;
        }
        setTeNumeroDeJogadores("Backup.bin", numeroDeJogadores, T); // Define o número de jogadores no backup
    } else {
        printf("T anterior: %d\n", T);
    }
    //se for zero iniciar os arquivos com todos os jogadores
    if(opcao == 0){
        InicializarIndex(T);

        THnome_inicializa("hash_nome.bin", "dados_nome.bin"); // Inicializa a tabela hash de nomes

        LerEscreverJogadoresArvore("index.bin", "tennis_players.txt", "champions.txt",&numeroDeJogadores, T); // Lê e escreve jogadores na árvore B+ e na tabela hash de nomes
        
        THativos_construcao("tennis_players.txt","hash_ativos.bin","dados_ativos.bin"); // Constrói a tabela hash de jogadores ativos
        
        THcamp_ano_construcao("champions.txt","hash_campeonatos_ano.bin","dados_campeonatos_ano.bin"); // Constrói a tabela hash de campeonatos por ano
        
        THnacionalidade_construcao("tennis_players.txt", "hash_nacionalidade.bin","dados_nacionalidade.bin");
        
        THcamp_construcao("champions.txt","hash_campeonatos.bin","dados_campeonatos.bin"); // Constrói a tabela hash de campeonatos
        
        THranking_construcao("champions.txt","hash_ranking.bin","dados_ranking.bin",T); // Constrói a tabela hash de ranking
        
        THtemporada_construcao("champions.txt","hash_temporada.bin","dados_temporada.bin",T);
        
        printf("Árvore B+ e tabelas hash inicializadas com sucesso.\n");
    }
    imprimirArvoreB(T);
    printf("Número de jogadores inseridos: %d\n", numeroDeJogadores); // Imprime o número de jogadores inseridos
    
    while(opcao != 16) {
        menu(); // Exibe o menu de opções
        printf("Escolha uma opção: ");
        scanf("%d", &opcao); // Lê a opção escolhida pelo usuário
        switch(opcao) {
            char nomeBusca[50];
            Tplayer *jogador;
            char *idBusca;
            case 1:// Opção para inserir um jogador



                printInsereJogador(T, numeroDeJogadores); // Chama a função para inserir um jogador
                printf("Jogador inserido com sucesso.\n");
                numeroDeJogadores++; // Incrementa o número de jogadores
                break;

            

            case 2:// Opção para buscar um jogador



                printf("Digite o Nome completo do jogador a ser buscado: ");
                scanf(" %[^\n]", nomeBusca); // Lê o nome do jogador a ser buscado
                idBusca = THnome_busca("hash_nome.bin", "dados_nome.bin", nomeBusca);
                if (!idBusca) {
                    printf("Jogador não encontrado.\n");
                    break;
                }
                jogador = buscarJogador(idBusca,0,T); // Busca o jogador na árvore B+
                if(!jogador){
                    printf("Jogador não encontrado.\n");
                    free(idBusca); // Libera a memória alocada para o ID do jogador buscado
                } else {
                    printf("Jogador encontrado: %s, ID: %s, Nacionalidade: %s, Ano de Nascimento: %d\n", jogador->nome, jogador->id, jogador->nacionalidade, jogador->anoNascimento); // Imprime os detalhes do jogador encontrado
                    free(jogador); // Libera a memória alocada para o jogador
                    free(idBusca); // Libera a memória alocada para o ID do jogador buscado
                }
                break;



            case 3:// Opção para buscar um jogador ativo



                printf("Digite o Nome completo do jogador ativo a ser buscado: ");
                scanf(" %[^\n]", nomeBusca); // Lê o nome do jogador ativo a ser busc
                idBusca = THnome_busca("hash_nome.bin", "dados_nome.bin", nomeBusca);
                jogador = buscarJogador(idBusca,0,T); // Busca o jogador ativo na árvore
                if(!jogador){
                    printf("Jogador ativo não encontrado.\n");
                } else {
                    if((ANO_ATUAL - jogador->anoNascimento) < 39) { // Verifica se o jogador está ativo
                        printf("o jogador %s está ativo com o id %s.\n", jogador->nome,jogador->id); // Imprime o nome do jogador ativo
                    }
                    free(jogador); // Libera a memória alocada para o jogador
                }
                free(idBusca); // Libera a memória alocada para o ID do jogador buscado
                break;



            case 4:// Opção para buscar um jogador por país



                printf("Digite o país do jogador a ser buscado: ");
                scanf(" %[^\n]", nomeBusca); // Lê o país do jogador a ser buscad
                char nacionalidadeBusca[4];
                if(!strcmp(nomeBusca,"Australia"))strcpy(nacionalidadeBusca,"AUT");
                else if(!strcmp(nomeBusca,"Belarus"))strcpy(nacionalidadeBusca,"BLR");
                else if(!strcmp(nomeBusca,"Great Britain")) strcpy(nacionalidadeBusca,"GBR");
                else if(!strcmp(nomeBusca,"Greece")) strcpy(nacionalidadeBusca,"GRC");
                else {
                strncpy(nacionalidadeBusca,nomeBusca,3);
                maiuscula(nacionalidadeBusca); // Converte nacionalidade para maiúsculas
                }  
                printf("Nacionalidade buscada: %s\n", nacionalidadeBusca); // Imprime a nacionalidade buscada
                
                buscaAtivosPais("hash_nacionalidade.bin", "dados_nacionalidade.bin", nacionalidadeBusca, T); // Busca jogadores ativos por país
                break;  


            case 5:// Opção para remover um jogador



                printf("Digite o nome do jogador a ser removido: ");
                scanf(" %[^\n]", nomeBusca); // Lê o nome do jogador a ser removido
                char *idRemover;
                idRemover = THnome_busca("hash_nome.bin", "dados_nome.bin", nomeBusca);

                if(idRemover == NULL) {
                    printf("Jogador não encontrado.\n");
                    break;
                }

                printf("ID do jogador a ser removido: %s\n", idRemover);
                
                jogador = buscarJogador(idRemover, 0, T); // Busca o jogador na árvore B+
                if(!jogador) {
                    printf("Jogador não encontrado.\n");
                    free(idRemover); // Libera a memória alocada para o ID do jogador
                    break;
                }else{
                    free(jogador);
                }

                jogador = Removerjogador("index.bin", idRemover, T, 0, 0); // Remove o jogador da árvore B+

                if(jogador) {
                    printf("Jogador removido: %s\n", jogador->nome); // Imprime
                    THativos_retira("hash_ativos.bin", "dados_ativos.bin", jogador->id); // Remove o jogador da tabela hash de jogadores ativos

                    THnacionalidade_retira("hash_nacionalidade.bin", "dados_nacionalidade.bin", jogador->id); // Remove o jogador da tabela hash de nacionalidade

                    THranking_retira_id("hash_ranking.bin", "dados_ranking.bin", jogador->id); // Remove o jogador da tabela hash de ranking

                    THtemporada_retira_id("hash_temporada.bin", "dados_temporada.bin", jogador->id); // Remove o jogador da tabela hash de temporada

                    THcamp_remove("hash_campeonatos.bin", "dados_campeonatos.bin", jogador->id); // Remove o jogador da tabela hash de campeonatos

                    THcamp_ano_remove("hash_campeonatos_ano.bin", "dados_campeonatos_ano.bin", jogador->id); // Remove o jogador da tabela hash de campeonatos por ano

                    THnome_retira("hash_nome.bin", "dados_nome.bin",jogador->nome); // Remove o jogador da tabela hash de nomes
                
                    free(jogador); // Libera a memória alocada para o jogador removido
                    numeroDeJogadores--; // Decrementa o número de jogadores
                }else{
                    printf("Jogador não encontrado na árvore B+. ERRO\n");
                }
                free(idRemover); // Libera a memória alocada para o ID do jogador removido
                break;



            case 6:// Opção para imprimir as árvores B+ e tabelas hash



                imprimirArvoreB(T); // Imprime a árvore B+
                printf("Número de jogadores: %d\n", numeroDeJogadores); // Imprime o número de jogadores
                break;



            case 7:



                printf("Questão 1 - Títulos de ativos e aposentados:\n");
                Questao1(T); // Chama a função para exibir títulos de ativos e aposentados
                break;



            case 8:



                // Questão 2 - Compatriota de quem ganhou Grand Slam
                printf("Questão 2 - Compatriota de quem ganhou Grand Slam:\n");
                Questao2(T); // Chama a função para exibir compatriotas de quem ganhou Grand Slam
                break;



            case 9:



                // Questão 3 - Pontuação acumulada
                printf("Questão 3 - Pontuação acumulada:\n");
                Questao3(T); // Chama a função para exibir a pontuação acumulada
                break;



            case 10:



                // Questão 4 - Top 25 por ano
                printf("Questão 4 - Top 25 por ano:\n");
                Questao4(T); // Chama a função para exibir o top 25 por ano
                break;



            case 11:



                // Questão 5 - Vencedores de todos os Grand Slam
                printf("Questão 5 - Vencedores de todos os Grand Slam:\n");
                Questao5(T); // Chama a função para exibir os vencedores de todos os Grand Slam
                break;



            case 12:



                // Questão 6 - Furodores de ranking
                printf("Questão 6 - Furadores de ranking:\n");
                Questao6(T); // Chama a função para exibir os furodores de ranking
                break;



            case 13:



                // Questão 7 - Retirar jogadores de países
                printf("Questão 7 - Retirar jogadores de países:\n");
                Questao7(T,&numeroDeJogadores); // Chama a função para retirar jogadores de países
                break;



            case 14:



                // Questão 8 - Top 8 por ano
                printf("Questão 8 - Top 8 por ano:\n");
                Questao8(T); // Chama a função para exibir o top 8 por ano
                break;



            case 15:



                // Questão 9 - Vencedores por torneio
                printf("Questão 9 - Vencedores por torneio:\n");
                Questao9(T); // Chama a função para exibir os vencedores por torneio
                break;
            case 16:



                // Opção para sair do programa
                setTeNumeroDeJogadores("Backup.bin", numeroDeJogadores, T); // Atualiza o número de jogadores no backup
                printf("Backup atualizado com sucesso.\n");
                printf("Número de jogadores: %d\n", numeroDeJogadores); // Imprime
                printf("T anterior: %d\n", T); // Imprime o valor de T
                printf("Backup atualizado com sucesso.\n");
                printf("Saindo do programa...\n");
                break;



            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    }

    return 0;
}