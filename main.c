#include "TABM.h"
#include "TABMaux.h"
#include "Players.h"
#include "THMSnome.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int numeroDeJogadores = 0;
    int T = 10;
    //KillArvoreB(T);
    InicializarIndex(T);
    THnome_inicializa("hash_nome.bin", "dados_nome.bin"); // Inicializa a tabela hash de nomes
    LerEscreverJogadoresArvore("index.bin", "tennis_players.txt", "champions.txt",&numeroDeJogadores, T); // Lê e escreve jogadores na árvore B+ e na tabela hash de nomes
    //Inicializa o índiceions
    //KillArvoreB();
    // printf("Inicializando o índice...\n");
    
    // // //Exemplo de inserção de jogadores
    // Tplayer jogador1 = {"12", "Jogador 1"};
    // Tplayer jogador2 = {"21", "Jogador 2"};
    // Tplayer jogador3 = {"37", "Jogador 3"};
    // Tplayer jogador4 = {"48", "Jogador 4"};
    // Tplayer jogador5 = {"56", "Jogador 5"};
    // Tplayer jogador6 = {"68", "Jogador 6"};
    // Tplayer jogador7 = {"79", "Jogador 7"};
    // Tplayer jogador8 = {"85", "Jogador 8"};
    // Tplayer jogador9 = {"97", "Jogador 9"};
    // Tplayer jogador10 = {"99", "Jogador 10"};
    // Tplayer jogado11 = {"999", "Jogador 11"};
    // Tplayer jogador12 = {"99998", "Jogador 12"};
    // Tplayer jogador13 = {"99998754", "Jogador 12"};
    // Tplayer jogador14 = {"999981231", "Jogador 12"};
    // Tplayer jogador15 = {"9999111", "Jogador 12"};
    // Tplayer jogador16 = {"99998781", "Jogador 12"};
    // Tplayer jogador17 = {"99998782", "Jogador 12"};
    // Tplayer jogador18 = {"99998783", "Jogador 12"};
    // Tplayer jogador19 = {"99998784", "Jogador 12"};
    // Tplayer jogador20 = {"99998785", "Jogador 12"};
    // Tplayer jogador21 = {"99998786", "Jogador 12"};
    // Tplayer jogador22 = {"99998787", "Jogador 12"};
    // // printf("Inserindo jogadores...\n");
    
    // InsereJogador("index.bin", &jogador1, 0,0);
    // InsereJogador("index.bin", &jogador2, 0,0);
    // InsereJogador("index.bin",&jogador3,0,0);
    // InsereJogador("index.bin",&jogador4,0,0);
    // InsereJogador("index.bin",&jogador5,0,0);
    // InsereJogador("index.bin",&jogador6,0,0);
    // InsereJogador("index.bin",&jogador7,0,0);
    // InsereJogador("index.bin",&jogador8,0,0);
    // InsereJogador("index.bin",&jogador9,0,0);
    // InsereJogador("index.bin",&jogador10,0,0);
    // InsereJogador("index.bin",&jogado11,0,0);
    // InsereJogador("index.bin",&jogador12,0,0);
    // InsereJogador("index.bin",&jogador13,0,0);
    // InsereJogador("index.bin",&jogador14,0,0);
    // InsereJogador("index.bin",&jogador15,0,0);
    // InsereJogador("index.bin",&jogador16,0,0);
    // InsereJogador("index.bin",&jogador17,0,0);
    // InsereJogador("index.bin",&jogador18,0,0);
    // InsereJogador("index.bin",&jogador19,0,0);
    // InsereJogador("index.bin",&jogador20,0,0);
    // InsereJogador("index.bin",&jogador21,0,0);
    // InsereJogador("index.bin",&jogador22,0,0);


    // printf("\n\n\n\n\n");
    printIndexTotal("index.bin",T);
    imprimirArvoreB(T);
    //THnome_imprime("hash_nome.bin", "dados_nome.bin", TAM_HASH); // Imprime a tabela hash de nomes
    printf("Número total de jogadores inseridos: %d\n", numeroDeJogadores);
    // //Exemplo de busca de jogadores
    // Tplayer *buscado1 = buscarJogador("2", 0);
    // if (buscado1) {
    //     printf("Jogador encontrado: %s\n", buscado1->nome);
    //     free(buscado1);
    // } else {
    //     printf("Jogador não encontrado.\n");
    // }

        

    return 0;
}