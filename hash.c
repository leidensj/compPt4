#include "hash.h"
#include "astree.h"
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createTokenContent(TOKEN_CONTENT *content, int type, char *text) 
{
    int len = strlen(text);
    content->text = (char*) malloc(len + 1);
    strcpy(content->text, text);

    content->type = type;
    content->wordVal = 0;
    content->charVal = '\0';
    content->boolVal = 0;
    content->dataType  = 0;
    content->ast = 0;
    content->lineNumber = getLineNumber();
    
    switch (type) 
    {
        case SYMBOL_LIT_INTEGER:
            content->wordVal = atoi(text);
            break;
        case SYMBOL_LIT_CHAR:
            content->charVal = text[1];
            break;
        case SYMBOL_LIT_TRUE:
            content->boolVal = 1;
            break;
        case SYMBOL_LIT_FALSE:
            content->boolVal = 0;
            break;
    }
}

void hashInit(void)
{
    int i;
    for (i = 0; i < HASH_SIZE; i++)
        Table[i] = 0;
}

int hashAddress(char *text)
{
    int addr = 1;
    int i;

    for (i=0; i<strlen(text); i++)
        addr = (addr*text[i]) % HASH_SIZE + 1;
    return addr - 1;
}
	
HASH_NODE *hashInsert(char *text, int type)
{
    HASH_NODE *found = hashFind(text, type);
    if(!found)
    {
        HASH_NODE *newnode;
        newnode = (HASH_NODE*) calloc(1, sizeof(HASH_NODE));
        int address = hashAddress(text);
        newnode->next = Table[address];
        createTokenContent(&(newnode->content), type, text);
        Table[address] = newnode;
        return newnode;
    }
    else
        return found;
}
	
HASH_NODE* hashFind(char *text, int type)
{
    HASH_NODE *pt;
    int addr = hashAddress(text);
	
    for(pt = Table[addr]; pt; pt = pt->next)
        if(!strcmp(pt->content.text, text) && pt->content.type == type)
            return pt;

    return NULL;
}
	
void hashPrint()
{
    int i;
    HASH_NODE *pt;
    printf("╔═══════════════════════════════════════════════════════════════════════\n");
    printf("║TABELA HASH\n");
    for(i = 0; i< HASH_SIZE; i++)
        for(pt = Table[i]; pt; pt = pt->next)
            switch(pt->content.type)
            {
                case SYMBOL_TK_IDENTIFIER:  printf("║T:%d\tL:%d\t=> NAO DECLARADO = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case SYMBOL_TYPE_VAR:       printf("║T:%d\tL:%d\t=> VARIAVEL      = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case SYMBOL_TYPE_FUNC:      printf("║T:%d\tL:%d\t=> FUNCAO        = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case SYMBOL_TYPE_VEC:       printf("║T:%d\tL:%d\t=> VETOR         = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case SYMBOL_TYPE_PTR:       printf("║T:%d\tL:%d\t=> PONTEIRO      = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case SYMBOL_LIT_INTEGER:    printf("║T:%d\tL:%d\t=> INTEIRO       = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case SYMBOL_LIT_FALSE:      printf("║T:%d\tL:%d\t=> FALSE         = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case SYMBOL_LIT_TRUE:       printf("║T:%d\tL:%d\t=> TRUE          = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case SYMBOL_LIT_CHAR:       printf("║T:%d\tL:%d\t=> CHAR          = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case SYMBOL_LIT_STRING:     printf("║T:%d\tL:%d\t=> STRING        = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                case TOKEN_ERROR:           printf("║T:%d\tL:%d\t=> ERROR         = %s \n", i, pt->content.lineNumber, pt->content.text); break;
                default:                    printf("║ DEFAULT  %s \n", pt->content.text); break;
            }
    printf("╚═══════════════════════════════════════════════════════════════════════\n");
}

TOKEN_CONTENT *hashGet(int type, char *text) 
{
    int address;
    HASH_NODE *node;
    
    address = hashAddress(text);
    
    node = hashFind(text, type);
    
    if (node != NULL) {
        return &(node->content);
    }    
    return NULL;
}

void destroyTokenContent(TOKEN_CONTENT *content) 
{
    if (content->text != NULL)
        free(content->text);
}

int hashDestroy() 
{
    int i;
    HASH_NODE *node;
    
    for (i = 0; i < HASH_SIZE+1; i++) {
        for (node = Table[i]; node != NULL; node = node->next) {
            destroyTokenContent(&node->content);
            free(node);
        }
        Table[i] = NULL;
    }
        
    return 0;
}