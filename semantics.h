//Batch 77
//Radhika Pasari 2012B4A7445P
//Pankhuri Kumar 2012B4A7662P
#ifndef _SEMANTICS_H
#define _SEMANTICS_H
#include "semanticsDef.h"

ASTnode* constructAST(child* ptree);
void printast(ASTnode* root);
void loopcheck(ASTnode* root);
void compresspercent(child* parse, ASTnode* ast);
void parameterchainchecker(ASTnode* root);
void typechecker(ASTnode* ast);

#endif