//Batch 77
//Radhika Pasari 2012B4A7445P
//Pankhuri Kumar 2012B4A7662P
#ifndef _LEXERDEF_H
#define _LEXERDEF_H

typedef struct tokenInfos {
	char value[31];
	char token[14];
	int linenumber;
} tokenInfo;

typedef struct list {
	char value[21];
	char token[14];
	struct list *next;
} list;

#endif