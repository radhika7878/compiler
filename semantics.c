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

extern char* terminals[];
extern char* nonterminals[];
extern HashTab htable[11];

ASTnode* abstractae(child* parse, ASTnode** cur);
ASTnode* abstractterm(child* parse, ASTnode** cur);
ASTnode* abstractexp(child* parse, ASTnode** cur);

//AST CREATION FUNCTIONS
void addkid(ASTnode** node,child* temp){
	ASTnode* root=(ASTnode*)malloc(sizeof(ASTnode));
	root->num=temp->num;
	root->term=temp->term;
	root->line=temp->line;
	strcpy(root->value, temp->value);
	root->next=NULL;
	root->kid=NULL;
	root->prev=NULL;
	root->parent=(*node);
	(*node)->kid=root;
}

void addsibling(ASTnode** node,child* temp){
	ASTnode* root=(ASTnode*)malloc(sizeof(ASTnode));
	root->num=temp->num;
	root->term=temp->term;
	root->line=temp->line;
	strcpy(root->value, temp->value);
	root->next=NULL;
	root->kid=NULL;
	root->prev=*node;
	root->parent=(*node)->parent;
	(*node)->next=root;
}
void addkid2(ASTnode** node,int num, int term){
	ASTnode* root=(ASTnode*)malloc(sizeof(ASTnode));
	root->num=num;
	root->term=term;
	root->next=NULL;
	root->kid=NULL;
	root->prev=NULL;
	root->parent=(*node);
	(*node)->kid=root;
}

void addsibling2(ASTnode** node,int num, int term){
	ASTnode* root=(ASTnode*)malloc(sizeof(ASTnode));
	root->num=num;
	root->term=term;
	root->next=NULL;
	root->kid=NULL;
	root->prev=*node;
	root->parent=(*node)->parent;
	(*node)->next=root;
}

ASTnode* abstractterm(child* parse, ASTnode** cur){
	child* temp=parse;
	temp=temp->kid->kid;
	(*cur)->num=temp->num;
	(*cur)->term=temp->term;
	(*cur)->line=temp->line;
	strcpy((*cur)->value,temp->value);
	temp=temp->parent->parent->prev;
	child* t = temp;

	ASTnode* temp1;
	if(temp->rulenumber==81){
		temp=temp->kid->next;
		ASTnode* newcur=(ASTnode*)malloc(sizeof(ASTnode));
		(*cur)->kid=newcur;
		newcur->parent=(*cur);
		temp1=abstractae(temp,&newcur);
	}
	else{
		temp=temp->kid->kid;
		addkid(cur,temp);
		temp1=(*cur)->kid;
		if(temp->next && temp->next->rulenumber==91){
			temp=temp->next->kid->kid;
			addsibling(&temp1,temp);
			temp1=temp1->next;
		}
	}

	temp=t;
	temp=temp->next;
	temp=temp->kid->next->next;
	if(temp->rulenumber==79){
		ASTnode* newcur=(ASTnode*)malloc(sizeof(ASTnode));
		temp1->next=newcur;
		newcur->parent=temp1->parent;
		ASTnode* temp2=abstractterm(temp,&newcur);
	}
	else{
		temp=temp->prev;
		if(temp->rulenumber==81){
		temp=temp->kid->next;
		ASTnode* newcur=(ASTnode*)malloc(sizeof(ASTnode));
		temp1->next=newcur;
		newcur->parent=temp1->parent;
		temp1=abstractae(temp,&newcur);
		}
		else{
			temp=temp->kid->kid;
			addsibling(&temp1,temp);
			temp1=temp1->next;
			if(temp->next && temp->next->rulenumber==91){
				temp=temp->next->kid->kid;
				addsibling(&temp1,temp);
				temp1=temp1->next;
			}
		}
	}
	return (*cur);
}

ASTnode* abstractexp(child* parse, ASTnode** cur){
	child* temp=parse;
	temp=temp->kid->kid;
	(*cur)->num=temp->num;
	(*cur)->term=temp->term;
	strcpy((*cur)->value,temp->value);
	temp=temp->parent->parent->prev;
	temp=temp->kid->next;
	child* t = temp;
	ASTnode* temp1;
	if(temp->rulenumber==79){
		ASTnode* newcur=(ASTnode*)malloc(sizeof(ASTnode));
		(*cur)->kid=newcur;
		newcur->parent=(*cur);
		temp1=abstractterm(temp,&newcur);
	}
	else{
		temp=temp->prev;
		if(temp->rulenumber==81){
			temp=temp->kid->next;
			ASTnode* newcur=(ASTnode*)malloc(sizeof(ASTnode));
			(*cur)->kid=newcur;
			newcur->parent=(*cur);
			temp1=abstractae(temp,&newcur);
		}
		else{
			temp=temp->kid->kid;
			addkid(cur,temp);
			temp1=(*cur)->kid;
			if(temp->next && temp->next->rulenumber==91){
				temp=temp->next->kid->kid;
				addsibling(&temp1,temp);
				temp1=temp1->next;
			}
		}
	}
	temp=t;
	temp=temp->parent->next;
	temp=temp->kid->next;
	temp=temp->next;
	if(temp->rulenumber==77){
		temp=temp->prev;
		temp=temp->kid->next;
		if(temp->rulenumber==79){
		ASTnode* newcur=(ASTnode*)malloc(sizeof(ASTnode));
		temp1->next=newcur;		
		newcur->parent=temp1->parent;
		temp1=abstractterm(temp,&newcur);
		}
		else{
			temp=temp->prev;
			if(temp->rulenumber==81){
				temp=temp->kid->next;
				ASTnode* newcur=(ASTnode*)malloc(sizeof(ASTnode));
				temp1->next=newcur;
				newcur->parent=temp1->parent;
				temp1=abstractae(temp,&newcur);
			}
			else{
				temp=temp->kid->kid;
				addsibling(&temp1,temp);
				temp1=temp1->next;
				if(temp->next && temp->next->rulenumber==91){
					temp=temp->next->kid->kid;
					addsibling(&temp1,temp);
					temp1=temp1->next;
				}
			}
		}
		
	}
	else{
		ASTnode* newcur=(ASTnode*)malloc(sizeof(ASTnode));
		temp1->next=newcur;
		newcur->parent=temp1->parent;
		ASTnode* temp2=abstractexp(temp,&newcur);
	}
	return (*cur);
}

ASTnode* abstractae(child* parse, ASTnode** cur){
	child* temp=parse->kid->next;
	if(temp->rulenumber==76){
		ASTnode* temp1=abstractexp(temp,cur);
		return temp1;
	}
	else{
		temp=temp->prev->kid->next;
		if(temp->rulenumber==79){
			ASTnode* temp1=abstractterm(temp,cur);
			return temp1;
		}
	}
	temp=parse->kid->kid->kid->kid;
	(*cur)->num=temp->num;
	(*cur)->term=temp->term;
	(*cur)->line=(*cur)->term;
	strcpy((*cur)->value,temp->value);

	if(temp->next && temp->next->rulenumber==91){
		temp=temp->next->kid->kid;
		addsibling(cur,temp);
		(*cur)=(*cur)->next;
	}
	return (*cur);
}

ASTnode* abstractbool(child* parse, ASTnode** cur, int kid){
	ASTnode* root=(ASTnode*)malloc(sizeof(ASTnode));
	if(kid==1){
		root->parent=(*cur);
		(*cur)->kid=root;
	}
	else if(kid==0){
		root->prev=(*cur);
		root->parent=(*cur)->parent;
		(*cur)->next=root;
	}
	if(parse->rulenumber==56){
		parse=parse->kid->next->kid;
		root->num=parse->num;
		root->term=parse->term;
		root->line=parse->line;
		strcpy(root->value,parse->value);
		root->next=NULL;
		root->kid=NULL;
		root->prev=NULL;
		parse=parse->parent->prev->kid;
		//if type==type
		addkid(&root,parse);
		ASTnode* temp=root->kid;
		parse=parse->parent->next->next->kid;
		addsibling(&temp,parse);	
	}
	else if(parse->rulenumber==57){
		parse=parse->kid;
		root->num=parse->num;
		root->term=parse->term;
		root->line=parse->line;
		strcpy(root->value,parse->value);
		root->kid=NULL;
		root->prev=NULL;
		root->next=NULL;
		parse=parse->next->next;
		ASTnode* temp=abstractbool(parse,&root,1);
	}
	else if(parse->rulenumber==55){
		parse = parse->kid->next->next->next->kid;
		root->num=parse->num;
		root->term=parse->term;
		root->line=parse->line;
		strcpy(root->value,parse->value);
		root->next=NULL;
		root->kid=NULL;
		root->prev=NULL;
		parse=parse->parent->prev->prev;
		ASTnode* temp1=abstractbool(parse,&root,1);
		parse=parse->next->next->next->next;
		ASTnode* temp=abstractbool(parse,&temp1,0);
	}
	

	return root;
}

ASTnode* abstractstmt(child* parse, ASTnode** cur, int kid){
	ASTnode* root=(ASTnode*)malloc(sizeof(ASTnode));
	child* temp=parse->kid;
	root->num=temp->num;
	root->term=temp->term;
	root->line=temp->line;
	strcpy(root->value,temp->value);
	root->next=NULL;
	root->kid=NULL;
	root->prev=NULL;
	if(kid==1){
		root->parent=(*cur);
		(*cur)->kid=root;
	}
	else if(kid==0){
		root->prev=(*cur);
		root->parent=(*cur)->parent;
		(*cur)->next=root;
	}
	ASTnode* ASTtemp=root;

	if(temp->parent->rulenumber==31){
		temp=temp->kid->kid;
		addkid(&ASTtemp,temp);
		ASTtemp=ASTtemp->kid;
		temp=temp->next;
		if(temp->rulenumber==39){
			temp=temp->kid->next;
			addsibling(&ASTtemp,temp);
			ASTtemp=ASTtemp->next;
			temp=temp->parent;
		}
		temp=temp->parent;
		temp=temp->next;
		addsibling(&ASTtemp,temp);
		ASTtemp=ASTtemp->next;
		temp=temp->next;
		addsibling(&ASTtemp,temp);
		ASTtemp=ASTtemp->next;
		ASTtemp=abstractae(temp,&ASTtemp);
	}
	else if(temp->parent->rulenumber==32){
		temp=temp->kid;
		addkid(&ASTtemp,temp);
		ASTtemp=ASTtemp->kid;
		temp=temp->next->next;
		ASTtemp=abstractbool(temp,&ASTtemp,0);
		temp=temp->next->next;
		ASTtemp=abstractstmt(temp,&ASTtemp,0);					
	}
	else if(temp->parent->rulenumber==33){
		temp=temp->kid;//at if
		addkid(&ASTtemp,temp);//added if
		ASTtemp=ASTtemp->kid;
		temp=temp->next->next;//at boolean
		ASTtemp=abstractbool(temp,&ASTtemp,1);
		temp=temp->next->next->next;//at stmt
		ASTtemp=abstractstmt(temp,&ASTtemp,0);
		temp=temp->next->next;//at elsepart
		if(temp->rulenumber==46){
			temp=temp->kid;//at else
			addsibling(&ASTtemp,temp);
			ASTtemp=ASTtemp->next;
			temp=temp->next;
			ASTtemp=abstractstmt(temp,&ASTtemp,0);
		}					
	}
	else if(temp->parent->rulenumber==34){
		temp=temp->kid;
		addkid(&ASTtemp,temp);
		ASTtemp=ASTtemp->kid;
		temp=temp->next->next;
		if(temp->rulenumber==51 || temp->rulenumber==52){
			temp=temp->kid;
			addkid(&ASTtemp,temp);
			ASTtemp=ASTtemp->kid;
		}
		else{
			temp=temp->kid;
			addkid(&ASTtemp,temp);
			ASTtemp=ASTtemp->kid;
			temp=temp->next;
			if(temp->rulenumber==39 || temp->rulenumber==54){
				temp=temp->kid->next;
				addsibling(&ASTtemp,temp);
				ASTtemp=ASTtemp->next;
			}
		}					
	}
	else if(temp->parent->rulenumber==35){
		temp=temp->kid;
		addkid(&ASTtemp,temp);
		ASTtemp=ASTtemp->kid;
		child* temp1=temp;
		ASTnode* temp2=ASTtemp;
		if(temp->rulenumber==42)
			addkid2(&ASTtemp,-1,-1);
		else{
			temp=temp->kid->next;
			temp=temp->kid;
			addkid(&ASTtemp,temp);
			ASTtemp=ASTtemp->kid;
			temp=temp->next;
			while(1){
				if(temp->rulenumber==74)
					break;
				else{
					temp=temp->kid->next;
					temp=temp->kid;
					addsibling(&ASTtemp,temp);
					ASTtemp=ASTtemp->next;
					temp=temp->next;
				}
			}
		}
		temp=temp1;
		ASTtemp=temp2;
		temp=temp->next->next;
		addsibling(&ASTtemp,temp);
		ASTtemp=ASTtemp->next;
		temp=temp->next->next->next;
		addsibling(&ASTtemp,temp);
		ASTtemp=ASTtemp->next;
		temp=temp->kid->next;
		temp=temp->kid;
		addkid(&ASTtemp,temp);
		ASTtemp=ASTtemp->kid;
		temp=temp->next;
		while(1){
			if(temp->rulenumber==74)
				break;
			else{
				temp=temp->kid->next;
				temp=temp->kid;
				addsibling(&ASTtemp,temp);
				ASTtemp=ASTtemp->next;
				temp=temp->next;
			}
		}					
	}
	parse=parse->next;
	if(parse->rulenumber==29){
		parse=parse->kid;
		ASTnode* temp1=abstractstmt(parse,&root,0);
	}
	
	return root;
}

ASTnode* constructAST(child* ptree){
	ASTnode* root=(ASTnode*)malloc(sizeof(ASTnode));
	root->num=0;
	root->term=0;
	root->next=NULL;
	root->kid=NULL;
	root->prev=NULL;
	root->parent=NULL;
	root->line=0;
	strcpy(root->value,"program");

	int no_func=0;
	child* temp=ptree->kid;
	while(1){
		if(temp->rulenumber==2){
			no_func++;
			temp=(temp->kid)->next;
		}
		else if(temp->rulenumber==3)
			break;
	}

	int i;
	ASTnode* ASTtemp;
	if(no_func==0){
		addkid2(&root,1,0);
		ASTtemp=root->kid;
	}
	else{
		addkid2(&root,3,0);
		ASTtemp=root->kid;
		for(i=0;i<no_func-1;i++){
			addsibling2(&ASTtemp,3,0);
			ASTtemp=ASTtemp->next;
		}
		addsibling2(&ASTtemp,1,0);
	}

	ASTtemp=root->kid;
	temp=ptree->kid;

	ASTnode* asttreepos;
	child* ptreepos;

	for(i=0;i<=no_func;i++){
		if(i!=no_func){
			if(i!=0){
				ASTtemp=asttreepos->parent->next;
				temp=ptreepos->parent->next;
			}
			addkid2(&ASTtemp,6,1);
			temp=temp->kid->kid;
			ASTtemp=ASTtemp->kid;
			strcpy(ASTtemp->value,temp->value);
			addsibling2(&ASTtemp,4,0);
			ASTtemp=ASTtemp->next;
			addsibling2(&ASTtemp,5,0);
			ASTtemp=ASTtemp->next;
			addsibling2(&ASTtemp,11,0);
			ASTtemp=ASTtemp->next;

			ASTtemp=(ASTtemp->prev)->prev;
			asttreepos = ASTtemp;
			temp=temp->next;
			ptreepos = temp;

			temp=temp->kid;
			temp=temp->next;
			temp=temp->next;
			temp=temp->next;
			temp=temp->next;

			temp=temp->kid;
			if(temp->rulenumber==9){
				temp=temp->kid->kid;
				addkid(&ASTtemp,temp);
				ASTtemp=ASTtemp->kid;
				temp=temp->parent->parent;
				temp=temp->next;
			}
			else{
				temp=temp->kid->kid->next;
				addkid(&ASTtemp,temp);
				ASTtemp=ASTtemp->kid;
				temp=temp->prev;
				temp=temp->parent->parent;
				temp=temp->next;
			}
			addsibling(&ASTtemp,temp);
			ASTtemp=ASTtemp->next;
			temp=temp->next;
			while(1){
				if(temp->rulenumber==15)
					break;
				else{
					temp=temp->kid->next;
					temp=temp->kid;
					if(temp->rulenumber==9){
						temp=temp->kid->kid;
						addsibling(&ASTtemp,temp);
						ASTtemp=ASTtemp->next;
						temp=temp->parent->parent;
						temp=temp->next;
					}
					else{
						temp=temp->kid->kid->next;
						addsibling(&ASTtemp,temp);
						ASTtemp=ASTtemp->next;
						temp=temp->prev;
						temp=temp->parent->parent;
						temp=temp->next;
					}
					addsibling(&ASTtemp,temp);
					ASTtemp=ASTtemp->next;
					temp=temp->next;
				}
			}

			temp = ptreepos->next;
			ptreepos=temp;
			ASTtemp = asttreepos->next;
			asttreepos=ASTtemp;

			if(temp->rulenumber==7){
				addkid2(&ASTtemp,-1,-1);
			}
			else{
				temp=temp->kid;
				temp=temp->next;
				temp=temp->next;
				temp=temp->next;
				temp=temp->next;

				temp=temp->kid;
				if(temp->rulenumber==9){
					temp=temp->kid->kid;
					addkid(&ASTtemp,temp);
					ASTtemp=ASTtemp->kid;
					temp=temp->parent->parent;
					temp=temp->next;
				}
				else{
					temp=temp->kid->kid->next;
					addkid(&ASTtemp,temp);
					ASTtemp=ASTtemp->kid;
					temp=temp->prev;
					temp=temp->parent->parent;
					temp=temp->next;
				}
				addsibling(&ASTtemp,temp);
				ASTtemp=ASTtemp->next;
				temp=temp->next;
				while(1){
					if(temp->rulenumber==15)
						break;
					else{
						temp=temp->kid->next;
						temp=temp->kid;
						if(temp->rulenumber==9){
							temp=temp->kid->kid;
							addsibling(&ASTtemp,temp);
							ASTtemp=ASTtemp->next;
							temp=temp->parent->parent;
							temp=temp->next;
						}
						else{
							temp=temp->kid->kid->next;
							addsibling(&ASTtemp,temp);
							ASTtemp=ASTtemp->next;
							temp=temp->prev;
							temp=temp->parent->parent;
							temp=temp->next;
						}
						addsibling(&ASTtemp,temp);
						ASTtemp=ASTtemp->next;
						temp=temp->next;
					}
				}
			}

			temp = ptreepos->next->next;
			ptreepos=temp;//at stmts
			ASTtemp = asttreepos->next;
			asttreepos=ASTtemp;//at stmts
		}

		else if(i==no_func){
			if(i!=0){
				ASTtemp=asttreepos->parent->next;
			}
			temp=ptree->kid->next;
			addkid2(&ASTtemp,15,1);
			ASTtemp=ASTtemp->kid;
			addsibling2(&ASTtemp,11,0);
			ASTtemp=ASTtemp->next;
			asttreepos=ASTtemp;//at stmts
			temp=temp->kid->next;
			ptreepos=temp;//at stmts
		}


		temp=temp->kid;
		addkid(&ASTtemp,temp);

		child* pcurt;
		ASTnode* astcurt;
		int flag=0;
		ASTtemp=ASTtemp->kid;
		while(1){
			if(temp->rulenumber==18){
				if(flag==0)
					addkid2(&ASTtemp,-1,-1);
				break;
			}
			else{
				temp=temp->kid;
				pcurt=temp;
				if(flag==0){
					addkid(&ASTtemp,temp);
					ASTtemp=ASTtemp->kid;
					flag=1;
				}
				else{
					addsibling(&ASTtemp,temp);
					ASTtemp=ASTtemp->next;
				}			
				astcurt=ASTtemp;
				temp=temp->kid->next;
				addkid(&ASTtemp,temp);
				ASTtemp=ASTtemp->kid;
				temp=temp->next;
				addsibling(&ASTtemp,temp);
				ASTtemp=ASTtemp->next;
				temp=temp->kid->kid;
				temp=temp->next->kid;
				addkid(&ASTtemp,temp);
				ASTtemp=ASTtemp->kid;
				temp=temp->parent;
				temp=temp->next->next;
				addsibling(&ASTtemp,temp);
				ASTtemp=ASTtemp->next;
				temp=temp->parent;
				temp=temp->next;
				temp=temp->kid->next;
				temp=temp->kid;
				addsibling(&ASTtemp,temp);
				ASTtemp=ASTtemp->next;
				temp=temp->parent;
				temp=temp->next->next;
				addsibling(&ASTtemp,temp);
				ASTtemp=ASTtemp->next;
				temp=temp->parent->next;
				while(1){
					if(temp->rulenumber==23)
						break;
					else{
						temp=temp->kid;
						temp=temp->kid->next;
						temp=temp->kid;
						addsibling(&ASTtemp,temp);
						ASTtemp=ASTtemp->next;
						temp=temp->parent;
						temp=temp->next->next;
						addsibling(&ASTtemp,temp);
						ASTtemp=ASTtemp->next;
						temp=temp->parent->next;
					}
				}
				temp=pcurt->next;
				ASTtemp=astcurt; 

			}
		}

		temp=ptreepos;
		ASTtemp=asttreepos;

		temp=temp->kid->next;
		ASTtemp=ASTtemp->kid;
		addsibling(&ASTtemp,temp);
		ASTtemp=ASTtemp->next;
		flag=0;
		while(1){
			if(temp->rulenumber==25){
				if(flag==0)
					addkid2(&ASTtemp,-1,-1);
				break;
			}
			else{
				temp=temp->kid;
				pcurt=temp;
				if(flag==0){
					addkid(&ASTtemp,temp);
					ASTtemp=ASTtemp->kid;
					flag=1;
				}
				else{
					addsibling(&ASTtemp,temp);
					ASTtemp=ASTtemp->next;
				}
				astcurt=ASTtemp;
				temp=temp->kid->next;
				if(temp->rulenumber==9){
					temp=temp->kid->kid;
					addkid(&ASTtemp,temp);
					ASTtemp=ASTtemp->kid;
					temp=temp->parent->parent;
				}
				else{
					temp=temp->kid->kid->next;
					addkid(&ASTtemp,temp);
					ASTtemp=ASTtemp->kid;
					temp=temp->prev;
					temp=temp->parent->parent;
				}
				temp=temp->next->next;
				addsibling(&ASTtemp,temp);
				ASTtemp=ASTtemp->next;
				temp=temp->next;
				if(temp->rulenumber==27){
					addsibling2(&ASTtemp,16,1);
				}
				temp=pcurt->next;
				ASTtemp=astcurt; 
			}
		}

		temp=ptreepos;
		ASTtemp=asttreepos;

		temp=temp->kid->next->next;
		ASTtemp=ASTtemp->kid->next;
		addsibling(&ASTtemp,temp);
		ASTtemp=ASTtemp->next;//at other stmts
		flag=0;
		if(temp->rulenumber==30)
			addkid2(&ASTtemp,-1,-1);
		else{
			temp=temp->kid;
			ASTtemp=abstractstmt(temp,&ASTtemp,1);	
		}
		temp=ptreepos;
		ASTtemp=asttreepos;
		temp=temp->kid->next->next->next;
		ASTtemp=ASTtemp->kid->next->next;
		addsibling(&ASTtemp,temp);
		ASTtemp=ASTtemp->next;
		temp=temp->kid;
		addkid(&ASTtemp,temp);
		ASTtemp=ASTtemp->kid;
		temp=temp->next;		
		if(temp->rulenumber==70){
			temp=temp->kid->next->kid;
			addkid(&ASTtemp,temp);
			ASTtemp=ASTtemp->kid;
			temp=temp->next;
			while(1){
				if(temp->rulenumber==74)
					break;
				else{
					temp=temp->kid->next->kid;
					addsibling(&ASTtemp,temp);
					ASTtemp=ASTtemp->next;
					temp=temp->next;
				}

			}
		}

	}
	return root;
}

void dfs2(ASTnode* node){
	if(node->term){
		printf("%-25s",node->value);
		printf("%d.",node->line);
		printf("%-25s",terminals[node->num]);
		if(node->num==4 || node->num==5){
			printf("%-5s",node->value);
		}
		else
			printf("---- ");
		if(!(node->num==0 && node->term==0)){
			if(node->parent->term)
				printf("%-25s",terminals[node->parent->num]);
			else
				printf("%-25s",nonterminals[node->parent->num]);
		}
		printf("YES ");
		printf("\n");
	}
	else{
		printf("------------------------ ");
		printf("%d.",node->line);
		printf("%-25s",nonterminals[node->num]);
		printf("---- ");
		if(!(node->num==0 && node->term==0)){
			if(node->parent->term)
				printf("%-25s",terminals[node->parent->num]);
			else
				printf("%-25s",nonterminals[node->parent->num]);
		}
		else 
			printf("ROOT                       ");
		printf("NO  ");
		printf("%s\n",nonterminals[node->num]);
	}
	if(node->kid)
		dfs2(node->kid);
	if(node->next)
		dfs2(node->next);
	return;
}

void printast(ASTnode* root){
	dfs2(root);
	return;
}

//TYPE CHECKING FUNCTIONS
/* function to check if loop variables are updated or not, and print relevant errors
 * input parameters: pointer to root of AST
 */
void loopcheck(ASTnode* root){
	ASTnode* temp=root;
	temp=temp->kid;//at function
	ASTnode* treepos=temp;
	while(1){
		if(temp->num==3){
			temp=temp->kid->next->next->next;// at stmts
		}
		else{
			temp=temp->kid->next;//at stmts
		}
		temp=temp->kid->next->next;//at other stmts
		temp=temp->kid;
		int flag=1;
		int stline, endline;
		while(1){
			if(temp->num==28){
				stline=temp->line;
				flag=0;
				ASTnode* t=temp;
				t=t->kid->next;
				char lhs[31];
				char rhs[31];
				t=t->kid;
				strcpy(lhs,t->value);
				t=t->next;
				strcpy(rhs,t->value);
				t=t->parent;
				t=t->next;
				while(1){
					if(t->num==22){
						t=t->kid;
						if( (strcmp(t->value,lhs)==0) || (strcmp(t->value,rhs))==0){
							flag=1;
							break;
						}
						t=t->parent;
					}
					if(t->next){
						t=t->next;
						continue;
					}
					endline=t->line;					
					break;
				}
				if(flag==0)
					printf("lines %d-%d: None of the variables participating in the iterations of the while loop get updated.\n",stline,endline);
			}
			if(temp->next){
				temp=temp->next;
				continue;
			}
			break;
		}
		temp=treepos;
		if(temp->next){
			temp=temp->next;
			treepos=temp;
			continue;
		}
		break;
	}
	return;
}

/* function to count nodes in parse tree
 * input parameters: pointer to root of parse tree, pointer to int (stores count of nodes)
 */
void countparse(child* node, int* no){
	(*no)++;
	if(node->kid)
		countparse(node->kid,no);
	if(node->next)
		countparse(node->next,no);
	return;
}

/* function to count nodes in AST
 * input parameters: pointer to root of AST, pointer to int (stores count of nodes)
 */
void countast(ASTnode* node, int* no){
	(*no)++;
	if(node->kid)
		countast(node->kid,no);
	if(node->next)
		countast(node->next,no);
	return;
}

/* function to calculate compression percentage
 * input parameters: pointers to roots of parse tree and AST
 * calls countparse() and countast()
 */
void compresspercent(child* parse, ASTnode* ast){
	int noparse,noast;
	noparse=0;
	noast=0;
	countparse(parse,&noparse);
	countast(ast,&noast);
	int sizechild = sizeof(child);
	int sizeast = sizeof(ASTnode);
	printf("Parse Tree  ");
	printf("Number of nodes = %d  ",noparse);
	printf("Allocated memory = %d bytes\n", noparse*sizechild);
	printf("AST         ");
	printf("Number of nodes = %d  ",noast);
	printf("Allocated memory = %d bytes\n", noast*sizeast);
	float ans = ((float)(noparse*sizechild-noast*sizeast)/(float)(noparse*sizechild))*100;
	printf("Compression percentage = ((%d-%d)/%d)*100 = %f\n",noparse*sizechild,noast*sizeast,noparse*sizechild,ans);

}

/* function to check if output parameters of a function are updated within the function or not, and prints relevant errors
 * check if output parameter variables are found in LHS of assignment statement, I/O statement or function call statement
 * input parameters: pointer to root of AST
 */
void parameterchainchecker(ASTnode* root) {
    ASTnode *p = root->kid;
    ASTnode *kid = NULL;
    ASTnode *c = NULL;
    
    int* check;
    //char** name;
    while (p->num != 1) {
    	int count = 0;
        int line = p->line;
        ASTnode *j1 = ((p->kid)->next)->next;
        ASTnode *stats = NULL;
        if (j1->num == 5 && j1->term == 0) {
            kid = j1->kid;
            stats = ((((j1->next)->kid)->next)->next)->kid;
        }
        else {
            p = p->next;
            continue;
        }
        c = kid;
        while (c != NULL) {
            count++;
            c = c->next;
        }
        //char names[count/2][31];
        //int check[count/2];

        check= (int*)malloc(sizeof(int)*(count/2));
        
        int i = 0;
        int j;
        // while (kid != NULL) {
        //     if (kid->term == 1 && kid->num == 3) {
        //     	printf("%s\n", kid->value);
        //         strcpy(names[i], kid->value);
        //         check[i] = 0;
        //         i++;
        //     }
        //     kid = kid->next;
        // }
        while(i<count/2){
        	check[i]=0;
        	i++;
        }
        while (stats != NULL) {
            if (stats->num == 22 && stats->term == 0) {
                ASTnode *id = stats->kid;
                i = 0;
                ASTnode* k=kid->next;
                while (i < count/2) {
                	
                	for(j=0;j<i;j++){
                		k=k->next->next;
                	}
                    if (!strcmp(id->value, k->value)) {
                        check[i] = 1;
                        break;
                    }
                    i++;
                }
            }
            else if (stats->num==31 && stats->term==0){
            	ASTnode *id=stats->kid;
            	if(id->num==32){
            		id=id->kid;
            		i = 0;
            		ASTnode* k=kid->next;
	                while (i < count/2) {
	                	
	                	for(j=0;j<i;j++){
	                		k=k->next->next;
	                	}
	                    if (!strcmp(id->value, k->value)) {
	                        check[i] = 1;
	                        break;
	                    }
	                    i++;
	                }

            	}
            }
            else if (stats->num==25 && stats->term==0){
            	ASTnode *id=stats->kid;
            	if(id->kid){
            		id=id->kid;
	            	while(id){
	            		i = 0;
	            		ASTnode* k=kid->next;
		                while (i < count/2) {		                	
		                	for(j=0;j<i;j++){
		                		k=k->next->next;
		                	}
		                    if (!strcmp(id->value, k->value)) {
		                        check[i] = 1;
		                    
		                        break;
		                    }
		                    i++;
		                }
		                id=id->next;
	            	}
	            }
	        }

            stats = stats->next;
        }
        i = 0;
       
        ASTnode* k=kid->next;
        while (i < count/2){
            if (check[i] == 0) {           	
            	for(j=0;j<i;j++){
            		k=k->next->next;
            	}
            	//printf("%s", names[i]);
                printf("Line %d: The output parameter <%s> in function %s is not assigned a value.\n", line, k->value, (p->kid)->value);
            }
            i++;
        }
        p = p->next;
    }
}

/* function to check for type mismatches in a boolean or arithmetic expression
 * calls searchSymbolTable() to check type of variables/records
 * input parameters: pointer to root of arithmetic/boolean expression, and scope of function
 * output parameters: integer representing type of expression (0: error, 1: expression deals only with int type variables and values,
 * 2: expression deals only with real type variables and values, 3: expression deals with record variables, and maybe int/real also)
 */
int boolcheck(ASTnode* temp,int hash,char fname[31]){
	int lhs,rhs;
	int recids;
	if(temp->kid){
		if(temp->kid->num==45)
			temp=temp->kid->kid;
		if(temp->kid->next->next){
			recids=1;
		}
		else{
			lhs=boolcheck(temp->kid,hash,fname);
			rhs=boolcheck(temp->kid->next,hash,fname);
			if(lhs!=3 && rhs!=3){
				if(lhs==rhs)
					return lhs;
				else 
					return 0;
			}
			else{
				if(lhs!=rhs)
					return 3;
				else if(temp->num== 37|| temp->num ==38){
					printf("line %d: Record multiplication/division not allowed\n",temp->line);
					return 0;
				}
				else
					return 3;
			}
		}		
	}
	if(recids==1){
		temp=temp->kid->next;
		if(temp->num==2){
			temp=temp->prev;
			char value[31];
			strcpy(value,temp->value);
			TypeNode* ans= searchSymbolTable(hash,fname,value);
			TypeNode* mem = ans->members;
			char fid[31];
			temp=temp->next;
			strcpy(fid,temp->value);
			lhs=0;
			while(mem){
				if(!strcmp(fid,mem->value)){
					if(!strcmp(mem->type,"int"))
						lhs=1;
					else
						lhs=2;
					break;
				}
				mem=mem->next;
			}
			if(temp->next->next){
				temp=temp->next;
				char value[31];
				strcpy(value,temp->value);
				TypeNode* ans= searchSymbolTable(hash,fname,value);
				TypeNode* mem = ans->members;
				char fid[31];
				temp=temp->next;
				strcpy(fid,temp->value);
				rhs=0;
				while(mem){
					if(!strcmp(fid,mem->value)){
						if(!strcmp(mem->type,"int"))
							rhs=1;
						else
							rhs=2;
						break;
					}
					mem=mem->next;
				}
			}
			else{
				temp=temp->next;
				rhs = boolcheck(temp,hash,fname);	
			}
		}
		else{
			temp=temp->prev;
			lhs=boolcheck(temp,hash,fname);	
			temp=temp->next;
			char value[31];
			strcpy(value,temp->value);
			TypeNode* ans= searchSymbolTable(hash,fname,value);
			TypeNode* mem = ans->members;
			char fid[31];
			temp=temp->next;
			strcpy(fid,temp->value);
			rhs=0;
			while(mem){
				if(!strcmp(fid,mem->value)){
					if(!strcmp(mem->type,"int"))
						rhs=1;
					else
						rhs=2;
					break;
				}
				mem=mem->next;
			}
		}
		if(lhs==rhs)
			return lhs;
		else 
			return 0;
	}
	else{
		if(temp->num==4)
			return 1;
		else if(temp->num==5)
			return 2;
		else if(temp->num==3){
			char value[31];
			strcpy(value,temp->value);
			TypeNode* ans= searchSymbolTable(hash,fname,value);
			if(ans){
				if(!strcmp(ans->type,"int"))
					return 1;
				else if(!strcmp(ans->type,"real"))
					return 2;
				else
					return 3;
			}
			return 0;
		}
	}
}

/* function for type checking of iterative, function call, conditional and assigment statements
 * input parameters: pointer to root of AST, hash value of current function scope, and name of function
 * uses this to call searchSymbolTable() for type-checking of variables
 */
void ostype(ASTnode* root ,int hash, char fname[31]){
	ASTnode* temp=root;
	if(temp->num==22){
		//assignment
		int isrec=0;
		temp=temp->kid->next;
		if(temp->num==2){
			isrec=1;
			temp=temp->next;
		}
		temp=temp->next;
		int ret= boolcheck(temp,hash,fname);
		if(ret==0)
			printf("line %d: Type mismatch in assignment statement\n",temp->line);
		else{
			temp=temp->prev->prev;
			if(isrec)
				temp=temp->prev;
			char value[31];
			strcpy(value,temp->value);
			TypeNode* ans= searchSymbolTable(hash,fname,value);
			if(ans){
				int ty;
				if(isrec){
					TypeNode* mem = ans->members;
					char fid[31];
					temp=temp->next;
					strcpy(fid,temp->value);
					while(mem){
						if(!strcmp(fid,mem->value)){
							if(!strcmp(mem->type,"int"))
								ty=1;
							else
								ty=2;
							break;
						}
						mem=mem->next;
					}

				}
				else{
					if(!strcmp(ans->type,"int"))
						ty=1;
					else if(!strcmp(ans->type,"real"))
						 ty=2;
					else
						ty=3;
					if(ret!=ty)
						printf("line %d: Type mismatch: while assigning <%s>\n",temp->line,value);
				}
			}

		}

	}
	if(temp->num==25){
		//funcall
		temp=temp->kid->next;
		int f = hashfunction(temp->value);
		temp=temp->prev;
		ASTnode* op=temp;
		TypeNode* point =  htable[f].outputpars;
		while(1){
			if(point){			
				if(temp->kid){
					temp=temp->kid;
					char value[31];
					strcpy(value,temp->value);
					TypeNode* ans= searchSymbolTable(hash,fname,value);
					if(ans){
						if(strcmp(ans->type,point->type))
							printf("line %d: Type mismatch: <%s> and <%s>\n",temp->line,value, point->label);
					}
				}
				else if(temp->next){
					temp=temp->next;
					char value[31];
					strcpy(value,temp->value);
					TypeNode* ans= searchSymbolTable(hash,fname,value);
					if(ans){
						if(strcmp(ans->type,point->type))
							printf("line %d: Type mismatch: <%s> and <%s>\n",temp->line,value, point->label);
					}
				}
				else{
					printf("line %d: number of output parameters dont match\n",temp->line);
					break;
				}
				point=point->next;
			}
			else if(temp->next || temp->kid){
				printf("line %d: number of output parameters dont match\n",temp->line);
				break;
			}
			else
				break;
		}
		temp=op;
		temp=temp->next->next;
		point =  htable[f].inputpars;
		while(1){
			if(point){			
				if(temp->kid){
					temp=temp->kid;
					char value[31];
					strcpy(value,temp->value);
					TypeNode* ans= searchSymbolTable(hash,fname,value);
					if(ans){
						if(strcmp(ans->type,point->type))
							printf("line %d: Type mismatch: <%s> and <%s>\n",temp->line,value, point->label);
					}
				}
				else if(temp->next){
					temp=temp->next;
					char value[31];
					strcpy(value,temp->value);
					TypeNode* ans= searchSymbolTable(hash,fname,value);
					if(ans){
						if(strcmp(ans->type,point->type))
							printf("line %d: Type mismatch: <%s> and <%s>\n",temp->line,value, point->label);
					}
				}
				else{
					printf("line %d: number of input parameters dont match\n",temp->line);
					break;
				}
				point=point->next;
			}
			else if(temp->next || temp->kid){
				printf("line %d: number of input parameters dont match\n",temp->line);
				break;
			}
			else
				break;
		}



	}
	if(temp->num==28){
		temp=temp->kid->next;
		int ret= boolcheck(temp,hash,fname);
		if(ret==0)
			printf("line %d: Type mismatch in condition of statement\n", temp->line);
		temp=temp->next;
		while(1){
			ostype(temp,hash,fname);
			if(temp->next){
				temp=temp->next;
				continue;
			}
			break;
		}
		//is
	}
	if(temp->num==29){
		temp=temp->kid->kid;
		int ret= boolcheck(temp,hash,fname);
		if(ret==0)
			printf("line %d: Type mismatch in conditional statement\n", temp->line);
		temp=temp->next;
		while(1){
			ostype(temp,hash,fname);
			if(temp->next){
				temp=temp->next;
				continue;
			}
			break;
		}
		//cs

	}
}

/* driver function for type-checking of every function defined in the source code
 * input parameters: pointer to root of AST
 */
void typechecker(ASTnode* root){
	int global=hashfunction("global");
	ASTnode* temp=root;
	temp=temp->kid;
	ASTnode* treepos=temp;
	while(1){
		int hash;
		char fname[31];
		if(temp->num==3){
			temp=temp->kid;
			hash = hashfunction(temp->value);
			strcpy(fname,temp->value);
			temp=temp->next->next->next;//at stmts
			temp=temp->kid->next->next;//at other stmts
		}
		else{
			hash = hashfunction("_main");
			strcpy(fname,"_main");
			temp=temp->kid->next;//at stmts
			temp=temp->kid->next->next;//at other stmts
		}
		temp=temp->kid;
		while(1){
			ostype(temp,hash,fname);
			if(temp->next){
				temp=temp->next;
				continue;
			}
			break;
		}
		temp=temp->parent->next->kid;
		TypeNode* point =  htable[hash].outputpars;
		
		while(1){
			if(point){			
				if(temp->kid){
					temp=temp->kid;
					char value[31];
					strcpy(value,temp->value);
					if(strcmp(value,point->label))
						printf("line %d: Type mismatch: <%s> and <%s>\n",temp->line,value, point->label);
				}
				else if(temp->next){
					temp=temp->next;
					char value[31];
					strcpy(value,temp->value);
					if(strcmp(value,point->label))
						printf("line %d: Type mismatch: <%s> and <%s>\n",temp->line,value, point->label);
				}
				else{
					printf("line %d: number of output parameters dont match\n",temp->line);
					break;
				}
				point=point->next;
			}
			else if(temp->next || temp->kid){
				printf("line %d: number of output parameters dont match\n",temp->line);
				break;
			}
			else
				break;
		}
		if(treepos->next){
			temp=treepos->next;
			treepos=temp;
			continue;
		}
		break;
	}

}
