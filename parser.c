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

extern char* terminals[];
extern char* nonterminals[];
extern char first_buffer[100];
extern char second_buffer[100];
extern FILE *fp;
extern HashTab htable[11];
extern int compiled;

//STACK FUNCTIONS
void addnode(node* arr[], int i, char* name) {
	node* temp1 = arr[i];
	if(temp1){
		while(temp1->next!=NULL)
			temp1 = temp1->next;
		node* temp = (node *)malloc(sizeof(node));
		temp->key = (char *)malloc(strlen(name+1));
		strcpy(temp->key,name);
		temp->next = NULL;
		temp1->next = temp;
	}
	else{
		arr[i] = (node *)malloc(sizeof(node));
		arr[i]->key = (char *)malloc(strlen(name+1));
		strcpy(arr[i]->key,name);
		arr[i]->next = NULL;
	}
}

void popstack(stack** top) {
	stack* temp;
	temp = *top;	
	(*top) = (*top)->n;
	free(temp);	
}

void pushstack(stack** top, int val, child* tree) {
	stack* temp;
	temp =(stack*)malloc(sizeof(stack));
	temp->data = val;
	temp->tr = tree;
	temp->n = *top;
	*top = temp;
}

void display(stack** top) {
	stack* temp;
	temp = *top;
	if(temp!=NULL){
		while(temp!=NULL){
			printf("%d\n",temp->data);
			temp = temp->n;
		}
	}
}

//SYMBOL TABLE FUNCTIONS
/* calculates hash value for symbol table
 * input parameter: string to be hashed
 * output parameter: hash value of string
 */
int hashfunction(char value[]) {
	int i = 0;
	int sum = 0;
	while (value[i] != '\0') {
		sum += (int) value[i];
		i++;
	}
	return (sum % hashsize);
}

/* function to insert item into Symbol Table
 * input paramters: hash value of current scope, name of scope (function), tokenInfo of item to be inserted, type (local variable, input parameter, output parameter)
 * output parameters: pointer to inserted node in symbol table
 */
TypeNode* insertInST(int bin, char key[], tokenInfo token, int type) {
	TypeNode *next = NULL;
	TypeNode *prev = NULL;
	TypeNode *last = NULL;
	int f = 0;
	//going to relevant pointer
	switch(type) {
		case 0:	if (htable[bin].variables == NULL) htable[bin].variables = (TypeNode*) malloc(sizeof(TypeNode));
				next = htable[bin].variables; break;
		case 1: if (htable[bin].inputpars == NULL) htable[bin].inputpars = (TypeNode*) malloc(sizeof(TypeNode));
				next = htable[bin].inputpars; break;
		case 2: if (htable[bin].outputpars == NULL) htable[bin].outputpars = (TypeNode*) malloc(sizeof(TypeNode));
				next = htable[bin].outputpars; break;
	}
	//creating new node
	prev = next;
	next = (TypeNode*) malloc(sizeof(TypeNode));
	next->lines = 1;
	strcpy(next->label, token.value);
	strcpy(next->key, key);
	next->linenumber[0] = token.linenumber;
	next->next = NULL;
	//appending to corrent pointer list
	//if first item to be added
	if (!strcmp(prev->label, "")) {
		switch(type) {
			case 0: htable[bin].variables = next; break;
			case 1: htable[bin].inputpars = next; break;
			case 2: htable[bin].outputpars = next; break;
		}
		return next;
	}
	//if linked list already exists, append to end
	while (prev != NULL) {
			f = 1;
			last = prev;
			prev = prev->next;
	}
	if (f == 1) {
		f = 0;
		last->next = next;
	}	
	return next;
}

/* function to add Ffields to record declaration in global scope
 * input parameters: pointer to record definition (global), type of field, and token to other relevant information
 * called when parser encounters a new record definition
 */
void addMember(TypeNode *p, child *sibling, tokenInfo token) {
	TypeNode *mem = p->members;
	TypeNode *next = (TypeNode*) malloc(sizeof(TypeNode));
	strcpy(next->label, token.value);
	next->lines = 1;
	next->linenumber[0] = token.linenumber;
	strcpy(next->type, ((sibling->next)->kid)->value);
	next->isRecord = 0;
	next->members = NULL;
	//if this is first field of record
	if(mem == NULL)
		p->members = next;
	else {
		//else, add record in same order as declaration
		while(mem->next != NULL)
			mem = mem->next;
		mem->next = next;
	}
}

/* function that adds line number wherever an already defined variable is used
 * input parameters: pointer to referenced node, line number of reference
 * assumption: a variable cannot be used more than 10 times in a program
 */
void addLineNumber(TypeNode *p, int line) {
	//assumption that a variable will not be referenced more than 10 times in a program
	if (p->lines > 9) 
		printf("ERROR: Cannot insert more than 10 references to %s.\n", p->label);
	else {
		p->linenumber[p->lines] = line;
		p->lines++;
	}
}

/* searches symbol table to find given item. If not found in current scope, searches global symbol table.
 * input parameters: hash value of current scope, name of current scope, item to be found
 * output parameters: pointer to node, if found. Else, NULL
 */
TypeNode* searchSymbolTable(int bin, char key[], char item[]) {
	TypeNode *p = NULL;
	int i = 0;
	int flag = 0;
	//search all parameters of current scope
	while (i < 3) {
		if (i == 0) 
			p = htable[bin].variables;
		else if (i == 1)
			p = htable[bin].inputpars;
		else
			p = htable[bin].outputpars;
		while (p != NULL) {
			//since each hashtable entry can be for more than one function, match the variable AND the function name (key)
			if ((strcmp(p->label, item) == 0) && (strcmp(p->key, key) == 0)) {
				flag = 1;
				break;
			}
			p = p->next;
		}
		if (flag == 1)
			break;
		i++;
	}
	//flag is set only if found in current scope
	if (flag == 1)
		return p;
	//if the variable was not found in the function, search global scope
	if (p == NULL) {
		int bin2 = hashfunction("global");
		//global I/P and O/P parameters are empty, by design
		p = htable[bin2].variables;
		while(p != NULL) {
			if ((strcmp(p->label, item) == 0) && (strcmp(p->key, "global") == 0)) {
				flag = 1;
				break;
			}
			p = p->next;
		}
	}
	//again, flag set only if node found
	if (flag == 1)
		return p;
	return p;
}

/* function that changes scope (and position in Symbol Table) of variable, if it is declared global.
 * required as Symbol Table is created concurrently with Parse Table, global scope is not known beforehand
 * input parameters: hash value of current scope
 */
void changeToGlobal(int bin) {
	TypeNode *p = htable[bin].variables;
	TypeNode *prev = p;
	int flag = 0;
	int bin2 = hashfunction("global");
	TypeNode *addnew = htable[bin2].variables;
	//the last variable to be added to current scope is the one that has to be changed to global scope
	//this needs to be removed from current scope, prev stores penultimate node
	while (p->next != NULL) {
		flag = 1;
		prev = p;
		p = p->next;
	}
	while (addnew->next != NULL)
		addnew = addnew->next;
	addnew->next = p;
	strcpy(p->key, "global");
	if (flag == 0)
		htable[bin].variables = NULL;
	else
		prev->next = NULL;
}

/* function to return last variable in current scope
 * input parameter: hash value of current scope
 * output parameters: pointer to last node
 */
TypeNode* returnLastVar(int bin) {
	TypeNode *p = htable[bin].variables;
	while(p->next != NULL) 
		p = p->next;
	return p;
}

/* remove last variable in current scope from symbol table
 * input parameters: hash value of current scope, name of item to be removed
 */
void removeFromST(int bin, TypeNode *item) {
	TypeNode *p = htable[bin].variables;
	TypeNode *prev = NULL;
	while (p->next != NULL) {
		prev = p;
		p = p->next;
	}
	prev->next = NULL;
}

/* function to create fields for new declaration of a record, using the global record definition
 * input parameters: newly created node for record, name of record of which it is a type
 * output parameter: returns 0 if record is not yet defined.
 */
int copyMembers(TypeNode *p, char recordname[]) {
	int bin = hashfunction("global");
	TypeNode *mem = NULL;
	TypeNode *newmem = (TypeNode*) malloc(sizeof(TypeNode));
	TypeNode *start = newmem;
	//search if record definition is found in global scope
	TypeNode *record = searchSymbolTable(bin, "global", recordname);
	if (record == NULL) {
		//if not found
		return 0;
	} else {
		//if found, create members for new record, by copying names and types from record declaration
		mem = record->members;
		while (mem != NULL) {
			strcpy(newmem->label, mem->label);
			strcpy(newmem->type, mem->type);
			newmem->isRecord = 0;
			newmem->members = NULL;
			mem = mem->next;
			if (mem != NULL) {
				newmem->next = (TypeNode*) malloc(sizeof(TypeNode));
				newmem = newmem->next;
			}
		}
		p->members = start;
	}
	strcpy(p->type, "");
	while((start->next) != NULL) {
		//storing type of record
		strcat(p->type, start->type);
		strcat(p->type, " x ");
		if ((start->next)->next == NULL)
			break;
		start = start->next;
	}
	strcat(p->type, start->type);
}

/* function to check if any records were declared before record definition, and replace those variables with relevant type and field members
 * if a record is declared before its definition, the record type's name (#marks) is stored in the variable's type (instead of 'real x real x real')
 * this helps finding un-initialised records
 */
void checkForRecords() {
	int bin = hashfunction("global");
	TypeNode *record = htable[bin].variables;
	TypeNode *p = NULL;
	int i;
	//check global scope for all record definitions
	while (record != NULL) {
		if (strcmp(record->type, "recordDef")) {
			record = record->next;
			continue;
		}
		//if definition found, search entire hashtable for un-initialized records of same type
		char *item = record->label;
		int bin2 = 0;
		while (bin2 < hashsize) {
			i = 0;
			while (i < 3) {
				if (i == 0) 
					p = htable[bin2].inputpars;
				else if (i == 1)
					p = htable[bin2].outputpars;
				else
					p = htable[bin2].variables;
				while (p != NULL) {
					// if found, initialize the record with memory and relevant type
					if (strcmp(p->type, item) == 0) {
						copyMembers(p, record->label);
					}
					p = p->next;
				}
				i++;
			}
			bin2++;
		}
		record = record->next;
	}
}

/* function to check if any records remain uninitialised in the symboltable, and remove them. Called ONLY AFTER checkForRecords(), to identify records that have been declared, but not defined
 * input parameters: op which calls the function. if op is 6 (print semantic errors), then errors are displayed. 
 * if op is 5(display symbol table), errors are not displayed, but symboltable is updated
 */
void removeRecords(int op) {
	int bin = 0;
	int i;
	TypeNode *p = NULL;
	TypeNode *prev;
	while (bin < hashsize) {
		i = 0;
		while (i < 3) {
			if (i == 0) 
				p = htable[bin].inputpars;
			else if (i == 1)
				p = htable[bin].outputpars;
			else
				p = htable[bin].variables;
			prev = NULL;
			while (p != NULL) {
				if (p->type[0] == '#' && op == 6) {
					int s = 1;
					printf("Line %d: The type <%s> is undefined.\n", p->linenumber[0], p->type);
					while (s < p->lines) {
						printf("Line %d: The variable <%s> is undefined.\n", p->linenumber[s], p->label);
						s++;
					}
					if (prev == NULL) {
						if (i == 0) 
							htable[bin].inputpars = NULL;
						else if (i == 1)
							htable[bin].outputpars = NULL;
						else
							htable[bin].variables = NULL;
					} else {
						prev->next = p->next;
					}
				}
				prev = p;
				p = p->next;
			}
			i++;
		}
		bin++;
	}
}

/* function to display entire symbol table. Call after checkForRecords() and removeRecords()
 */
void displayHashTable() {
	int i = 0;
	TypeNode *p;
	printf("\nLexeme\t\t\t\t\tType\t\t\t\t\tScope\t\t\t\t\t\tLines\n");
	while (i < hashsize) {
		int k = 0;
		p = htable[i].inputpars;
		if (p == NULL) {
			p = htable[i].outputpars;
			k = 1;
		}
		if (p == NULL) {
			p = htable[i].variables;
			k = 2;
		}
		if (p != NULL) {
			//printf("\n%s:\n", p->key);
			while (k < 3) {
				if (k == 2) {
					p = htable[i].variables;
				}
				else if (k == 0) {
					p = htable[i].inputpars;
				}
				else {
					p = htable[i].outputpars;
				}
				while (p != NULL) {
					printf("%-35s\t%-35s\t%-35s\t\t", p->label, p->type, p->key);
					int s = 0;
					while (s < p->lines-1) {
						printf("%d, ", p->linenumber[s]);
						s++;
					}
					printf("%d\n", p->linenumber[s]);
					p = p->next;
				}
				k++;
			}
		}		
		i++;
	}
}

/* function to create parse table
 */
void createParseTable(node* grammar[N_R], node* first[N_NT], node* follow[N_NT], int parsetable[N_NT][N_T]) {
	int i;
	node *temp1, *temp2, *temp3;
	int row,fi,col,flag,fo;
	char *lhs, *rhs;
	for(i=0;i<N_R;i++){
		temp1 = grammar[i];
		lhs = (char*)malloc(sizeof(temp1->key)+1);
		strcpy(lhs, temp1->key);
		row=0;
		while(strcmp(lhs,nonterminals[row]))
			row++;
		while(temp1->next){
			flag=0;
			temp1=temp1->next;
			rhs = (char*)malloc(sizeof(temp1->key)+1);
			strcpy(rhs, temp1->key);
			if(rhs[0]=='T'){
				col=0;
				while(strcmp(rhs,terminals[col]))
					col++;
				//printf("For %s to %s we add rule %d\n",lhs,rhs,i);
				parsetable[row][col]=i;
			}
			else if(!strcmp(rhs,"eps")){
				flag=1;
				break;
			}
			else{
				fi=0;
				while(strcmp(rhs,nonterminals[fi]))
					fi++;
				temp2=first[fi];
				while(temp2->next){
					temp2=temp2->next;
					rhs = (char*)malloc(sizeof(temp2->key)+1);
					strcpy(rhs, temp2->key);
					if(!strcmp(rhs,"e"))
						flag=1;
					else{
						col=0;
						while(strcmp(rhs,terminals[col]))
							col++;
						//printf("For %s to %s we add rule %d\n",lhs,rhs,i);
						parsetable[row][col]=i;
					}	
				}	
			}
			if(flag==0)
				break;
		}
		if(flag==1){
			fo=0;
			while(strcmp(lhs,nonterminals[fo]))
				fo++;
			temp3 = follow[fo];
			while(temp3->next){
				temp3=temp3->next;
				rhs = (char*)malloc(sizeof(temp3->key)+1);
				strcpy(rhs, temp3->key);
				col=0;
				while(strcmp(rhs,terminals[col]))
					col++;
				//printf("For %s to %s we add rule %d\n",lhs,rhs,i);
				parsetable[row][col]=i;
			}
		}
	}
	free(temp1);
	free(temp2);
	free(temp3);
}

/* function to parse source code, create parse tree and populate symbol table
 */
child* parseInputSourceCode(node* grammar[N_R], node* first[N_NT], node* follow[N_NT], char* testCaseFile, int parsetable[N_NT][N_T], int op) {
	fp = fopen(testCaseFile, "r");

	child* root;
	root = (child *)malloc(sizeof(child));
	root->num=0;
	root->term=0;
	root->rulenumber=100;
	root->next=NULL;
	root->kid=NULL;
	root->prev=NULL;
	root->parent=NULL;
	root->line=0;
	strcpy(root->value,"program");

	stack* top;
	pushstack(&top, 100, NULL);
	pushstack(&top, 0, root);

	int lookahead=1;
	char* token;
	char* key;
	char* recordname;
	int bin, bin2;
	tokenInfo tok;
	int col;
	int comment =0;
	int function_flag = 0;
	int inputflag = 0;
	int outputflag = 0;
	int record_flag = 0;
	int relevantfunction = 0;
	child *ch;


	while(1){
		child* temp3 = (child *)malloc(sizeof(child));
		ch = NULL;
		if(lookahead==1){
			tok = getNextToken(op);
			token = (char *)malloc(strlen(tok.token)+1);
			strcpy(token,tok.token);
			if (!strcmp(token, "TK_INPUT"))
				inputflag = 1;
			if (!strcmp(token, "TK_OUTPUT"))
				outputflag = 1;
			if (inputflag == 1 && !strcmp(token, "TK_SQR"))
				inputflag = 0;
			if(outputflag == 1 && !strcmp(token, "TK_SQR"))
				outputflag = 0;
			//printf("%s\n",token);
			if(!strcmp(token, "ERR")){
				compiled=0;
				printf("Compilation failed.\n");
				return root;
			}
			if(!strcmp(token,"EOF")){
				break;
			}
			if(!strcmp(token,"TK_COMMENT"))
				comment=1;
			else{
				col=0;
				while(strcmp(token, terminals[col])){
					col++;
				}
			}
			lookahead=0;
		}
		if(comment==1){
			lookahead=1;
			comment=0;
			continue;
		}
		//row = stack's top element
		int row = top->data;
		int ifterm = (top->tr)->term;
		//if top is terminal, check with lexer's token, else error
		if(ifterm==1){
			int i;
			if(!strcmp(token, terminals[row])){
				ch = top->tr;
				child* treepointer=top->tr;
				strcpy(treepointer->value, tok.value);
				lookahead=1;
				popstack(&top);
			} else{
				compiled=0;
				printf("ERROR: The token %s for lexeme %s does not match at line %d. The expected token here is %s.\n ",token, tok.value, tok.linenumber+1, terminals[row] );
				return root;
			}

		} else{
			//top of stack is non term
			//if no rule found, error
			if(parsetable[row][col]==100){
				compiled=0;
				printf("ERROR: The token %s for lexeme %s does not match at line %d. The expected token here is/are ",token, tok.value, tok.linenumber+1);
				int i=0;
				for(i=0;i<N_T;i++){
					if(parsetable[row][i] != 100){
						printf("%s ",terminals[i]);
					}
				}
				printf("\n");
				return root;
			} 
			else{
				int ruleno= parsetable[row][col];
				node* temp = grammar[ruleno];
				//temp = RHS of req grammar rule
				temp = temp->next;
				//temp2 = corresponding tree node of top of stack
				child* temp2 = top->tr;
				//strcpy(temp2->value,tok.value);
				//strcpy(temp3->value,tok.value);
				if((temp->key)[0] == 'T'){
					temp3->term =1;
					temp3->line=tok.linenumber;
					int id=0;
					while(strcmp(temp->key, terminals[id]))
						id++;
					temp3->num=id;
				} else if(!strcmp((temp->key), "eps")){
					temp3->term=-1;
					temp3->num=-1;
					temp3->line=tok.linenumber;
					temp3->rulenumber=100;
					temp3->kid = NULL;
					temp3->prev = NULL;
					temp3->next = NULL;
					temp3->parent=temp2;
					temp2->rulenumber=ruleno;
					temp2->kid = temp3;
					ch = top->tr;
					popstack(&top);
					continue;
				} else {
					temp3->term =0;
					temp3->line=tok.linenumber;
					int id=0;
					while(strcmp(temp->key, nonterminals[id]))
						id++;
					temp3->num=id;
				}
				temp3->rulenumber=100;
				temp3->kid = NULL;
				temp3->prev = NULL;
				temp3->next = NULL;
				temp3->parent=temp2;
				temp2->rulenumber=ruleno;
				temp2->kid = temp3;
				//creating child layer for given non term
				temp=temp->next;
				while(temp){
					child* temp4 = (child *)malloc(sizeof(child));
					temp4->line=tok.linenumber;
					temp4->parent=temp2;
					temp4->rulenumber=100;
					temp4->prev = temp3;
					temp4->kid = NULL;
					temp4->next=NULL;
					if((temp->key)[0] == 'T'){
						temp4->term =1;
						int id=0;
						while(strcmp(temp->key, terminals[id]))
							id++;
						temp4->num=id;
						temp3->next = temp4;
					}
					else{
						temp4->term =0;
						int id=0;
						while(strcmp(temp->key, nonterminals[id]))
							id++;
						temp4->num=id;
						temp3->next = temp4;
					}
					temp = temp->next;
					temp3 = temp3->next;
				}
				popstack(&top);
				while(temp3){
					pushstack(&top, temp3->num, temp3);
					temp3=temp3->prev;
				}
			}
		}
		//adding to ST begins here
		child* par = NULL;
		TypeNode* inserted = NULL;
		if (lookahead == 1 && (op == 5 || op == 6)) {
			par = ch->parent;
			//printf("%s", token);
			//if token is function, add function name to global scope
			if ((!strcmp(token, "TK_FUNID") && par->rulenumber == 4) || (!strcmp(token, "TK_MAIN") && par->rulenumber == 1)) {
				function_flag = 1;
				bin = hashfunction("global");
				TypeNode *p = searchSymbolTable(bin, "global", tok.value);
				if (p == NULL) {
					inserted = insertInST(bin, "global", tok, 0);
					strcpy(inserted->type, "function");
					key = (char*)malloc(strlen(tok.value) + 1);
					strcpy(key, tok.value);
					bin = hashfunction(key);
					TypeNode *next = htable[bin].variables;
					while (next != NULL && strcmp(next->key, key) != 0) {
						bin++;
						if (bin == hashsize) bin = 0;
					}
					continue;
				} else {
					//if function is already defined
					relevantfunction = 1;
					if (op == 6) {
						printf("Line %d: The function <%s> is already defined. All variables till end of function are not added to Symbol Table.\n", tok.linenumber, tok.value);
					}
				}
			} else if (!strcmp(token, "TK_RECORDID") && par->rulenumber == 19 && relevantfunction == 0) {
				//if token is record, add definition to global scope
				record_flag = 1;
				recordname = (char*)malloc(strlen(tok.value) + 1);
				strcpy(recordname, tok.value);
				bin2 = hashfunction("global");
				inserted = insertInST(bin2, "global", tok, 0);
				strcpy(inserted->type, "recordDef");
				inserted->isRecord = 1;
			}
		}
		if (record_flag == 1 && ch != NULL && (op == 5 || op == 6) && relevantfunction == 0) {
			//adding field members to record definition in global scope
			if (!strcmp(token, "TK_ENDRECORD")) {
				record_flag = 0;
			} else if (!strcmp(token, "TK_FIELDID") && par->rulenumber == 21) {
				TypeNode *p = searchSymbolTable(bin2, "global", recordname);
				addMember(p, par->kid, tok);
			}
		}
		if (function_flag == 1 && ch != NULL && (op == 5 || op == 6) && relevantfunction == 0) {
			//adding all variables declared in a function, to the relevant function scope entry in symbol table
			child* par2 = par->parent;
			if (!strcmp(token, "TK_END")) {
				// if token is TK_END, this function scope ends
				function_flag = 0;
			} else if ((!strcmp(token, "TK_ID")) || (!strcmp(token, "TK_FUNID"))) {
				//if new variable/function is found, check if already exists in symbol table
				TypeNode *p = searchSymbolTable(bin, key, tok.value);
				if (!strcmp(token, "TK_FUNID") && !strcmp(tok.value, key) && op == 6)
					printf("Line %d: Recursive calls to function %s() are not allowed.\n", tok.linenumber, tok.value);
				if (p != NULL) {
					//if found in ST, and this is a declaration
					if (par->rulenumber == 26) {
						//if scope of new variable and found variable is not same (found variable can be global), add variable to current scope
						if (strcmp(p->key, key) != 0)
							insertInST(bin, key, tok, 0);
						//if scopes match, this is a re-declaration, throw error for relevant option
						else if (strcmp(p->key, key) == 0 && op == 6)
							printf("Line %d: The identifier <%s> is declared more than once.\n", tok.linenumber, p->label);
					}
					else
						// if node is found in symbol table, but this is not a declaration, add line number where variable was referenced
						addLineNumber(p, tok.linenumber);
				} else {
					//if not found in ST
					child* sibling = par->kid;
					int y = -1;
					if (par->rulenumber == 26) {
						//type declarations
						inserted = insertInST(bin, key, tok, 0);
						if (sibling->term == 1 && sibling->num == 40) {
							//if record type definition statement
							sibling = sibling->next;
							y = copyMembers(inserted, sibling->value);
							if (y == 0) {
								//y is 0 means record is of a type whose declaration is not yet found. Copy recordname into type, which will be replaced when record in defined
								strcpy(inserted->type, sibling->value);
							}
							inserted->isRecord = 1;
						} else {
							// if declaration statements
							while (!(sibling->term == 0 && sibling->num == 7))
								sibling = sibling->next;
							sibling = (sibling->kid)->kid;
							if (sibling->term == 1 && sibling->num == 40) {
								//if record
								sibling = sibling->next;
								y = copyMembers(inserted, sibling->value);
								if (y == 0)
									strcpy(inserted->type, sibling->value);
								inserted->isRecord = 1;
							} else {
								strcpy(inserted->type, sibling->value);
								inserted->isRecord = 0;
								inserted->members = NULL;
							}
						}
					} else if ((par->rulenumber == 8 && par2->rulenumber == 5) || inputflag == 1) {
						//input parameter
						inserted = insertInST(bin, key, tok, 1);
						while (!(sibling->term == 0 && sibling->num == 7))
							sibling = sibling->next;
						sibling = (sibling->kid)->kid;
						if (sibling->term == 1 && sibling->num == 40) {
							//if record
							sibling = sibling->next;
							y = copyMembers(inserted, sibling->value);
							if (y == 0) {
								//y is 0 means record is of a type whose declaration is not yet found. Copy recordname into type, which will be replaced when record in defined
								strcpy(inserted->type, sibling->value);
							}
							inserted->isRecord = 1;
						} else {
							strcpy(inserted->type, sibling->value);
							inserted->isRecord = 0;
							inserted->members = NULL;
						}
					} else if ((par->rulenumber == 8 && par2->rulenumber == 6) || outputflag == 1) {
						//output parameter
						//printf("%s inserted 2.", tok.value);
						inserted = insertInST(bin, key, tok, 2);
						while (!(sibling->term == 0 && sibling->num == 7))
							sibling = sibling->next;
						sibling = (sibling->kid)->kid;
						if (sibling->term == 1 && sibling->num == 40) {
							sibling = sibling->next;
							y = copyMembers(inserted, sibling->value);
							if (y == 0) {
								//y is 0 means record is of a type whose declaration is not yet found. Copy recordname into type, which will be replaced when record in defined
								strcpy(inserted->type, sibling->value);
							}
							inserted->isRecord = 1;
						} else {
							strcpy(inserted->type, sibling->value);
							inserted->isRecord = 0;
							inserted->members = NULL;
						}
					} else {
						//if not found in symbol table, and these are not declaration statements
						if (!strcmp(tok.token, "TK_FUNID") && op == 6)
							printf("Line %d: The function %s() is undefined.\n", tok.linenumber, tok.value);
						if (!strcmp(tok.token, "TK_ID") && op == 6)
							printf("Line %d: The variable <%s> is not declared.\n", tok.linenumber, tok.value);
					}
				}
			} else if ((!strcmp(token, "TK_GLOBAL")) && par2->rulenumber == 26) {
				// if variable is declared global
				int bin2 = hashfunction("global");
				//find last variable added to current scope
				TypeNode *p1 = returnLastVar(bin);
				//search for same variable in global scope
				TypeNode *p2 = searchSymbolTable(bin2, "global", p1->label);
				//if p2 is null, this is a new global variable
				if (p2 == NULL)
					changeToGlobal(bin);
				else {
					//if p2 is not null, this variable is already defined to be global
					if (op == 6)
						printf("Line %d: Global variable <%s> cannot be declared more than once.\n", tok.linenumber, p2->label);
					removeFromST(bin, p1);
				}
				//printf("%s changed -1.", ch);
			}
		} 
		if (!strcmp(token, "TK_END") && relevantfunction == 1) {
			// end of repeating function
			relevantfunction = 0;
		} //adding to ST ends here
	}

	if(op==2){
		int rem = top->data;
		if(rem==100){
			compiled=1;
			printf("Compilation successful!\n");
			return root;
		} 
		else {
			compiled=0;
			printf("Program not complete. Compilataion failed.\n");
			if((top->tr)->term){
				printf("Expected token is %s\n", terminals[rem]);
			} 
			else{
				printf("Expected tokens is/are: ");
				int i=0;
				for(i=0;i<N_T;i++){
					if(parsetable[rem][i] != 100){
						printf("%s ",terminals[i]);
					}
				}
				printf("\n");
			}
		}
	}
	return root;
}

/* function for dfs traversal and printing of parse tree
 */
void dfs(child* node){
	if(node->term){
		printf("%-25s",node->value);
		printf("%d.",node->line);
		printf("%-25s",terminals[node->num]);
		if(node->num==4 || node->num==5){
			printf("%-5s",node->value);
		}
		else
			printf("---- ");
		if(!(node->num==0 && node->term==0))
			printf("%-25s",nonterminals[node->parent->num]);
		printf("YES ");
		printf("\n");
	}
	else{
		printf("------------------------ ");
		printf("%d.",node->line);
		printf("%-25s",nonterminals[node->num]);
		printf("---- ");
		if(!(node->num==0 && node->term==0))
			printf("%-25s",nonterminals[node->parent->num]);
		else 
			printf("ROOT                       ");
		printf("NO  ");
		printf("%s\n",nonterminals[node->num]);
	}
	if(node->kid)
		dfs(node->kid);
	if(node->next)
		dfs(node->next);
	return;
}

/*driver function for dfs traversal of parse tree
 */
void printParseTree(child* root){
	dfs(root);
	return;
}

