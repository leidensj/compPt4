#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "astree.h"

int erro;

void initializeSemantic(ASTREE *node);

void checkDeclarationFuncParam(ASTREE *node, ASTREE *functionNode);

void checkDeclaration(ASTREE *root);

ASTREE* retrieveVarDeclaration(ASTREE *initialRoot, ASTREE *nodeToRetrieve);

void checkVarDeclAsFunc(ASTREE *ast, char *text);

void checkVarDeclAsVector(ASTREE *ast, char *text);

void checkFuncCall(ASTREE *ast, char *text);

void checkUtilization(ASTREE *root, ASTREE *rootAux);

int checkTwoRetTypes(int ret1, int ret2, int operation);

int checkRetType(ASTREE *node, int ptr);

int analyseExpression(ASTREE *node);

int checkParamsFuncAux(ASTREE *ast, ASTREE *node);

int checkParamsFunc(ASTREE *ast, ASTREE *filho);

void checkUndeclared();

int getErrorNumber();

#endif