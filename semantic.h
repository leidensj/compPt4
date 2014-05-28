#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "astree.h"

int erro;

void checkDeclarationFuncParam(ASTREE *node, ASTREE *functionNode);

void checkDeclaration(ASTREE *root);

void checkVarDeclAsFunc(ASTREE *ast, char *text);

void checkVarDeclAsVector(ASTREE *ast, char *text);

void checkFuncCall(ASTREE *ast, char *text);

void checkUtilization(ASTREE *root, ASTREE *rootAux);

int checkTwoRetTypes(int ret1, int ret2, int operation);

int checkRetType(ASTREE *node, int ptr);

int analyzeExpression(ASTREE *node);

int checkParamsFuncAux(ASTREE *ast, ASTREE *node);

int checkParamsFunc(ASTREE *ast, ASTREE *filho);

void checkUndeclared();

int getErrorNumber();

#endif