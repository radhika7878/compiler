//Batch 77
//Radhika Pasari 2012B4A7445P
//Pankhuri Kumar 2012B4A7662P
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "parserDef.h"
#include "lexer.h"
#include "lexerDef.h"
#include "semantics.h"
#include "semanticsDef.h"
#include "codegen.h"

char *nonterminals[] = { "<program>", "<mainFunction>", "<otherFunctions>", "<function>", "<input_par>", "<output_par>", "<parameter_list>", "<dataType>", "<primitiveDatatype>", "<constructedDatatype>", "<remaining_list>", "<stmts>", "<typeDefinitions>", "<typeDefinition>", "<fieldDefinitions>", "<fieldDefinition>", "<moreFields>", "<declarations>", "<declaration>", "<global_or_not>", "<otherStmts>", "<stmt>", "<assignmentStmt>", "<singleOrRecId>", "<new_24>", "<funCallStmt>", "<outputParameters>", "<inputParameters>", "<iterativeStmt>", "<conditionalStmt>", "<elsePart>", "<ioStmt>", "<allVar>", "<booleanExpression>", "<var>", "<logicalOp>", "<relationalOp>", "<returnStmt>", "<optionalReturn>", "<idList>", "<more_ids>", "<arithmeticExpression>", "<expPrime>", "<term>", "<termPrime>", "<factor>", "<highOperators>", "<lowOperators>", "<all>", "<new_25>", "<new_26>"};
char* terminals[] = {"$", "TK_ASSIGNOP", "TK_FIELDID", "TK_ID", "TK_NUM", "TK_RNUM", "TK_FUNID", "TK_RECORDID", "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE", "TK_INT", "TK_REAL", "TK_TYPE", "TK_MAIN", "TK_GLOBAL", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT", "TK_OUTPUT", "TK_SEM", "TK_COLON", "TK_DOT", "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF", "TK_THEN", "TK_ENDIF", "TK_READ","TK_WRITE", "TK_RETURN", "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_CALL", "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", "TK_OR", "TK_NOT", "TK_LT", "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE", "TK_COMMA" };

char first_buffer[100] = "";
char second_buffer[100] = "";
int line_number=1;
list *root;
int k=0;
FILE *fp;
int firsttime=0;
HashTab htable[11];
int compiled;

static node* grammar[N_R];
static node* first[N_NT];
static node* follow[N_NT];

/* Main driver function
 * displays current status of the compiler and relevant options to run it
 * input: testcaseX.txt, the source code file and code.asm, where NASM code is stored
 */
int main(int argc, char *argv[]) {
	int parsetable[N_NT][N_T];
	char *testCaseFile = argv[1];
	char *outfile = argv[2];
	child* root = (child*)malloc(sizeof(child));
	ASTnode* astroot=(ASTnode*)malloc(sizeof(ASTnode));
	int i,j;

	for(i=0;i<N_NT;i++){
		first[i] =NULL;
		follow[i] = NULL;
	}

	for(i=0;i<N_R;i++)
		grammar[i]=NULL;
	
	FILE *fp1;
	char buff[50];

	fp1 = fopen("grammar.txt","r");
	for(i=0;i<N_R;i++){
		while(1){
			fscanf(fp1,"%s",buff);
			if(strcmp(buff,"0."))
				addnode(grammar,i,buff);
			else 
				break;		
		}
	}
	fclose(fp1);

	fp1 = fopen("first.txt","r");
	for(i=0;i<N_NT;i++){
		while(1){
			fscanf(fp1,"%s",buff);
			if(strcmp(buff,"0."))
				addnode(first,i,buff);
			else 
				break;		
		}
	}
	fclose(fp1);

	fp1 = fopen("follow.txt","r");
	for(i=0;i<N_NT;i++){
		while(1){
			fscanf(fp1,"%s",buff);
			if(strcmp(buff,"0."))
				addnode(follow,i,buff);
			else 
				break;		
		}
	}
	fclose(fp1);

	for(i=0;i<N_NT;i++){
		for(j=0;j<N_T;j++)
			parsetable[i][j] = 100;

	}
	
	int op;
	printf("LEVEL 4: AST/Symbol Table/Type Checking/Semantic Rules modules work, Code Generation partially implemented.\n");
	do {
		printf("Options:\n");
		printf("1. Display all tokens\n2. Verify Syntatic Correctness of Program and Print Parse Tree\n3. Print Abstract Syntax Tree\n4. Display compression percentage\n5. Print Symbol Table\n6. Front End Compilation\n7. Produce NASM code\n8. Exit\nYour Choice: ");
		scanf("%d", &op);
		switch (op) {
			case 1: displayFromLexer(testCaseFile,op); break;
			case 2: createParseTable(grammar, first, follow, parsetable);
					root = parseInputSourceCode(grammar, first, follow, testCaseFile, parsetable,op);
					if(compiled==0)
						exit(-1); 
					printParseTree(root);
					break;
			case 3: astroot = constructAST(root);
					printast(astroot);
					break;
			case 4: compresspercent(root,astroot);
					break;
			case 5: root = parseInputSourceCode(grammar, first, follow, testCaseFile, parsetable,op);
					checkForRecords();
					removeRecords(op);
					displayHashTable();
					for(i=0;i<hashsize;i++){
						htable[i].variables=NULL;
						htable[i].inputpars=NULL;
						htable[i].outputpars=NULL;
					}
					break;
			case 6: root = parseInputSourceCode(grammar, first, follow, testCaseFile, parsetable,op);
					checkForRecords();
					removeRecords(op);
					astroot = constructAST(root);
					loopcheck(astroot);
					parameterchainchecker(astroot);
					typechecker(astroot);
					break;
			case 7: printf("Code Generation is partially completed: Cannot handle records and nested boolean/arithmetic expressions.\n");
					codegenerator(astroot, outfile);
					break;
		}
	}while (op < 8);
}