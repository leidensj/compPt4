#ifndef HASH_H_
#define HASH_H_

#define HASH_SIZE 997

#define SYMBOL_TK_IDENTIFIER  1
#define SYMBOL_LIT_INTEGER    2
#define SYMBOL_LIT_FLOATING   3
#define SYMBOL_LIT_TRUE       4
#define SYMBOL_LIT_FALSE      5
#define SYMBOL_LIT_CHAR       6
#define SYMBOL_LIT_STRING     7

#define SYMBOL_VARIABLE       8
#define SYMBOL_VECTOR         9
#define SYMBOL_FUNCTION       10

#define DATATYPE_KW_WORD      1
#define DATATYPE_KW_BYTE      3
#define DATATYPE_KW_BOOL      4

typedef struct {
     char *text;
     int type;
     int wordVal;
     char charVal;
     int boolVal;
     void *ast;
     int dataType;
     int lineNumber;
} TOKEN_CONTENT;

typedef struct hash_node {
	struct hash_node *next;
     TOKEN_CONTENT content;
} HASH_NODE;

HASH_NODE* Table[HASH_SIZE];

void hashInit(void);
void hashPrint();
int hashDestroy();
int hashAddress(char *text);
HASH_NODE *hashInsert(char *text, int type);
HASH_NODE *hashFind(char *text, int type);
TOKEN_CONTENT *hashGet(int type, char *text);

#endif