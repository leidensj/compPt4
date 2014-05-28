#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "astree.h"
#include "lex.yy.h"
#include "y.tab.h"

int yylex();
extern FILE * yyin;
extern int running;
extern int getLineNumber(void);
extern int getErrorNumber();

int main(int argv, char **argc)
{
    initMe();

    if (argv < 3) 
    {
        printf("Digite o nome do arquivo de entrada e arquivo de saida!\n");
        exit(1);
    }
    
    yyin = fopen(argc[1],"r");
    astGetOutputFileName(argc[2]);
    printf("╔═══════════════════════════════════════════════════════════════════════\n");
    printf("║ETAPA 4 - ANALISE SEMANTICA\n");
    printf("║Joao Felipe Leidens e Gennaro Rodrigues\n");
    printf("║Arquivo de entrada: %s\n", argc[1]);
    printf("║Arquivo de saida: %s\n", argc[2]);
    printf("║Nota ao professor: quando ha comentarios, em alguns casos \n");
    printf("║o erro e mencionado na linha errada. Por exemplo: Se\n");
    printf("║1║ a = v \n");
    printf("║2║ //comment \n");
    printf("║3║ //comment \n");
    printf("║4║ //comment \n");
    printf("║5║ //comment \n");
    printf("║O erro e apontado na linha 5 e nao na linha 2.\n");
    printf("║Esse foi o unico problema encontrado\n");
    printf("╚═══════════════════════════════════════════════════════════════════════\n");

    yyparse();
    int error = getErrorNumber();
    //hashPrint();
    if(error == 4)
    {
        printf("╔═══════════════════════════════════════════════════════════════════════\n");
        printf("║ERRO de semantica: Saindo com erro 4\n");
        printf("╚═══════════════════════════════════════════════════════════════════════\n");
        
        exit(4);
    }
        
    printf("╔═══════════════════════════════════════════════════════════════════════\n");
    printf("║Nenhum erro encontrado\n");
    printf("╚═══════════════════════════════════════════════════════════════════════\n");
    /*printf("╔═╗╦ ╦╔═╗╔═╗╔═╗╔═╗╔═╗\n");
    printf("╚═╗║ ║║  ║╣ ╚═╗╚═╗║ ║\n");
    printf("╚═╝╚═╝╚═╝╚═╝╚═╝╚═╝╚═╝\n");
    printf("Analise sintatica feita com sucesso. Nenhum erro encontrado.\n");
    printf("Numero de linhas do arquivo de entrada: %d\n", getLineNumber());*/
    exit(0);
}
