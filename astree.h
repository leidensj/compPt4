#ifndef ASTREE_HEADER
#define ASTREE_HEADER

//arvore com 4 filhos
#include "hash.h"
#define MAX_SONS 4

#define AST_SYMBOL 1
#define AST_ADD 2
#define AST_SUB 3 
#define AST_MUL 4
#define AST_DIV 5
#define AST_EQUALS 6
#define AST_CMDS 7
#define AST_KW_LOOP 8
#define AST_KW_IF 9
#define AST_KW_IF_ELSE 10
#define AST_KW_INPUT 11
#define AST_OUTPUT 12
#define AST_RETURN 13
#define AST_FUNCTION_DECL 14

#define AST_HIGHER 15
#define AST_LOWER 16
#define AST_OPERATOR_AND 17
#define AST_OPERATOR_OR 18
#define AST_OPERATOR_LE 19
#define AST_OPERATOR_GE 20
#define AST_OPERATOR_EQ 21
#define AST_OPERATOR_NE 22
#define AST_NOT 23
#define AST_GLOBAL_DECL 24
#define AST_KW_WORD 25
#define AST_KW_BYTE 26
#define AST_KW_BOOL 27
#define AST_LIT_INTEGER 28
#define AST_LIT_TRUE 29
#define AST_LIT_FALSE 30
#define AST_LIT_CHAR 31
#define AST_LIT_STRING 32
#define AST_POINTER 33
#define AST_POINTER_REF 34
#define AST_EXPR_W_BRACKETS 35
#define AST_OUTP1 36
#define AST_OUTP2 37
#define AST_USING_PRIMARY_PARAMETER 38
#define AST_USING_PARAMETERS 39
#define AST_EXPR_W_TKIDENTIFIER 40
#define AST_TK_IDENTIFIER_VET 41
#define AST_USING_PARAMETER_CONST 42
#define AST_VECTOR_SIZE 43
#define AST_DECL_VECTOR 44
#define AST_DECL_VECTOR_INITVALUE 45
#define AST_DECL 46
#define AST_DECL_POINTER 47
#define AST_INIT_VEC_VALUES 48
#define AST_BLOCK 49
#define AST_VECTOR_EQUALS 50
#define AST_FUNCT_PARAMS 51
#define AST_FUNCT_MORE_PARAMS 52

typedef struct astreenode
{
     int type;
     HASH_NODE* symbol;
     struct astreenode* son[MAX_SONS];
     int printed;
     int lineNumber;
} ASTREE;

ASTREE* astCreate(int type, HASH_NODE* symbol, ASTREE* s0, ASTREE* s1, ASTREE* s2, ASTREE* s3);
void astPrintSingle(ASTREE* node);
void astPrintTree(ASTREE* root, int level);
void astGetOutputFileName(char *fn);
int dataTypeMap(int astType);
#endif
