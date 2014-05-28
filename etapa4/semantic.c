#include "semantic.h"
#include <stdio.h>

#define CHECKPARAMFUNC_OK	  100
#define CHECKPARAMFUNC_ERROR1 101
#define CHECKPARAMFUNC_ERROR2 102
#define CHECKPARAMFUNC_ERROR3 103
#define CHECKPARAMFUNC_ERROR4 104
#define CHECKPARAMFUNC_ERROR5 105
#define CHECKPARAMFUNC_ERROR6 106

#define RET_UNKNOWN 		  200
#define RET_WORD 			  201
#define RET_WORD_PTR          202
#define RET_BYTE              203
#define RET_BYTE_PTR          204
#define RET_BOOL              205
#define RET_BOOL_PTR          206
#define RET_BYTEWORD		  207
#define RET_INVALID			  208

#define OP_TYPE_LOGICAL       301
#define OP_TYPE_ARITH 		  302
#define OP_TYPE_ADD			  303 //add funciona pra variaveis E ponteiros ;)


int guardaTypeRet=0, initialize = 0;
ASTREE *funcAst, *initialRoot;

int getErrorNumber()
{
	return erro;
}

void checkDeclaration(ASTREE *root)
{
	if(initialize == 0)
	{
		initialRoot = root;
		initialize = 1;
	}

	int i;
	if (root == 0)
		return;		

	if((root->type == AST_DECL_VECTOR) || 
	   (root->type == AST_DECL_VECTOR_INITVALUE) || 
	   (root->type == AST_DECL) || 
	   (root->type == AST_DECL_POINTER) || 
	   (root->type == AST_FUNCTION_DECL) ||
	   (root->type == AST_FUNCT_MORE_PARAMS) ||
	   (root->type == AST_FUNCT_PARAMS))
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
					root->symbol->content.type = SYMBOL_TYPE_FUNC;
				else 
				{
					if (root->type == AST_DECL || root->type == AST_FUNCT_MORE_PARAMS || root->type == AST_FUNCT_PARAMS)
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

void checkUtilization(ASTREE *root, ASTREE *rootAux)
{
	int i, result, verRet=0, verRet2=0, operIgual=0;

	if (root == 0)
		return;
	if(root->type == AST_FUNCTION_DECL)
		guardaTypeRet = checkRetType(root, 0);
		
	switch(root->type)
	{
		case AST_KW_INPUT:
			if(root->symbol != 0)
				if (root->symbol->content.type != SYMBOL_TYPE_VAR)
				{
	       			printf("ERRO - Linha %d: Variavel em INPUT faltante!\n", root->lineNumber);
					erro=4;
				}
			break;
		case AST_EXPR_W_TKIDENTIFIER:
			if(root->symbol->content.type != SYMBOL_TK_IDENTIFIER)
			{
				funcAst = 0;
				retrieveFuncDeclaration(initialRoot, root);
				if(funcAst != 0)
				{
					result = CHECKPARAMFUNC_OK;
					result = checkParamsFunc(funcAst->son[1], root);
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
			verRet = analyzeExpression(root->son[0]);
			if(guardaTypeRet != verRet)
			{
				if(verRet == RET_INVALID)
				{
					printf("ERRO - Linha %d: Expressao invalida de retorno! Retornar funcao ou vetor nao e possivel\n", root->lineNumber-1);
					erro=4;
				}
				else
				{
					if(guardaTypeRet == RET_BYTE && verRet == RET_WORD)
						printf("AVISO - Linha %d: Retornando WORD ao inves de byte, perda de representatividade possivel!\n", root->lineNumber-1);
					else
						if(!((guardaTypeRet == RET_BYTE || guardaTypeRet == RET_WORD) && (verRet == RET_BYTEWORD || verRet == RET_BYTE)))
						{
							printf("ERRO - Linha %d: Incompatibilidade entre retorno da funcao e retorno da expressao!\n", root->lineNumber-1);
							erro=4;
						}
				}
			}
			break;
		case AST_KW_IF:
			verRet = analyzeExpression(root->son[0]);
			if(verRet != RET_BOOL)
			{
				printf("ERRO - Linha %d: Expressao IF nao contem uma expressao booleana!\n", root->lineNumber-1);
				erro=4;
			}
			break;
		case AST_KW_IF_ELSE:
			verRet = analyzeExpression(root->son[0]);
			if(verRet != RET_BOOL)
			{
				printf("ERRO - Linha %d: Expressao IF nao contem uma expressao booleana!\n", root->lineNumber-1);
				erro=4;
			}
			break;
		case AST_KW_LOOP:
			verRet = analyzeExpression(root->son[1]);
			if(verRet != RET_BOOL)
			{
				printf("ERRO - Linha %d: Expressao AST_KW_LOOP nao contem uma expressao booleana!\n", root->lineNumber-1);
				erro=4;
			}
			break;
		case AST_VECTOR_EQUALS:
			if(root->symbol->content.type!=SYMBOL_TYPE_VEC)
			{
				printf("ERRO - Linha %d: Variavel %s da atribuicao nao e um vetor!\n", root->lineNumber-1, root->symbol->content.text);
				erro=4;
				break;
			}
			verRet = analyzeExpression(root->son[0]);
			if(verRet != RET_BYTE && verRet != RET_WORD && verRet != RET_BYTEWORD)
			{
				printf("ERRO - Linha %d: Indexacao errada do vetor %s!\n", root->lineNumber-1, root->symbol->content.text);
				erro=4;
				break;
			}
			if(root->symbol->content.type == SYMBOL_TYPE_PTR)
				verRet2 = checkRetType(root, 1);
			else
				verRet2 = checkRetType(root, 0);
			if(verRet2 != verRet)
			{
				if(verRet == RET_INVALID)
				{
					printf("ERRO - Linha %d: Expressao invalida de atribuicao!\n", root->lineNumber-1);
					erro=4;
				}
				else
				{
					if(verRet2 == RET_BYTE && verRet == RET_WORD)
						printf("AVISO - Linha %d: Atribuicao de WORD ao inves de byte, perda de representatividade possivel!\n", root->lineNumber-1);
					else
						if(!((verRet2 == RET_BYTE || verRet2 == RET_WORD) && (verRet == RET_BYTEWORD || verRet == RET_BYTE)))
						{
							printf("ERRO - Linha %d: Incompatibilidade entre atribuicao!\n", root->lineNumber-1);
							erro=4;
						}
				}
			}
			break;
		case AST_EQUALS:
			if(root->symbol->content.type==SYMBOL_TYPE_VEC)
			{
				printf("ERRO - Linha %d: Atribuicao ao vetor nao e possivel!\n", root->lineNumber-1);
				erro=4;
				break;
			}
			if(root->symbol->content.type==SYMBOL_TYPE_FUNC)
			{
				printf("ERRO - Linha %d: Atribuicao a funcao nao e possivel!\n", root->lineNumber-1);
				erro=4;
				break;
			}
			verRet = analyzeExpression(root->son[0]);
			if(verRet == RET_INVALID)
			{
				printf("ERRO - Linha %d: Expressao com termos invalidos ou incompativeis na atribuicao!\n", root->lineNumber-1);
				erro=4;
				break;
			}
			if(root->symbol->content.type == SYMBOL_TYPE_PTR)
				verRet2 = checkRetType(root, 1);
			else
				verRet2 = checkRetType(root, 0);
			if(verRet2 != verRet)
			{
				if(verRet == RET_INVALID)
				{
					printf("ERRO - Linha %d: Expressao invalida de atribuicao!\n", root->lineNumber-1);
					erro=4;
				}
				else
				{
					if(verRet2 == RET_BYTE && verRet == RET_WORD)
						printf("AVISO - Linha %d: Atribuicao de WORD ao inves de byte, perda de representatividade possivel!\n", root->lineNumber-1);
					else
						if(!((verRet2 == RET_BYTE || verRet2 == RET_WORD) && (verRet == RET_BYTEWORD || verRet == RET_BYTE)))
						{
							printf("ERRO - Linha %d: Incompatibilidade entre atribuicao!\n", root->lineNumber-1);
							erro=4;
						}
				}
			}
			break;
		default:
			break;
	}
	for(i=0; i < MAX_SONS; i++)
		checkUtilization(root->son[i], rootAux);
}

void retrieveFuncDeclaration(ASTREE *ast, ASTREE *nodeToRetrieve)
{
	int i;
	if(ast==0)
		return;
	if(ast->symbol!=0)
	{
		if(ast->type == AST_FUNCTION_DECL)
			if(strcmp(ast->symbol->content.text, nodeToRetrieve->symbol->content.text) == 0)
			{
				funcAst = ast;
				return;
			}
	}
	for(i=0; i < MAX_SONS; i++)
		retrieveFuncDeclaration(ast->son[i], nodeToRetrieve);
}

int checkTwoRetTypes(int ret1, int ret2, int operation)
{
	if(operation == OP_TYPE_ARITH)
	{
		if(ret1 == RET_BYTE_PTR || ret1 == RET_WORD_PTR || ret1 == RET_BOOL_PTR)
			return RET_INVALID;
		if(ret2 == RET_BYTE_PTR || ret2 == RET_WORD_PTR || ret2 == RET_BOOL_PTR)
			return RET_INVALID;
	}
	if(operation == OP_TYPE_LOGICAL)
	{
		if(ret1 == RET_BYTE && ret1 == RET_WORD && ret1 == RET_BOOL)
			return RET_INVALID;
		if(ret2 == RET_BYTE && ret2 == RET_WORD && ret2 == RET_BOOL)
			return RET_INVALID;
		if(ret1 != ret2 && (ret1==RET_BOOL || ret2==RET_BOOL))
			return RET_INVALID;
		else
			return RET_BOOL;
	}
	if(operation == OP_TYPE_ADD)
	{
		if(ret1 != RET_BYTE_PTR || ret1 == RET_WORD_PTR || ret1 == RET_BOOL_PTR)
			return ret1;
		else
			if(ret2 != RET_BYTE_PTR || ret2 == RET_WORD_PTR || ret2 == RET_BOOL_PTR)
				return ret2;
	}
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
			if(node->symbol->content.dataType == DATATYPE_KW_WORD && ptr == 0)
				return RET_WORD;
			if(node->symbol->content.dataType == DATATYPE_KW_WORD && ptr == 1)
				return RET_WORD_PTR;
			if(node->symbol->content.dataType == DATATYPE_KW_BYTE && ptr == 0)
				return RET_BYTE;
			if(node->symbol->content.dataType == DATATYPE_KW_BYTE && ptr == 1)
				return RET_BYTE_PTR;
			if(node->symbol->content.dataType == DATATYPE_KW_BOOL && ptr == 0)
				return RET_BOOL;
			if(node->symbol->content.dataType == DATATYPE_KW_BOOL && ptr == 1)
				return RET_BOOL_PTR;
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

int analyzeExpression(ASTREE *node)
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
				{
					if(node->symbol->content.type == SYMBOL_TYPE_VAR)
						retType = checkRetType(node, 0);
					if(node->symbol->content.type == SYMBOL_TYPE_PTR)
						retType = checkRetType(node, 1);
				}
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
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_ADD);
				break;
			case AST_SUB:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_ARITH);
				break;
			case AST_MUL:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_ARITH);
				break;
			case AST_DIV:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_ARITH);
				break;
			case AST_HIGHER:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_LOGICAL);
				break;
			case AST_LOWER:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_LOGICAL);
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
				retType = analyzeExpression(node->son[0]);
				break;
			case AST_NOT:
				retType = analyzeExpression(node->son[0]);
				if(retType != RET_BOOL)
					retType = RET_INVALID;
			case AST_OPERATOR_AND:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_LOGICAL);
				break;
			case AST_OPERATOR_OR:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_LOGICAL);
				break;
			case AST_OPERATOR_LE:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_LOGICAL);
				break;
			case AST_OPERATOR_GE:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_LOGICAL);
				break;
			case AST_OPERATOR_NE:
				ret1 = analyzeExpression(node->son[0]);
				ret2 = analyzeExpression(node->son[1]);
				retType = checkTwoRetTypes(ret1, ret2, OP_TYPE_LOGICAL);
				break;
			case AST_TK_IDENTIFIER_VET:
				if(node->symbol->content.type != SYMBOL_TYPE_VEC)
				{
					printf("ERRO - Linha %d: Variavel %s nao e um vetor!\n", node->lineNumber, node->symbol->content.text);
					return RET_INVALID;
				}
				ret1 = analyzeExpression(node->son[0]);
				if(ret1 != RET_WORD && ret1 != RET_BYTE && ret1 != RET_BYTEWORD)
				{
					printf("ERRO - Linha %d: Indexacao errada do vetor %s!\n", node->lineNumber, node->symbol->content.text);
					return RET_INVALID;
				}
				retType = checkRetType(node, 0);
				break;
			case AST_EXPR_W_TKIDENTIFIER:
				if(node->symbol->content.type != SYMBOL_TYPE_FUNC)
				{
					printf("ERRO - Linha %d: Variavel %s nao e uma funcao!\n", node->lineNumber, node->symbol->content.text);
					return RET_INVALID;
				}
				retType = checkRetType(node, 0);
				break;
			default:
				printf("\n\nNODO AINDA NAO DEFINIDO\n\n");
				break;
		}
	}
	return retType;
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
	int end = 0;
	nodeSon = son->son[0];
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
				return CHECKPARAMFUNC_ERROR2;	
			else
			{
				if(nodeSon->son[1]==0)
				{
					if(nodeSon->son[0]->type == AST_USING_PRIMARY_PARAMETER)
					{
						end = 1;
						ret = checkParamsFuncAux(ast, nodeSon->son[0]->son[0]);
						if(ret != CHECKPARAMFUNC_OK)
							return ret;
					}
					else
					{
						end = 1;
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
				if(end == 0)
					nodeSon = nodeSon->son[0];
				else
					nodeSon = 0;
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