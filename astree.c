#include "astree.h"
#include <stdlib.h>
#include <stdio.h>

FILE *fd = 0;

void astGetOutputFileName(char *fn)
{
    fd = fopen(fn, "w");
    if(fd == 0)
    {
        printf("\nImpossivel criar arquivo de saida.\n\n");
        exit(-30);
    }
}

ASTREE* astCreate(int type, HASH_NODE* symbol, ASTREE* s0, ASTREE* s1, ASTREE* s2, ASTREE* s3)
{
     ASTREE* node;
     node = (ASTREE*) calloc(1,sizeof(ASTREE));
     node->type = type;
     node->symbol = symbol;
     node->son[0] = s0;
     node->son[1] = s1;
     node->son[2] = s2;
     node->son[3] = s3;
     node->printed = 0;
     node->lineNumber = getLineNumber();
     return node;
}

void astPrintSingle(ASTREE* node)
{
    if(fd == 0)
    {
        printf("\nImpossivel criar arquivo de saida.\n\n");
        exit(-1);
    }
     if(node == 0) return;
     if(node->printed == 1) return;
     node->printed = 1;
     switch(node->type)
     {
          case AST_GLOBAL_DECL:
               astPrintSingle(node->son[0]);
               astPrintSingle(node->son[1]);                
               break;
          case AST_SYMBOL:
               fprintf(fd, "%s", node->symbol->content.text);
               break;            
          case AST_ADD:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " + ");
		     astPrintSingle(node->son[1]);
               break;
          case AST_SUB:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " - ");
		     astPrintSingle(node->son[1]);
               break;
          case AST_MUL:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " * ");
		     astPrintSingle(node->son[1]);
               break;
          case AST_DIV:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " / ");
		     astPrintSingle(node->son[1]);
               break;
          case AST_EQUALS:
               fprintf(fd, "%s", node->symbol->content.text); 
		     fprintf(fd, " = ");
		     astPrintSingle(node->son[0]);
               fprintf(fd, "\n");
               break;
          case AST_CMDS:
               astPrintSingle(node->son[0]);
               astPrintSingle(node->son[1]);
               break;
          case AST_KW_LOOP:
               fprintf(fd, "loop\n");
               astPrintSingle(node->son[0]);
               fprintf(fd, "(");
               astPrintSingle(node->son[1]);
               fprintf(fd, ")\n");
               break;
          case AST_KW_IF:
               fprintf(fd, "if(");
               astPrintSingle(node->son[0]);
               fprintf(fd, ") then");
               astPrintSingle(node->son[1]);
               break;
          case AST_KW_IF_ELSE:
               fprintf(fd, "if(");
               astPrintSingle(node->son[0]);
               fprintf(fd, ") else\n");
               astPrintSingle(node->son[1]);
               fprintf(fd, "then\n");
               astPrintSingle(node->son[2]);
               break;
          case AST_KW_INPUT:
               fprintf(fd, "input ");
               fprintf(fd, "%s", node->symbol->content.text);
               fprintf(fd, "\n");
               break;
          case AST_OUTPUT:
               fprintf(fd, "output ");
               astPrintSingle(node->son[0]);
               fprintf(fd, "\n");       
               break;
          case AST_RETURN:
               fprintf(fd, "return ");
               astPrintSingle(node->son[0]);
               fprintf(fd, "\n");
               break;
          case AST_FUNCTION_DECL:
               astPrintSingle(node->son[0]);
               fprintf(fd, " %s(", node->symbol->content.text);
               astPrintSingle(node->son[1]);
               fprintf(fd, ")\n");
               astPrintSingle(node->son[2]);
               fprintf(fd, ";\n");
               break;
          case AST_DECL_VECTOR:
               astPrintSingle(node->son[0]);
               fprintf(fd, " %s[", node->symbol->content.text);
               astPrintSingle(node->son[1]);
               fprintf(fd, "];");
               fprintf(fd, "\n");
               break;
          case AST_DECL_VECTOR_INITVALUE:
               astPrintSingle(node->son[0]);
               fprintf(fd, " %s[", node->symbol->content.text);
               astPrintSingle(node->son[1]);
               fprintf(fd, "]:");
               astPrintSingle(node->son[2]);
               fprintf(fd, ";\n");
               break;
          case AST_DECL:
               astPrintSingle(node->son[0]);
               fprintf(fd, " %s:", node->symbol->content.text);
               astPrintSingle(node->son[1]);
               fprintf(fd, ";\n");
               break;
          case AST_DECL_POINTER:
               astPrintSingle(node->son[0]);
               fprintf(fd, " $%s:", node->symbol->content.text);
               astPrintSingle(node->son[1]);
               fprintf(fd, ";\n");
               break;
          case AST_VECTOR_SIZE:
               fprintf(fd, "%s", node->symbol->content.text);
               break;
          case AST_LIT_INTEGER:
               fprintf(fd, "%s", node->symbol->content.text);
               break;
          case AST_LIT_TRUE:
               fprintf(fd, "true");
               break;
          case AST_LIT_FALSE:
               fprintf(fd, "false");
               break;
          case AST_LIT_CHAR:
               fprintf(fd, "%s", node->symbol->content.text);
               break;
          case AST_LIT_STRING:
               fprintf(fd, "%s", node->symbol->content.text);
               break;
          case AST_INIT_VEC_VALUES:
               fprintf(fd, "%s ", node->symbol->content.text);
               astPrintSingle(node->son[0]);
               break;
          case AST_KW_WORD:
               fprintf(fd, "word");
               break;
          case AST_KW_BYTE:
               fprintf(fd, "byte");
               break;
          case AST_KW_BOOL:
               fprintf(fd, "bool");
               break;
          case AST_FUNCT_PARAMS:
               astPrintSingle(node->son[0]);
               fprintf(fd, " %s", node->symbol->content.text);
               astPrintSingle(node->son[1]);
               break;
          case AST_FUNCT_MORE_PARAMS:
               fprintf(fd, ", ");
               astPrintSingle(node->son[0]);
               fprintf(fd, " %s", node->symbol->content.text);
               astPrintSingle(node->son[1]);
               break;
          case AST_BLOCK:
               fprintf(fd, "{\n");
               astPrintSingle(node->son[0]);
               fprintf(fd, "}\n");
               break;
          case AST_VECTOR_EQUALS:
               fprintf(fd, "%s[", node->symbol->content.text);
               astPrintSingle(node->son[0]);
               fprintf(fd, "] = ");
               astPrintSingle(node->son[1]);
               fprintf(fd, "\n");
          case AST_OUTP1:
               astPrintSingle(node->son[0]);
               break;
          case AST_OUTP2:
               astPrintSingle(node->son[0]);
               fprintf(fd, ", ");
               astPrintSingle(node->son[1]);
               break;
          case AST_HIGHER:
               astPrintSingle(node->son[0]);
               fprintf(fd, " > ");
               astPrintSingle(node->son[1]);
               break;
          case AST_LOWER:
               astPrintSingle(node->son[0]);
               fprintf(fd, " < ");
               astPrintSingle(node->son[1]);
               break;
          case AST_POINTER:
               fprintf(fd, "$");
               astPrintSingle(node->son[0]);
               break;
          case AST_POINTER_REF:
               fprintf(fd, "&");
               astPrintSingle(node->son[0]);
               break;
          case AST_EXPR_W_BRACKETS:
               fprintf(fd, "(");
               astPrintSingle(node->son[0]);
               fprintf(fd, ")");
               break;
          case AST_NOT:
               fprintf(fd, "!");
               astPrintSingle(node->son[0]);     
	          break;
          case AST_OPERATOR_AND:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " && ");
		     astPrintSingle(node->son[1]);
               fprintf(fd, "\n");
               break;
          case AST_OPERATOR_OR:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " || ");
		     astPrintSingle(node->son[1]);
               fprintf(fd, "\n");
               break;
          case AST_OPERATOR_LE:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " <= ");
		     astPrintSingle(node->son[1]);
               fprintf(fd, "\n");
               break;
          case AST_OPERATOR_GE:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " >= ");
		     astPrintSingle(node->son[1]);
               fprintf(fd, "\n");
               break;
          case AST_OPERATOR_EQ:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " == ");
		     astPrintSingle(node->son[1]);
               fprintf(fd, "\n");
               break;
          case AST_OPERATOR_NE:
     		astPrintSingle(node->son[0]);     
		     fprintf(fd, " != ");
		     astPrintSingle(node->son[1]);
               fprintf(fd, "\n");
               break;
          case AST_TK_IDENTIFIER_VET:
               fprintf(fd, "%s[", node->symbol->content.text);
               astPrintSingle(node->son[0]);
               fprintf(fd, "]");
               break;
          case AST_EXPR_W_TKIDENTIFIER:
               fprintf(fd, "%s(", node->symbol->content.text);
               astPrintSingle(node->son[0]);
               fprintf(fd, ")");
               break;
          case AST_USING_PRIMARY_PARAMETER:
               astPrintSingle(node->son[0]);
               break;
          case AST_USING_PARAMETERS:
               astPrintSingle(node->son[0]);
               fprintf(fd, ", ");
               astPrintSingle(node->son[1]);
               break;
          case AST_USING_PARAMETER_CONST:
               astPrintSingle(node->son[0]);
               break;
          default: fprintf(fd, "unknown,");break; 
     }
}

void astPrintTree(ASTREE* root)
{
     int i;

     if(root == 0)
          return;

     for(i = 0; i<MAX_SONS; ++i)
          astPrintTree(root->son[i]);
}

int dataTypeMap(int astType)
{
     switch (astType)
     {
          case AST_KW_WORD: return DATATYPE_KW_WORD;
          case AST_KW_BYTE: return DATATYPE_KW_BYTE;
          case AST_KW_BOOL: return DATATYPE_KW_BOOL;
     }
}