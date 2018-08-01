//Batch 77
//Radhika Pasari 2012B4A7445P
//Pankhuri Kumar 2012B4A7662P
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parserDef.h"
#include "parser.h"
#include "lexerDef.h"
#include "lexer.h"
#include "semantics.h"
#include "semanticsDef.h"
#include "codegen.h"

extern char* terminals[];
extern char* nonterminals[];

/*function to generate code for declaration statements
 *INPUT parameters: pointer to declaration statement in the abstract syntax tree, name of file in which code is to be printed
 *reserves one word for each variable of integer type, cannot handle records
 */ 

void codedec(ASTnode* root,FILE* fp){
	ASTnode* temp=root;
	if(temp->kid->next)
		temp=temp->kid->next;
	fprintf(fp,"	%s: resw 1\n",temp->value);
	while(temp->next){
		if(temp->next->num==16){
			temp=temp->next;
			continue;
		}
		temp=temp->next->next;
		fprintf(fp,"	%s: resw 1\n",temp->value);
	}
	return;
}

/*function to generate code for assignment, iterative, conditional and io statements
 *INPUT parameters: pointer to particular statement in the abstract syntax tree, name of file in which code is to be printed
 *cannot handle recursive boolean/arithmetic expressions
 */ 

void codeother(ASTnode* root, FILE* fp){
	if(root->num==22){
		ASTnode* temp=root;
		temp=temp->kid;
		char val[31];
		strcpy(val,temp->value);
		temp=temp->next->next;
		if(temp->kid){
			if(!(temp->kid->kid) && !(temp->kid->next->kid)){
				if(temp->num==35)
					fprintf(fp,"ADD ");
				if(temp->num==36)
					fprintf(fp,"SUB ");
				if(temp->num==37)
					fprintf(fp,"MUL ");
				if(temp->num==38)
					fprintf(fp,"DIV ");
				temp=temp->kid;
				if(temp->num==3)
					fprintf(fp,"[%s],",temp->value);
				else
					fprintf(fp,"%s",temp->value);
				temp=temp->next;
				if(temp->num==3)
					fprintf(fp,"[%s]\n",temp->value);
				else
					fprintf(fp,"%s\n",temp->value);
			}
			else{
			printf("Nested arithmetic expressions cannot be handled\n");
			//nested arithmetic expression
			}

		}
		else{
			int i = atoi(temp->value);
			fprintf(fp,"mov ax,%d\n",i);
			fprintf(fp,"mov [%s],ax\n",val);
		}
		//assignment
	}
	if(root->num==28){
		ASTnode* temp=root;
		temp=temp->kid->next;
		if(!(temp->kid->kid) && !(temp->kid->next->kid)){
			temp=temp->kid;
			fprintf(fp,"LOOP: cmp " );
			if(temp->num==3)
				fprintf(fp,"[%s],",temp->value);
			else
				fprintf(fp,"%s",temp->value);
			temp=temp->next;
			if(temp->num==3)
				fprintf(fp,"[%s]\n",temp->value);
			else
				fprintf(fp,"%s\n",temp->value);
			temp=temp->parent;
			if(temp->num==46)
				fprintf(fp, "JGE LABEL\n");
			if(temp->num==47)
				fprintf(fp, "JG LABEL\n");
			if(temp->num==48)
				fprintf(fp,"JNE LABEL\n");
			if(temp->num==49)
				fprintf(fp,"JLE LABEL\n");
			if(temp->num==50)
				fprintf(fp, "JL LABEL\n");
			if(temp->num==51)
				fprintf(fp,"JE LABEL\n");
			while(temp->next){
				temp=temp->next;
				codeother(temp,fp);
			}
			fprintf(fp,"JMP LOOP\n");
			fprintf(fp,"LABEL:\n");
		}
		else{
			printf("Nested boolean expressions cannot be handled.\n");
		}

		//iterative
	}
	if(root->num==29){
		//conditional
		ASTnode* temp=root;
		temp=temp->kid;
		ASTnode* ifnode=temp;
		temp=temp->kid;
		if(!(temp->kid->kid) && !(temp->kid->next->kid)){
			temp=temp->kid;
			fprintf(fp,"cmp " );
			if(temp->num==3)
				fprintf(fp,"[%s],",temp->value);
			else
				fprintf(fp,"%s",temp->value);
			temp=temp->next;
			if(temp->num==3)
				fprintf(fp,"[%s]\n",temp->value);
			else
				fprintf(fp,"%s\n",temp->value);
			temp=ifnode->kid;
			if(temp->num==46)
				fprintf(fp, "JGE LABEL\n");
			if(temp->num==47)
				fprintf(fp, "JG LABEL\n");
			if(temp->num==48)
				fprintf(fp,"JNE LABEL\n");
			if(temp->num==49)
				fprintf(fp,"JLE LABEL\n");
			if(temp->num==50)
				fprintf(fp, "JL LABEL\n");
			if(temp->num==51)
				fprintf(fp,"JE LABEL\n");
			while(temp->next->num != 42){
				temp=temp->next;
				codeother(temp,fp);
			}
			temp=temp->next;
			fprintf(fp, "LABEL:\n");
			while(temp->next){
				temp=temp->next;
				codeother(temp,fp);
			}
		}
		else{
			printf("Nested boolean expressions cannot be handled\n");
			//nested boolean expression
		}
	}


	
	if(root->num==31){
		ASTnode* temp=root;
		temp=temp->kid;
		if(temp->num==32){
			//read
			temp=temp->kid;
			fprintf(fp,"mov ecx, %s\n",temp->value);
			fprintf(fp,"mov edx, buf_len\n");
			fprintf(fp,"mov ebx, 0\n");
			fprintf(fp,"mov eax, 3\n");
			fprintf(fp,"int 80H\n");

		}
		else{
			temp=temp->kid;
			fprintf(fp,"mov ecx, %s\n",temp->value);
			fprintf(fp,"mov edx, buf_len\n");
			fprintf(fp,"mov ebx, 1\n");
			fprintf(fp,"mov eax, 4\n");
			fprintf(fp,"int 80H\n");
			//write
		}
		//io
	}
}

/*driver function to generate code
 *INPUT parameters: root of abstract syntax tree, name of file in which code is to be printed
 *calls codeother() function to generate code for statements under <otherStmts>
 *calls codedec() function to generate code for statements under <declaration>
 */ 
void codegenerator(ASTnode* root, char* outfile){
	FILE* fp = fopen(outfile,"w");

	ASTnode* temp=root;
	temp=temp->kid;//<main>
	temp=temp->kid->next;//stmts
	temp=temp->kid->next;//declarations;
	ASTnode* treepos=temp;

	temp=temp->kid;
	fprintf(fp,"SECTION .bss\n");	
	while(1){
			
		if(temp){
			codedec(temp,fp);
			temp=temp->next;
			continue;
		}
		break;
	}
	fprintf(fp,"	buf: resw 1\n");
	fprintf(fp,"	buf_len: equ $-buf\n");
	fprintf(fp,";\n");
	temp=treepos->next;//at other stmts
	treepos=temp;

	fprintf(fp, "SECTION .text\n");
	fprintf(fp, "global main\n");
	fprintf(fp, "main:\n");
	temp=temp->kid;
	while(1){		
		if(temp){
			codeother(temp,fp);
			temp=temp->next;
			continue;
		}
		break;
	}

	fclose(fp);
}
