//Batch 77
//Radhika Pasari 2012B4A7445P
//Pankhuri Kumar 2012B4A7662P
#ifndef _LEXER_H
#define _LEXER_H
#include "lexerDef.h"

void displayFromLexer(char testCaseFile[],int op);
list* createKeywordList();
int removeComments(FILE *fp);
tokenInfo getNextToken();

#endif