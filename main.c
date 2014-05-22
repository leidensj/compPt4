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
void printTokens()
{
	int token = 0;
	token = yylex();
	switch(token)
	{
		case KW_WORD:					printf ("palavra reservada WORD \n"); break;
		case KW_BOOL:					printf ("palavra reservada BOOL \n"); break;
		case KW_BYTE:					printf ("palavra reservada BYTE \n"); break;
	     case KW_IF:					printf ("palavra reservada IF \n"); break;
		case KW_THEN:					printf ("palavra reservada THEN \n"); break;
		case KW_ELSE:					printf ("palavra reservada ELSE \n"); break;
		case KW_LOOP:					printf ("palavra reservada LOOP \n"); break;
		case KW_INPUT:					printf ("palavra reservada INPUT \n"); break;
		case KW_RETURN:				printf ("palavra reservada RETURN \n"); break;
		case KW_OUTPUT:				printf ("palavra reservada OUTPUT \n"); break;
		case OPERATOR_LE:				printf ("operador LE \n"); break;
		case OPERATOR_GE:				printf ("operador GE \n"); break;
		case OPERATOR_EQ:				printf ("operador EQ \n"); break;
		case OPERATOR_NE:				printf ("operador NE \n"); break;
		case OPERATOR_AND:				printf ("operador AND \n"); break;
		case OPERATOR_OR:				printf ("operador OR \n"); break;
		case TK_IDENTIFIER:				printf ("IDENTIFICADOR \n"); break;
		case LIT_INTEGER:				printf ("INTEIRO \n"); break;
		case LIT_FALSE:				printf ("FALSE \n"); break;
		case LIT_TRUE:					printf ("TRUE \n"); break;
		case LIT_CHAR:					printf ("CHAR \n"); break;
		case LIT_STRING:				printf ("STRING \n"); break;
		case TOKEN_ERROR:				printf ("ERROR \n"); break;
		default:						printf ("DEFAULT  %c \n", token);
	}
}

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
    printf("Arquivo de entrada: %s\n", argc[1]);
    printf("Arquivo de saida: %s\n", argc[2]);

	//yydebug = 1;
    yyparse();
    //printTokens();
    int error = getErrorNumber();
    if(error == 4)
    {
        printf("\nErro de semantica: Saindo com erro 4\n");
        exit(4);
    }
        
    
    /*printf("╔═╗╦ ╦╔═╗╔═╗╔═╗╔═╗╔═╗\n");
    printf("╚═╗║ ║║  ║╣ ╚═╗╚═╗║ ║\n");
    printf("╚═╝╚═╝╚═╝╚═╝╚═╝╚═╝╚═╝\n");
    printf("Analise sintatica feita com sucesso. Nenhum erro encontrado.\n");
    printf("Numero de linhas do arquivo de entrada: %d\n", getLineNumber());
    printf("TABELA HASH\n-----------\n");
    hashPrint();*/
    exit(0);
}
