//Batch 77
//Radhika Pasari 2012B4A7445P
//Pankhuri Kumar 2012B4A7662P
#ifndef _PARSERDEF_H
#define _PARSERDEF_H
#include "lexerDef.h"
typedef struct node{
	char* key;
	struct node *next;
}node;

typedef struct child{
	int num;
	int term;
	int line;
	int rulenumber;
	char value[31];
	struct child* next;
	struct child* prev;
	struct child* kid;
	struct child* parent;
}child;

typedef struct stack{
	int data;
	child* tr;
	struct stack *n;
}stack;


typedef struct TypeNode {
	char label[100];
	char value[30];
	char type[40];
	char key[31];
	int linenumber[10];
	int lines;
	int isRecord;
	struct TypeNode *members;
	struct TypeNode *next;
} TypeNode;

typedef struct HashTab {
	TypeNode *variables;
	TypeNode *inputpars;
	TypeNode *outputpars;
} HashTab;

#endif
