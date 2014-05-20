#ifndef HASH_H_
#define HASH_H_

#define HASH_SIZE 997

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