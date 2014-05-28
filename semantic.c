#include "semantic.h"
#include <stdio.h>

#define CHECKPARAMFUNC_OK	  0
#define CHECKPARAMFUNC_ERROR1 1
#define CHECKPARAMFUNC_ERROR2 2
#define CHECKPARAMFUNC_ERROR3 3
#define CHECKPARAMFUNC_ERROR4 4
#define CHECKPARAMFUNC_ERROR5 5
#define CHECKPARAMFUNC_ERROR6 6

#define RET_UNKNOWN 		  0
#define RET_WORD 			  1
#define RET_WORD_PTR          2
#define RET_BYTE              3
#define RET_BYTE_PTR          4
#define RET_BOOL              5
#define RET_BOOL_PTR          6
#define RET_BYTEWORD		  7
#define RET_INVALID			  8


int guardaTypeRet=0, guardaPtrRet=0;
ASTREE *funcAst, *funcAstVect, *initialRoot;
int scalDecl=0, scalDeclVect=0;

int getErrorNumber()
{
	return erro;
}

void initializeSemantic(ASTREE *node)
{
	initialRoot = node;
}

void checkDeclarationFuncParam(ASTREE *node, ASTREE *functionNode)
{
	if(node!=0)
	{
		if(node->type == AST_FUNCT_MORE_PARAMS || node->type == AST_FUNCT_PARAMS)
		{
			if(node->symbol->content.type == SYMBOL_TK_IDENTIFIER)
			{
				node->symbol->content.ast = (ASTREE*)malloc(sizeof(ASTREE));
				node->symbol->content.ast = functionNode;
				node->symbol->content.type = SYMBOL_TYPE_VAR;
			}
			else
			{
				printf("ERRO - Linha %d: Variavel %s na funcao ja definida.\n", node->lineNumber, node->symbol->content.text);
				erro=4;
			}
		}
		node = node->son[1];
		checkDeclarationFuncParam(node, functionNode);
	}
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
				printf("ERRO - Linha %d: Variavel %s ja definida.\n", root->lineNumber, root->symbol->content.text);
				erro=4;
			}
			else
			{
				if(root->type == AST_FUNCTION_DECL)
				{
					root->symbol->content.type = SYMBOL_TYPE_FUNC;
					if(root->son[1])
					{
						root->symbol->content.ast = (ASTREE*)malloc(sizeof(ASTREE));
						root->symbol->content.ast = root->son[1];
						checkDeclarationFuncParam(root->son[1], root);
					}
				} 
				else 
				{
					if (root->type == AST_DECL)
						root->symbol->content.type = SYMBOL_TYPE_VAR;
					else
						if (root->type == AST_DECL_POINTER)
							root->symbol->content.type = SYMBOL_TYPE_PTR;
						else
							if(root->type == AST_DECL_VECTOR || root->type == AST_DECL_VECTOR_INITVALUE)
								root->symbol->content.type = SYMBOL_TYPE_VEC;
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
	if(root->type == AST_FUNCTION_DECL)
		guardaTypeRet = dataTypeMap(root->son[0]->type);
		
	switch(root->type)
	{
		case AST_KW_INPUT:
			if(root->symbol != 0)
				if (root->symbol->content.type != SYMBOL_TYPE_VAR)
				{
	       			printf("ERRO - Linha %d: Declaracao da variavel em INPUT faltante!\n", root->lineNumber);
					erro=4;
				}
			break;
		case AST_EXPR_W_TKIDENTIFIER:
			if(root->symbol->content.type != SYMBOL_TK_IDENTIFIER)
			{
				if(root->symbol->content.type == SYMBOL_TYPE_FUNC)
				{
					funcAst = 0;
					checkFuncCall(rootAux, root->symbol->content.text);
					result = CHECKPARAMFUNC_OK;
					result = checkParamsFunc(funcAst->symbol->content.ast, root);
					switch(result)
					{
						case CHECKPARAMFUNC_OK:
							break;
						case CHECKPARAMFUNC_ERROR1:
							printf("ERRO - Linha %d: Funcao %s contem MAIS parametros do que o numero declarado!\n", root->lineNumber, root->symbol->content.text);
							erro=4;
							break;
						case CHECKPARAMFUNC_ERROR2:
							printf("ERRO - Linha %d: Funcao %s contem MENOS parametros do que o numero declarado!\n", root->lineNumber, root->symbol->content.text);
							erro=4;
							break;
						case CHECKPARAMFUNC_ERROR3:
							printf("ERRO - Linha %d: Parametros da funcao %s tem tipos diferentes dos declarados!\n", root->lineNumber, root->symbol->content.text);
							erro=4;
							break;
						case CHECKPARAMFUNC_ERROR4:
							printf("ERRO - Linha %d: Funcao nao pode ter uma funcao como parametro!\n", root->lineNumber, root->symbol->content.text);
							erro=4;
							break;
						case CHECKPARAMFUNC_ERROR5:
							printf("ERRO - Linha %d: Funcao nao pode ter um ponteiro como parametro!\n", root->lineNumber, root->symbol->content.text);
							erro=4;
							break;
						case CHECKPARAMFUNC_ERROR6:
							printf("ERRO - Linha %d: Funcao nao pode ter um vetor como ponteiro!\n", root->lineNumber, root->symbol->content.text);
							erro=4;
							break;

					}
				}
			}
			else
			{
				printf("ERRO - Linha %d: Funcao %s nao definida!\n", root->lineNumber, root->symbol->content.text);
				erro=4;
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
				//verRet = analisaExpressao(root->son[0], rootAux);
				verRet = 0;
				if(guardaTypeRet != verRet)
				{
					switch(guardaTypeRet)
					{
						case DATATYPE_KW_WORD:
							if(verRet == DATATYPE_KW_BYTE)
								printf("AVISO - Linha %d: Retornando BYTE ao inves de WORD!\n", root->lineNumber);		
							else
							{
								if(verRet == DATATYPE_KW_BOOL)
								{
									printf("ERRO - Linha %d: Retornando BOOL ao inves de WORD!\n", root->lineNumber);
									erro=4;
								}
								else
								{
									printf("ERRO - Linha %d: Retornando variavel nao declarada!\n", root->lineNumber);
									erro=4;
								}
							}
							break;
						case DATATYPE_KW_BYTE:
							if(verRet == DATATYPE_KW_WORD)
								printf("AVISO - Linha %d: Retornando WORD ao inves de BYTE! (possivel perda de representacao)!\n", root->lineNumber);		
							else 
							{
								if (verRet == DATATYPE_KW_BOOL)
								{
									printf("ERRO - Linha %d: Retornando BOOL ao inves de BYTE!\n", root->lineNumber);
									erro=4;
								}
								else
								{
									printf("ERRO - Linha %d: Retornando variavel nao declarada!\n", root->lineNumber);
									erro=4;
								}
							}
							break;
						case DATATYPE_KW_BOOL:
							if(verRet == DATATYPE_KW_WORD)
							{
								printf("ERRO - Linha %d: Retornando WORD ao inves de BOOL!\n", root->lineNumber);
								erro=4;	
							}
							else
							{ 
								if (verRet == DATATYPE_KW_BYTE)
								{
									printf("ERRO - Linha %d: Retornando BYTE ao inves de BOOL!\n", root->lineNumber);
									erro=4;
								}
								else
								{
									printf("ERRO - Linha %d: Retornando variavel nao declarada!\n", root->lineNumber);
									erro=4;
								}
							}
							break;
					}
				}
			}
			break;
		case AST_KW_IF:
			if(root->son[0]->son[0] == 0)
			{
				if(root->son[0]->type == AST_SYMBOL || root->son[0]->type == AST_POINTER)
				{
					if(!(root->son[0]->symbol->content.type == DATATYPE_KW_BOOL))
					{
						printf("ERRO - Linha %d: Expressao IF nao contem uma expressao booleana!\n", root->lineNumber);
						erro=4;
					}
				}
				else
				{
					if(root->son[0]->type != AST_LIT_TRUE && root->son[0]->type != AST_LIT_FALSE)
					{
						printf("ERRO - Linha %d: Expressao IF contem somente um literal!\n", root->lineNumber);
						erro=4;
					}
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
							printf("ERRO - Linha %d: Expressao aritmetica dentro de expressao LOOP!\n", root->lineNumber);
							erro=4;
						}
				}
			}
			break;
		case AST_EQUALS:
			if(analyseExpression(root->son[0]) == RET_INVALID)
			{
				printf("ERRO - Linha %d: Expressao com termos invalidos ou incompativeis!\n", root->lineNumber);
				erro=4;
			}
		default:
			break;
	}
	for(i=0; i < MAX_SONS; i++)
		checkUtilization(root->son[i], rootAux);
}

ASTREE* retrieveVarDeclaration(ASTREE *ast, ASTREE *nodeToRetrieve)
{
	int i;
	if(ast==0)
		return;
	if(ast->symbol!=0)
	{
		if( (ast->type == AST_DECL_VECTOR) || 
	   		(ast->type == AST_DECL_VECTOR_INITVALUE) || 
	   		(ast->type == AST_DECL) || 
	   		(ast->type == AST_DECL_POINTER) || 
	   		(ast->type == AST_FUNCTION_DECL) ||
	   		(ast->type ==AST_FUNCT_MORE_PARAMS) || 
	   		(ast->type == AST_FUNCT_PARAMS))
			if(strcmp(ast->symbol->content.text, nodeToRetrieve->symbol->content.text) == 0)
			{
				return ast;
			}
	}
	for(i=0; i < MAX_SONS; i++)
		retrieveVarDeclaration(ast->son[i], nodeToRetrieve);
}

int checkTwoRetTypes(int ret1, int ret2, int operation)
{
	if(ret1 == ret2)
		return ret1;
	if(ret1 == RET_BYTEWORD)
		if(ret2 == RET_BYTE || ret2 == RET_WORD)
			return ret2;
		else
			return RET_INVALID;
	if(ret2 == RET_BYTEWORD)
		if(ret1 == RET_BYTE || ret1 == RET_WORD)
			return ret1;
		else
			return RET_INVALID;
}

int checkRetType(ASTREE *node, int ptr)
{
	if(node!=0)
	{
		if(node->symbol!=0)
		{
			int retry = 0;
			ASTREE *nodeAux = node;
			if(retry < 2)
			{
				if(nodeAux->symbol->content.dataType == DATATYPE_KW_WORD && ptr == 0)
					return RET_WORD;
				if(nodeAux->symbol->content.dataType == DATATYPE_KW_WORD && ptr == 1)
					return RET_WORD_PTR;
				if(nodeAux->symbol->content.dataType == DATATYPE_KW_BYTE && ptr == 0)
					return RET_BYTE;
				if(nodeAux->symbol->content.dataType == DATATYPE_KW_BYTE && ptr == 1)
					return RET_BYTE_PTR;
				if(nodeAux->symbol->content.dataType == DATATYPE_KW_BOOL && ptr == 0)
					return RET_BOOL;
				if(nodeAux->symbol->content.dataType == DATATYPE_KW_BOOL && ptr == 1)
					return RET_BOOL_PTR;
				nodeAux = retrieveVarDeclaration(initialRoot, node);
				retry++;
			}
		}
		switch(node->type)
		{
			case AST_LIT_INTEGER:
				return RET_BYTEWORD;
				break;
			case AST_LIT_TRUE:
				return RET_BOOL;
				break;
			case AST_LIT_FALSE:
				return RET_BOOL;
				break;
			case AST_LIT_CHAR:
				return RET_BYTEWORD;
				break;
			case AST_LIT_STRING:
				return RET_INVALID;
				break;
			default:
				printf("\n\nCASO AINDA NAO DEFINIDO tipo nodo%d tipo dado %d tipo symbol %d\n", node->type, node->symbol->content.dataType, node->symbol->content.type);
				printf("TEXTO %s\n\n", node->symbol->content.text);
				return RET_INVALID;
				break;
		}
	}
	else
		return RET_UNKNOWN;
}

int analyseExpression(ASTREE *node)
{
	int retType = RET_UNKNOWN;
	int ret1 = RET_UNKNOWN, ret2 = RET_UNKNOWN;
	if(node != 0)
	{
		switch(node->type)
		{
			case AST_SYMBOL:
				if(node->symbol->content.type != SYMBOL_TYPE_VAR && node->symbol->content.type != SYMBOL_TYPE_PTR)
					return RET_INVALID;
				else
					retType = checkRetType(node, 0);
				break;
			case AST_LIT_INTEGER:
				retType = checkRetType(node, 0);
				break;
			case AST_LIT_TRUE:
				retType = checkRetType(node, 0);
				break;
			case AST_LIT_FALSE:
				retType = checkRetType(node, 0);
				break;
			case AST_LIT_CHAR:
				retType = checkRetType(node, 0);
				break;
			case AST_ADD:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_ADD);
				break;
			case AST_SUB:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_SUB);
				break;
			case AST_MUL:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_MUL);
				break;
			case AST_DIV:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_DIV);
				break;
			case AST_HIGHER:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_HIGHER);
				break;
			case AST_LOWER:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_LOWER);
				break;
			case AST_POINTER:
				if(node->symbol->content.type != SYMBOL_TYPE_PTR)
				{
					printf("ERRO - Linha %d: A variavel %s nao e um ponteiro!\n", node->lineNumber, node->symbol->content.text);
					return RET_INVALID;
				}
				else
					retType = checkRetType(node, 0);
				break;
			case AST_POINTER_REF:
				if(node->symbol->content.type != SYMBOL_TYPE_VAR)
				{
					printf("ERRO - Linha %d: A variavel %s nao e uma variavel do tipo BOOL, BYTE ou WORD para usar o operando &!\n", node->lineNumber, node->symbol->content.text);
					return RET_INVALID;
				}
				else
					retType = checkRetType(node, 1);
				break;
			case AST_EXPR_W_BRACKETS:
				retType = analyseExpression(node->son[0]);
				break;
			case AST_NOT:
				retType = analyseExpression(node->son[0]);
				if(retType != RET_BOOL)
					retType = RET_INVALID;
			case AST_OPERATOR_AND:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_OPERATOR_AND);
				break;
			case AST_OPERATOR_OR:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_OPERATOR_OR);
				break;
			case AST_OPERATOR_LE:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_OPERATOR_LE);
				break;
			case AST_OPERATOR_GE:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_OPERATOR_LE);
				break;
			case AST_OPERATOR_NE:
				ret1 = analyseExpression(node->son[0]);
				ret2 = analyseExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, AST_OPERATOR_NE);
				break;
			case AST_TK_IDENTIFIER_VET:
				ret1 = analyseExpression(node->son[0]);
				if(ret1 != RET_WORD && ret1 != RET_BYTE && ret1 != RET_BYTEWORD)
				{
					printf("ERRO - Linha %d: Indexacao errada do vetor %s. Necessita ser um BYTE ou WORD!\n", node->lineNumber, node->symbol->content.text);
					return RET_INVALID;
				}
				retType = checkRetType(node, 0);
				break;
			case AST_EXPR_W_TKIDENTIFIER:
				retType = checkRetType(node, 0);
				break;
			default:
				printf("\n\nNODO AINDA NAO DEFINIDO\n\n");
				break;
		}
	}
	return retType;
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

void checkVarDeclAsVector(ASTREE *ast, char *text)
{
	int i;

	if(ast==0)
		return;

	if((ast->symbol!=0) && ((ast->type == AST_DECL_VECTOR_INITVALUE) || (ast->type == AST_DECL_VECTOR)))
		if(strcmp(ast->symbol->content.text, text) == 0)
		{
			funcAstVect = ast;
			scalDeclVect = 1;
			return;
		}
	for(i=0; i < MAX_SONS; i++)
		checkVarDeclAsVector(ast->son[i], text);
}

int checkParamsFuncAux(ASTREE *ast, ASTREE *node)
{
	if(node!=0)
	{
		switch(node->type)
		{
			case AST_SYMBOL:
				if((node->symbol->content.dataType == ast->symbol->content.dataType) && node->symbol->content.type == SYMBOL_TYPE_VAR)
					return CHECKPARAMFUNC_OK;
				else
					if(node->symbol->content.dataType != ast->symbol->content.dataType)
						return CHECKPARAMFUNC_ERROR3;
					else
						if(node->symbol->content.type == SYMBOL_TYPE_FUNC)
							return CHECKPARAMFUNC_ERROR4;
						else
							if(node->symbol->content.type == SYMBOL_TYPE_PTR)
								return CHECKPARAMFUNC_ERROR5;
							else
								if(node->symbol->content.type == SYMBOL_TYPE_VEC)
									return CHECKPARAMFUNC_ERROR6;
				break;
			case AST_USING_PARAMETER_CONST:
				switch(node->son[0]->type)
				{
					case AST_LIT_INTEGER:
						if(ast->symbol->content.dataType == DATATYPE_KW_WORD || ast->symbol->content.dataType == DATATYPE_KW_BYTE)
							return CHECKPARAMFUNC_OK;
						else
							return CHECKPARAMFUNC_ERROR3;
						break;
					case AST_LIT_CHAR:
						if(ast->symbol->content.dataType == DATATYPE_KW_WORD || ast->symbol->content.dataType == DATATYPE_KW_BYTE)
							return CHECKPARAMFUNC_OK;
						else
							return CHECKPARAMFUNC_ERROR3;
						break;
					case AST_LIT_TRUE:
						if(ast->symbol->content.dataType == DATATYPE_KW_BOOL)
							return CHECKPARAMFUNC_OK;
						else
							return CHECKPARAMFUNC_ERROR3;
						break;
					case AST_LIT_FALSE:
						if(ast->symbol->content.dataType == DATATYPE_KW_BOOL)
							return CHECKPARAMFUNC_OK;
						else
							return CHECKPARAMFUNC_ERROR3;
						break;
					default:
						break;
				}
				break;
			default:
				return CHECKPARAMFUNC_OK;
		}	
	}
	else
		return CHECKPARAMFUNC_OK;
}
int checkParamsFunc(ASTREE *ast, ASTREE *son)
{
	int ret = 0;
	ASTREE *nodeSon = 0, *nodeSonAux = 0;

	nodeSon = son->son[0];
	int iterationNumber = 0;
	do
	{
		if(ast==0)
		{
			if(nodeSon==0)
				return CHECKPARAMFUNC_OK;
			else
				return CHECKPARAMFUNC_ERROR1;
		}
		else
		{
			if(nodeSon==0)
			{
				return CHECKPARAMFUNC_ERROR2;
			}
			else
			{
				if(nodeSon->son[0]!=0)
				{
					if(nodeSon->son[0]->type == AST_USING_PRIMARY_PARAMETER)
					{
						ret = checkParamsFuncAux(ast, nodeSon->son[0]->son[0]);
						if(ret != CHECKPARAMFUNC_OK)
							return ret;
					}
					else
					{
						ret = checkParamsFuncAux(ast, nodeSon->son[0]);
						if(ret != CHECKPARAMFUNC_OK)
							return ret;
					}
				}
				else
				{
					ret = checkParamsFuncAux(ast, nodeSon);
					if(ret != CHECKPARAMFUNC_OK)
						return ret;
				}
				nodeSon = nodeSon->son[1];
				ast = ast->son[1];
			}
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