//Batch 77
//Radhika Pasari 2012B4A7445P
//Pankhuri Kumar 2012B4A7662P
#ifndef _SEMANTICSDEF_H
#define _SEMANTICSDEF_H
#include "parserDef.h"

typedef struct ASTnode{
	int num;
	int term;
	int line;
	char value[31];
	struct ASTnode* next;
	struct ASTnode* kid;
	struct ASTnode* prev;
	struct ASTnode* parent;
}ASTnode;

#endif