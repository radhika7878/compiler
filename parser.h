//Batch 77
//Radhika Pasari 2012B4A7445P
//Pankhuri Kumar 2012B4A7662P
#ifndef _PARSER_H
#define _PARSER_H
#include "parserDef.h"

#define N_NT 51
#define N_T 53
#define N_R 92
#define BSIZE 2

#define hashsize 11

TypeNode* searchSymbolTable(int bin, char key[], char item[]);
int hashfunction(char value[]);
void displayHashTable();
void createParseTable(node* g[], node* fi[], node* fo[], int parsetable[N_NT][N_T]);
child* parseInputSourceCode(node* g[], node* fi[], node* fo[], char* testCaseFile, int parsetable[N_NT][N_T], int op);
void printParseTree(child* root);

#endif