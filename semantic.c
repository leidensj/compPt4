#include "semantic.h"
#include <stdio.h>

int guardaTypeRet=0;
ASTREE *funcAst;
int scalDecl=0;

int getErrorNumber()
{
	return erro;
}
void checkDeclaration(ASTREE *root)
{
	int i;

	if (root == 0)
		return;		

	if((root->type == AST_DECL_VECTOR) || 
	   (root->type == AST_DECL_VECTOR_INITVALUE) || 
	   (root->type == AST_DECL) || 
	   (root->type == AST_DECL_POINTER) || 
	   (root->type == AST_FUNCTION_DECL))
	{
		if(root->symbol == 0)
		{
			printf("ERRO - Identificador da declaracao esta faltando!!\n");
			erro=4;
		}
		else
		{
			if(root->symbol->content.type != SYMBOL_TK_IDENTIFIER)
			{
				printf("ERRO - Linha %d: Simbolo %s ja definido.\n", root->lineNumber, root->symbol->content.text);
				erro=4;
			}
			else
			{
				if(root->type == AST_FUNCTION_DECL)
				{
					root->symbol->content.type = SYMBOL_FUNCTION;
					if(root->son[1])
					{
						root->symbol->content.ast = (ASTREE*)malloc(sizeof(ASTREE));
						root->symbol->content.ast = root->son[1];
					}
				} 
				else 
				{
					if (root->type == AST_DECL || root->type == AST_DECL_POINTER)
						root->symbol->content.type = SYMBOL_VARIABLE;
					else
						if(root->type == AST_DECL_VECTOR || root->type == AST_DECL_VECTOR_INITVALUE)
							root->symbol->content.type = SYMBOL_VECTOR;
				}
			}
		}
	}
	for(i=0; i < MAX_SONS; i++)
		checkDeclaration(root->son[i]);
}


void checkFuncCall(ASTREE *ast, char *text)
{
	int i;

	if(ast==0)
		return;

	if((ast->symbol!=0) && (ast->type == AST_FUNCTION_DECL))
		if(strcmp(ast->symbol->content.text, text) == 0)
		{
			funcAst = ast;
			return;
		}
	for(i=0; i < MAX_SONS; i++)
		checkFuncCall(ast->son[i], text);
}

void checkUtilization(ASTREE *root, ASTREE *rootAux)
{
	int i, result, verRet=0, operIgual=0;

	if (root == 0)
		return;
	if((root->symbol != 0) && (root->type == AST_FUNCTION_DECL))
		guardaTypeRet = root->symbol->content.dataType;
	switch(root->type){
		case AST_KW_INPUT:
			if(root->symbol != 0)
				if (root->symbol->content.type != SYMBOL_VARIABLE)
				{
	       			printf("ERRO - Linha %d: Declaracao da variavel em INPUT faltante!\n", root->lineNumber);
					erro=4;
				}
			break;
		case AST_EXPR_W_TKIDENTIFIER:
			if(root->symbol->content.type != SYMBOL_TK_IDENTIFIER)
			{
				if(root->symbol->content.type == SYMBOL_FUNCTION)
				{
					funcAst = 0;
					checkFuncCall(rootAux, root->symbol->content.text);
					result = checkParamsFunc(funcAst->symbol->content.ast, root);
					if(result == 1)
					{
						printf("ERRO - Linha %d: Funcao %s contem MAIS parametros do que o numero declarado!\n", root->lineNumber, root->symbol->content.text);
						erro=4;
					}
					else
						if(result == 2)
						{
							printf("ERRO -  Linha %d: Funcao %s contem MENOS parametros do que o numero declarado!\n", root->lineNumber, root->symbol->content.text);
							erro=4;
						}
						else
							if(result == 3)
							{
								printf("ERRO - Linha %d: Parametros da funcao %s tem tipos diferentes ou declaracoes faltantes!\n", root->lineNumber, root->symbol->content.text);
								erro=4;
							}
				}
			}
			break;
		case AST_RETURN:
			if(root->son[0]->type == AST_EXPR_W_TKIDENTIFIER)
			{
				printf("ERRO - Linha %d: Retornar uma funcao nao e possivel!\n", root->lineNumber);
				erro=4;
				break;
			}
			if(root->son[0]->type == AST_TK_IDENTIFIER_VET)
			{
				printf("ERRO - Linha %d: Retornar um vetor nao e possivel!\n", root->lineNumber);
				erro=4;
			}
			else
			{
				verRet = analisaExpressao(root->son[0], rootAux);
				if(guardaTypeRet != verRet)
				{
					switch(guardaTypeRet)
					{
						case DATATYPE_KW_WORD:
							if(verRet == DATATYPE_KW_BYTE)
								printf("AVISO - Linha %d: Retornando BYTE ao inves de WORD!\n", root->lineNumber);		
							else if (verRet == DATATYPE_KW_BOOL)
							{
								printf("ERRO - Linha %d: Retornando BOOL ao inves de WORD!\n", root->lineNumber);
								erro=4;
							}
							else
							{
								printf("ERRO - Linha %d: Retornando tipo errado!\n", root->lineNumber);
								erro=4;	
							}	
							break;
						case DATATYPE_KW_BYTE:
							if(verRet == DATATYPE_KW_WORD)
								printf("AVISO - Linha %d: Retornando WORD ao inves de BYTE! (Perdas possiveis)!\n", root->lineNumber);		
							else if (verRet == DATATYPE_KW_BOOL)
							{
								printf("ERRO - Linha %d: Retornando BOOL ao inves de BYTE!\n", root->lineNumber);
								erro=4;
							}
							else
							{
								printf("ERRO - Linha %d: Retornando tipo errado!\n", root->lineNumber);
								erro=4;	
							}
							break;
						case DATATYPE_KW_BOOL:
							if(verRet == DATATYPE_KW_WORD)
							{
								printf("ERRO - Linha %d: Retornando WORD ao inves de BOOL!\n", root->lineNumber);
								erro=4;	
							}

							else if (verRet == DATATYPE_KW_BYTE)
							{
								printf("ERRO - Linha %d: Retornando BYTE ao inves de BOOL!\n", root->lineNumber);
								erro=4;
							}
							else
							{
								printf("ERRO - Linha %d: Retornando tipo errado!\n", root->lineNumber);
								erro=4;	
							}
							break;
					}
				}
			}
			break;
		case AST_KW_IF:
			if(root->son[0]->son[0] == 0)
			{
				if(root->son[0]->type != AST_SYMBOL && root->son[0]->type != AST_LIT_TRUE && root->son[0]->type != AST_LIT_FALSE)
				{
					printf("ERRO - Linha %d: Expressao IF contem somente um literal!\n", root->lineNumber);
					erro=4;
				}
			}
			else
			{
				if((root->son[0]->son[1] == 0) && (root->son[0]->symbol != 0))
				{
					if(root->son[0]->type == AST_EXPR_W_TKIDENTIFIER)
					{
						printf("ERRO - Linha %d: Funcao sendo chamada dentro de expressao IF!\n", root->lineNumber);
						erro=4;
					}
				}
				else
				{
					if((root->son[0]->son[1] != 0) && (root->son[0]->symbol == 0))						
						if(root->son[0]->type == (AST_ADD || AST_SUB || AST_MUL || AST_DIV))
						{
							printf("ERRO - Linha %d: Expressao aritmetica dentro de expressao IF!\n", root->lineNumber);
							erro=4;
						}
				}
			}
			break;
		case AST_KW_IF_ELSE:
			if(root->son[0]->son[0] == 0)
			{
				if(root->son[0]->type != AST_SYMBOL && root->son[0]->type != AST_LIT_TRUE && root->son[0]->type != AST_LIT_FALSE)
				{
					printf("ERRO - Line %d: Expressao IF contem somente um literal!\n", root->lineNumber);
					erro=4;
				}
			}
			else
			{
				if((root->son[0]->son[1] == 0) && (root->son[0]->symbol != 0))
				{
					if(root->son[0]->type == AST_EXPR_W_TKIDENTIFIER)
					{
						printf("ERRO - Linha %d: Funcao sendo chamada dentro de expressao IF!\n", root->lineNumber);
						erro=4;
					}
				}
				else
				{
					if((root->son[0]->son[1] != 0) && (root->son[0]->symbol == 0))						
						if(root->son[0]->type == (AST_ADD || AST_SUB || AST_MUL || AST_DIV))
						{
							printf("ERRO - Linha %d: Expressao aritmetica dentro de expressao IF!\n", root->lineNumber);
							erro=4;
						}
				}
			}
			break;
		case AST_KW_LOOP:
			if(root->son[1]->son[0] != 0)
			{
				if((root->son[1]->son[1] == 0) && (root->son[1]->symbol != 0))
				{
					if(root->son[1]->type == AST_EXPR_W_TKIDENTIFIER)
					{
						printf("ERRO - Linha %d: Funcao sendo chamada dentro de expressao LOOP!\n", root->lineNumber);
						erro=4;
					}
				}
				else
				{
					if((root->son[1]->son[1] != 0) && (root->son[1]->symbol == 0))						
						if(root->son[1]->type == (AST_ADD || AST_SUB || AST_MUL || AST_DIV))
						{
							printf("ERRO - Line %d: Expressao aritmetica dentro de expressao LOOP!\n", root->lineNumber);
							erro=4;
						}
				}
			}
			break;
		/*case AST_EQUALS:
			funcAst = 0;
			scalDecl = 0;
			checkVarDeclAsFunc(rootAux, root->son[0]->symbol->content.text);
			if(scalDecl == 1)
			{
				if(funcAst->symbol->content.type == SYMBOL_FUNCTION)
				{
					printf("ERRO - Linha %d: Variavel %s tem o mesmo nome de uma funcao!\n", root->lineNumber, root->son[0]->symbol->content.text);
					erro=4;
				}
			}
			if(root->son[0]->type == AST_SYMBOL)
			{
				if(root->son[0]->symbol->content.dataType != 0)
				{
					operIgual = analisaExpressao(root->son[1], rootAux);
					if(operIgual != 0)
					{
						if(root->son[0]->symbol->content.dataType != operIgual)
						{
							if(root->son[1]->type == AST_EXPR_W_TKIDENTIFIER)
								if(root->son[1]->symbol->content.type != SYMBOL_FUNCTION)
								{
									printf("ERRO - Linha %d: Variavel %s nao e declarada como funcao!\n", root->lineNumber, root->son[1]->symbol->content.text);
									erro=4;
								}
							switch(root->son[0]->symbol->content.dataType)
							{
								case DATATYPE_KW_BYTE:
									if(operIgual == DATATYPE_KW_WORD)
										printf("AVISO - Linha %d: Perda de precisao. Atribuindo WORD a BYTE!\n", root->lineNumber);
									else
									{
										printf("ERRO - Linha %d: Variaveis contendo tipos diferentes!\n", root->lineNumber);
										erro=4;
									}
									break;
								case DATATYPE_KW_WORD:
									if(operIgual == DATATYPE_KW_BOOL)
									{
										printf("ERRO - Linha %d: Variaveis contendo tipos diferentes!\n", root->lineNumber);
										erro=4;
									}
									break;
								case DATATYPE_KW_BOOL:
									printf("ERRO - Linha %d: Variaveis contendo tipos diferentes!!\n", root->lineNumber);
									erro=4;
									break;
							}
						}
						else
							if(root->son[1]->type == AST_EXPR_W_TKIDENTIFIER)
								if(root->son[1]->symbol->content.type != SYMBOL_FUNCTION)
								{
									printf("ERRO - Linha %d: Variavel %s nao e declarada como funcao!\n", root->lineNumber, root->son[1]->symbol->content.text);
									erro=4;
								}
					}
				}
			}
			else
			{
				if(root->son[0]->symbol->content.dataType != 0)
				{
					int switchTest = root->son[0]->son[0]->type;
					if(switchTest == AST_ADD || switchTest == AST_SUB || switchTest == AST_MUL || switchTest == AST_DIV)
						switchTest = AST_ADD;
					switch(root->son[0]->son[0]->type)
					{
						case AST_LIT_INTEGER:
							break;
						case AST_SYMBOL:
							if(root->son[0]->son[0]->symbol->content.dataType != 1)
							{
								if(root->son[0]->son[0]->symbol->content.dataType != 3)
								{
									printf("WARNING - Line %d: character variable into the vector!!!\n", root->lineNumber);
								}
								else
								{
									printf("ERRO - Line %d: Vector %s is being used with wrong parameters inside it. [ NOT VALID 1] !!!\n", root->lineNumber, root->son[0]->symbol->content.text);
									erro=1;
								}
							}
							break;
						case AST_LIT_CHAR:
							printf("WARNING - Line %d: character variable into the vector!!!\n", root->lineNumber);
							break;
						case AST_ADD:
							if(root->son[0]->son[0]->son[0]->symbol->content.dataType != root->son[0]->son[0]->son[1]->symbol->content.dataType)
							{
								if((root->son[0]->son[0]->son[0]->symbol->content.dataType > 1 ) && (root->son[0]->son[0]->son[0]->symbol->content.dataType != 3))
								{
									printf("ERRO - Line %d: Vector %s is being used with wrong parameters inside it. [ NOT VALID 2] !!! ROOT DATA = %d\n", root->lineNumber, root->son[0]->symbol->content.text, root->son[0]->son[0]->son[0]->symbol->content.dataType);
									erro=1;
								}
								else
								{
									if((root->son[0]->son[0]->son[1]->symbol->content.dataType > 1) && (root->son[0]->son[0]->son[1]->symbol->content.dataType != 3))
									{
										printf("ERRO - Line %d: Vector %s is being used with wrong parameters inside it. [ NOT VALID 3] !!!\n", root->lineNumber, root->son[0]->symbol->content.text);
										erro=1;
									}
									else
									{
										if(root->son[0]->son[0]->son[0]->symbol->content.dataType == 1)
										{
											if(root->son[0]->son[0]->son[1]->symbol->content.dataType != 1)
												printf("WARNING - Line %d: character variable in the operation into the vector!!!\n", root->lineNumber);
										}
										else
											printf("WARNING - Line %d: character variable in the operation into the vector!!!\n", root->lineNumber);
										}
									}
								}
								else
								{
									if((root->son[0]->son[0]->son[0]->symbol->content.dataType != 1) && (root->son[0]->son[0]->son[0]->symbol->content.dataType != 3))
									{
										printf("ERRO - Line %d: Vector %s is being used with wrong parameters inside it. [ NOT VALID 4] !!!\n", root->lineNumber, root->son[0]->symbol->content.text);
										erro=1;
									}
									else
										if(root->son[0]->son[0]->son[0]->symbol->content.dataType == 3)
											printf("WARNING - Line %d: character variable in the operation into the vector!!!\n", root->lineNumber, root->son[0]->symbol->content.text);
								}
								break;
							default:
								printf("ERRO - Line %d: Vector %s is being used with wrong parameters inside it. [ NOT VALID 5] !!!\n", root->lineNumber, root->son[0]->symbol->content.text);
								erro=1;
								break;
					}
					operIgual = analisaExpressao(root->son[0]->son[0], rootAux);
					if(operIgual != 0)
					{
						if(operIgual != DATATYPE_KW_WORD)
						{
							if(operIgual == DATATYPE_KW_WORD)
								printf("WARNING - Line %d: Loss of precision - float in vector!!!\n", root->lineNumber);
							else
							{
								printf("ERRO - Line %d: Vector containing wrong parameters!!!\n", root->lineNumber);
								erro=4;
							}
						}
						operIgual = analisaExpressao(root->son[1], rootAux);
						if(root->son[0]->symbol->content.dataType != operIgual)
						{
							if(root->son[1]->type == AST_EXPR_W_TKIDENTIFIER)
								if(root->son[1]->symbol->content.type != SYMBOL_FUNCTION)
								{
									printf("ERRO - Line %d: Variable %s is not declared as a function, but it is being used as one!!!\n", root->lineNumber, root->son[1]->symbol->content.text);
									erro=4;
								}

							switch(root->son[0]->symbol->content.dataType)
							{
								case DATATYPE_KW_BYTE:
									if(operIgual == DATATYPE_KW_WORD)
										printf("WARNING - Line %d: Loss of precision - different types, putting float into integer!!!\n", root->lineNumber);
									else
									{
										printf("ERRO - Line %d: Variables containing different types!!!\n", root->lineNumber);
										erro=4;
									}
									break;
								case DATATYPE_KW_WORD:
									if(operIgual == DATATYPE_KW_BOOL)
									{
										printf("ERRO - Line %d: Variables containing different types!!!\n", root->lineNumber);
										erro=4;
									}
									break;
								case DATATYPE_KW_BOOL:
									printf("ERRO - Line %d: Variable does not match!!!\n", root->lineNumber);
									erro=4;
									break;
							}
						}
						else
							if(root->son[1]->type == AST_EXPR_W_TKIDENTIFIER)
								if(root->son[1]->symbol->content.type != SYMBOL_FUNCTION)
								{
									printf("ERRO - Line %d: Variable %s is not declared as a function, but it is being used as one!!!\n", root->lineNumber, root->son[1]->symbol->content.text);
									erro=4;
								}
					}
				}
			}
			break;*/
		default:
			break;
	}
	for(i=0; i < MAX_SONS; i++)
		checkUtilization(root->son[i], rootAux);
}

int checkOperationTypes(int expr1, int expr2)
{
	if((expr1 != 0) && (expr2 != 0))
	{
		if(expr1 != expr2)
		{
			if((expr1 == DATATYPE_KW_BOOL) || (expr2 == DATATYPE_KW_BOOL))
				return 1;
			if((expr1 == DATATYPE_KW_WORD) || (expr2 == DATATYPE_KW_WORD))
			{
				if((expr1 == DATATYPE_KW_BYTE) || (expr2 == DATATYPE_KW_BYTE))
					return 2;
			}
			else
			{
				if((expr1 == DATATYPE_KW_WORD) || (expr2 == DATATYPE_KW_WORD))
					return 2;
			}
		}
		else
			return 3;
	}
	else
		return 0;
}

int checkResultOperTypes(int verified, int expr1, int expr2, int lineNumber)
{
	int help = 0;

	if(verified == 0)
	{
		printf("ERRO - Linha %d: Argumentos para o operando faltando!\n", lineNumber);
		erro=4;
		help = 0;
	}
	else
	{
		if(verified == 1)
		{
			printf("ERRO - Linha %d: variavel booleana na expressao!\n", lineNumber);
			erro=4;
		}
		else
		{
			if(verified == 2)
			{
				printf("AVISO - Linha %d: operacao de WORD com BYTE!\n", lineNumber);
			}
		}
		if(expr1 >= expr2)
			help = expr1;
		else
			help = expr2;
	}
	return help;
}

void checkVarDeclAsFunc(ASTREE *ast, char *text)
{
	int i;

	if(ast==0)
		return;

	if((ast->symbol!=0) && (ast->type == AST_FUNCTION_DECL))
		if(strcmp(ast->symbol->content.text, text) == 0)
		{
			funcAst = ast;
			scalDecl = 1;
			return;
		}
	for(i=0; i < MAX_SONS; i++)
		checkVarDeclAsFunc(ast->son[i], text);
}

int analisaExpressao(ASTREE *ast, ASTREE *rootAux)
{
	int help, verified, expr1, expr2;
	
	if(ast != 0)
	{
		switch(ast->type)
		{
			case AST_SYMBOL:
				funcAst = 0;
				scalDecl = 0;
				checkVarDeclAsFunc(rootAux, ast->symbol->content.text);
				if(scalDecl == 1)
				{
					if(funcAst->symbol->content.type == SYMBOL_FUNCTION)
					{
						printf("ERRO - Linha %d: Variavel %s usa o mesmo nome de uma funcao!\n", ast->lineNumber, ast->symbol->content.text);
						erro=4;
					}
					else
					{
						printf("ERRO - Linha %d: Variavel %s is declared as a vector!!!\n", ast->lineNumber, ast->symbol->content.text);
						erro=4;
					}
				}
				help = ast->symbol->content.dataType;
				break;
			case AST_LIT_INTEGER:
				help = DATATYPE_KW_WORD;
				break;
			case AST_LIT_TRUE:
				help = DATATYPE_KW_BOOL;
				break;
			case AST_LIT_FALSE:
				help = DATATYPE_KW_BOOL;
				break;
			case AST_LIT_CHAR:
				help = DATATYPE_KW_WORD;
				break;
			case AST_EXPR_W_TKIDENTIFIER:
				if(ast->symbol->content.dataType == 0)
					help = 0;
				else
					help = ast->symbol->content.dataType;
				break;
			case AST_EXPR_W_BRACKETS:
				help = analisaExpressao(ast->son[0], rootAux);
				break;
			case AST_ADD:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_SUB:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_MUL:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_DIV:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_NOT:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				if(expr1 != 0)
					help = expr1;
				else
					help = 0;
				break;
			case AST_OPERATOR_AND:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_OPERATOR_OR:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_OPERATOR_LE:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_OPERATOR_GE:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_OPERATOR_EQ:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_OPERATOR_NE:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_HIGHER:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_LOWER:
				expr1 = analisaExpressao(ast->son[0], rootAux);
				expr2 = analisaExpressao(ast->son[1], rootAux);
				verified = checkOperationTypes(expr1, expr2);
				help = checkResultOperTypes(verified, expr1, expr2, ast->lineNumber);
				break;
			case AST_TK_IDENTIFIER_VET:
				funcAst = 0;
				scalDecl = 0;
				checkVarDeclAsFunc(rootAux, ast->symbol->content.text);
				if(scalDecl == 1)
				{
					if(funcAst->symbol->content.type == SYMBOL_FUNCTION)
					{
						printf("ERRO - Linha %d: Variavel %s tem o mesmo nome de uma funcao!\n", ast->lineNumber, ast->symbol->content.text);
						erro=4;
					}
				}
				if(ast->symbol->content.dataType != 0)
				{
					help = ast->symbol->content.dataType;
				}
				else
					help = 0;
				break;
			default:
				help = 0;
				break;			
		}
	}
	return help;
}

int checkParamsFuncAux(ASTREE *ast, ASTREE *node)
{
	if(node!=0)
	{
		switch(node->type)
		{
			case AST_SYMBOL:
				if(node->symbol->content.dataType == ast->symbol->content.dataType)
					return 0;
				else
					return 3;
				break;
			case AST_USING_PARAMETER_CONST:
				switch(node->son[0]->type)
				{
					case AST_LIT_INTEGER:
						if(ast->symbol->content.dataType == DATATYPE_KW_WORD || ast->symbol->content.dataType == DATATYPE_KW_BYTE)
							return 0;
						else
							return 3;
						break;
					case AST_LIT_CHAR:
						if(ast->symbol->content.dataType == DATATYPE_KW_WORD || ast->symbol->content.dataType == DATATYPE_KW_BYTE)
							return 0;
						else
							return 3;
						break;
					case AST_LIT_TRUE:
						if(ast->symbol->content.dataType == DATATYPE_KW_BOOL)
							return 0;
						else
							return 3;
						break;
					case AST_LIT_FALSE:
						if(ast->symbol->content.dataType == DATATYPE_KW_BOOL)
							return 0;
						else
							return 3;
						break;
					default:
						break;
				}
				break;
		}	
	}
}
int checkParamsFunc(ASTREE *ast, ASTREE *son)
{
	int cont = 0;
	ASTREE *nodeSon = 0, *nodeSonAux = 0;

	nodeSon = son->son[0];
	int iterationNumber = 0;
	do
	{
		if(ast==0)
		{
			if(nodeSon==0)
				return 0;
			else
				return 1;
		}
		else
		{
			if(nodeSon==0)
				return 2;
			if(nodeSon != 0)
			{
				if(nodeSon->son[0]!=0)
				{
					if(nodeSon->son[0]->type==AST_USING_PRIMARY_PARAMETER)
					{
						if(checkParamsFuncAux(ast, nodeSon->son[0]->son[0]) == 3)
							return 3;
						nodeSon = nodeSon->son[1];
					}
					else
					{
						if(checkParamsFuncAux(ast, nodeSon) == 3)
							return 3;
						nodeSon = nodeSon->son[1];
					}
				}
			}
			ast = ast->son[1];
		}
	} while(1);
}

void checkUndeclared()
{
	int address, result;
	HASH_NODE *node;

	for(address=1;address < HASH_SIZE; address++)
		if(Table[address])
			for(node=Table[address]; node!=0; node=node->next)
				if(node->content.type == SYMBOL_TK_IDENTIFIER)
				{
						printf("ERRO - Linha %d: Expressao %s com declaracao faltante!\n", node->content.lineNumber, node->content.text);
						erro=4;
				}
}