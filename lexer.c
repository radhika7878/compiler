//Batch 77
//Radhika Pasari 2012B4A7445P
//Pankhuri Kumar 2012B4A7662P
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexerDef.h"

extern int line_number;
int buffersize = 101;
extern char first_buffer[100] ;
extern char second_buffer[100] ;
int end_check = 0;
int sb_check;
char *bufferinuse;
int buffno = 1;
char checking;
extern list *root;
extern int k;
extern FILE *fp;
extern int firsttime ;
tokenInfo getNextToken(int op);

/* driver function to display tokens
 * input parameters: source code file, and op = 1
 * calls getNextToken() to receive token and displays its contents
 */
void displayFromLexer(char testCaseFile[],int op) {
	fp = fopen(testCaseFile, "r");
	if (fp == NULL)
		printf("Error in opening input file.");
	while(1) {
		tokenInfo trial = getNextToken(op);
		if (!strcmp(trial.token, "EOF")) {
			printf("\n\n\nEnd of program. All tokens have been parsed.\n");
			break;
		}
		if(strcmp(trial.token,"ERR")!=0 || strcmp(trial.token,"ERR1")!=0){
			printf("\n%s", trial.token);
			printf("\n%s", trial.value);
			printf("\n%d", trial.linenumber);
		}
		
	}
	return;
}

/* function to create linkedlist of all keyword tokens eg. TK_MAIN, TK_RETURN which differ from fieldids and function id (for main)
 * output parameters: pointer to the root of the linkedlist
 * called by getNextToken() on first run
 */
list* createKeywordList() {
	FILE *fp1 = fopen("keywords.txt", "r+");
	if (fp1 == NULL)
		printf("\nError creating list of keywords.");
	list *root = (list *) malloc(sizeof(list));
	char v[21], t[14];
	int ret = fscanf(fp1, "%s %s", v, t);
	if (ret == 2) {
		strcpy(root->value, v);
		strcpy(root->token, t);
		root->next = NULL;
	}
	list *temp1 = root;
	//printf("%s %s", v, t);
	while ((ret = fscanf(fp1, "%s %s", v, t)) == 2) {
		list *temp = (list *) malloc(sizeof(list));
		strcpy(temp->value, v);
		strcpy(temp->token, t);
		//printf("%s %s", v, t);
		temp1->next = temp;
		temp1 = temp1->next;
	}
	return root;
}


/* function to traverse list of keyword and copies relevant token, if found
 * input parameters: root to linkedlist, keyword to be found(v), and variable where token is to be stored (tok)
 * output parameters: returns 1 if matching node is found, 0 otherwise
 */
int searchList(list *root, char v[], char tok[14]) {
	list *temp = root;
	while(temp != NULL) {
		if (!strcmp(temp->value, v)) {
			strcpy(tok, temp->token);
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}

/* function to efficiently read source code file, called by getNextToken
 * input parameters: file pointer to source code, buffer to be filled, size of file to be read
 * output parameters: updated file pointer after reading given buffersize of source code
 */
FILE *getStream(FILE *fp, char* buffer, int buffersize) {

	if (fgets(buffer, buffersize, fp) == NULL)
		end_check = 1;
	else
		return fp;
}

/* driver function to display source code without comments on console
 * input parameters: file pointer to source code
 * output parameters: returns -1 in case of error, 1 otherwise
 */
int removeComments(FILE *fp1) {
	char buffer[101];
	int j = 0;
	//assuming all comments are less than 100 characters long
	while(fgets(buffer, 100, fp1) != NULL) {
		j = 0;
		while (j < 101) {
			if (buffer[j] == '%') {
				while ((int)buffer[j] != 10) {
					j++;
				}
			}
			else if((int)buffer[j] == 0 || buffer[j] == '^')
				break;
			else
				printf("%c", buffer[j]);
			j++;
		}
		memset(buffer, '^', 100);		
	}
	if (feof(fp1)) {
		return 1;
	} else {
		return -1;
	}
}

/* function that reads source code file, recognizes tokens and returns them
 * input parameter: option which is calling this function.
 * output parameters: token with relevant details (value, tokentype, line number) filled
 */
tokenInfo getNextToken(int op) {
	char lookahead;
	tokenInfo newToken;
	char current_value[100] = "";
	int length = 0;
	//for repeating patterns within the same case
	int or_count = 0;
	int and_count = 0;
	int assign_count = 0;
	int rnum_count = 0;
	//to check if token has been found yet
	int flag = 0;
	int rnum_flag = 0;

	//create keywork token list the first time it is run
	if (firsttime == 0) {
		root = createKeywordList();
		firsttime = 1;
	}

	// for efficient reading of source code file, and alternating I/O operations and CPU operations
	if (buffno == 1)
		bufferinuse = first_buffer;

	//if both buffers are empty, read into both
	if ((int)first_buffer[0] == 0 && (int)second_buffer[0] == 0) {
		memset(first_buffer, '^', 99);
		memset(second_buffer, '^', 99);
		fp = getStream(fp, first_buffer, buffersize);
		first_buffer[buffersize-1] = '^';
		fp = getStream(fp, second_buffer, buffersize);
		second_buffer[buffersize-1] = '^';
		sb_check = 1;
		k = 0;
	}

	int current_state = 0;
	while (1) {
		//end_check is set if either of the buffers encounter EOF while reading file
		//sb_check is set if the secondbuffer encounters EOF
		if (end_check == 1) {
			if (sb_check == 0) {
				strcpy(newToken.value, "NULL");
				strcpy(newToken.token, "EOF");
				newToken.linenumber = line_number;
				memset(first_buffer, '^', 99);
				memset(second_buffer, '^', 99);
				end_check = 0;
				line_number = 1;
				buffno = 1;
				return newToken;
			}
		}

		//toggling between buffers
		checking = bufferinuse[k];
		if (checking == '^') {
			if (buffno == 1) {
				if (end_check != 1) {
					bufferinuse = second_buffer;
					buffno = 2;
					k = 0;
				} else {
					if (current_state == 0) {
						sb_check = 0;
						continue;
					}
				}	
			}
			else {
				if (end_check != 1) {
					memset(first_buffer, '^', 99);
					fp = getStream(fp, first_buffer, buffersize);
					sb_check = 0;
				}
				if (end_check != 1) {
					memset(second_buffer, '^', 99);
					fp = getStream(fp, second_buffer, buffersize);
					sb_check = 1;
				}				
				bufferinuse = first_buffer;
				buffno = 1;
				k = 0;
			}
		}

		if ((int)current_value[0] > 97 && (int)current_value[0] < 101 && length > 20) 
			current_state = 34;
		if (current_value[0] == '_' && length > 30)
			current_state = 35;
		
		switch (current_state) {
			case 0: lookahead = bufferinuse[k++];
					if ((int)lookahead == 32)
						continue;
					else if (lookahead == '[')
						current_state = 1;
					else if (lookahead == ']')
						current_state = 2;
					else if (lookahead == ';')
						current_state = 3;
					else if (lookahead == ':')
						current_state = 4;
					else if (lookahead == '.')
						current_state = 5;
					else if (lookahead == '(')
						current_state = 6;
					else if (lookahead == ')')
						current_state = 7;
					else if (lookahead == '+')
						current_state = 8;
					else if (lookahead == '-')
						current_state = 9;
					else if (lookahead == '*')
						current_state = 10;
					else if (lookahead == '/')
						current_state = 11;
					else if (lookahead == '@') {
						current_state = 12;
						or_count++;
					}
					else if (lookahead == '~')
						current_state = 13;
					else if (lookahead == '&') {
						current_state = 14;
						and_count++;
					}
					else if (lookahead == '<')
						current_state = 15;
					else if (lookahead == '=') {
						current_state = 16;
					}
					else if (lookahead == '>')
						current_state = 17;
					else if (lookahead == '!')
						current_state = 18;
					else if ((int)lookahead == 10) {
						line_number++;
						current_state = 0;
						break;
					}
					else if (lookahead == '%')
						current_state = 24;
					else if (lookahead == ',')
						current_state = 25;
					//main or TK_FUNID
					else if (lookahead == '_') 
						current_state = 26;
					//TK_RECORDID
					else if (lookahead == '#')
						current_state = 27;
					//TK_NUM or TK_RNUM
					else if ((int)lookahead <= 57 && (int)lookahead >= 48)
						current_state = 28;
					//TK_ID
					else if ((int)lookahead > 97 && (int)lookahead < 101)
						current_state = 29;
					//TK_FIELDID
					else if ((int)lookahead == 97 || ((int)lookahead >= 101 && (int)lookahead <=122))
						current_state = 30;
					else if (lookahead = ' ') {
						strcpy(current_value, "");
						continue;
					}
					else
						current_state = 32;
					break;
			case 1: strcpy(newToken.value, "[");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_SQL");
					flag = 1;
					current_state = 0;
					break;
			case 2: strcpy(newToken.value, "]");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_SQR");
					flag = 1;
					current_state = 0;
					break;
			case 3: strcpy(newToken.value, ";");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_SEM");
					flag = 1;
					current_state = 0;
					break;
			case 4: strcpy(newToken.value, ":");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_COLON");
					flag = 1;
					current_state = 0;
					break;
			case 5: strcpy(newToken.value, ".");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_DOT");
					flag = 1;
					current_state = 0;
					break;
			case 6: strcpy(newToken.value, "(");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_OP");
					flag = 1;
					current_state = 0;
					break;
			case 7: strcpy(newToken.value, ")");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_CL");
					flag = 1;
					current_state = 0;
					break;
			case 8: strcpy(newToken.value, "+");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_PLUS");
					flag = 1;
					current_state = 0;
					break;
			case 9: strcpy(newToken.value, "-");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_MINUS");
					flag = 1;
					current_state = 0;
					break;
			case 10: strcpy(newToken.value, "*");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_MUL");
					flag = 1;
					current_state = 0;
					break;
			case 11: strcpy(newToken.value, "/");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_DIV");
					flag = 1;
					current_state = 0;
					break;
			case 12: current_value[length] = lookahead; 
					length++;
					lookahead = bufferinuse[k++];
					if (lookahead == '@') {
						or_count++;
					}
					else {
						if (or_count == 3) {
							strcpy(newToken.value, current_value);
							newToken.linenumber = line_number;
							strcpy(newToken.token, "TK_OR");
							flag = 1;
							current_state = 0;
							k--;
						} else {
							current_state = 33;
						}
					}
					break;
			case 13: strcpy(newToken.value, "~");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_NOT");
					flag = 1;
					current_state = 0;
					break;
			case 14: current_value[length] = lookahead; 
					length++;
					lookahead = bufferinuse[k++];
					if (lookahead == '&') {
						and_count++;
					}
					else {
						if (and_count == 3) {
							strcpy(newToken.value, current_value);
							newToken.linenumber = line_number;
							strcpy(newToken.token, "TK_AND");
							flag = 1;
							current_state = 0;
						} else {
							current_state = 33;
						}
					}
					break;
			case 15: current_value[length] = lookahead; 
					length++;
					lookahead = bufferinuse[k++];
					if (lookahead == '-') {
						current_state = 20;
						assign_count++;
					}
					else if (lookahead == '=')
						current_state = 21;
					else {
						strcpy(newToken.value, current_value);
						newToken.linenumber = line_number;
						strcpy(newToken.token, "TK_LT");
						flag = 1;
						current_state = 0;
					}
					break;
			case 16: current_value[length] = lookahead; 
					length++;
					lookahead = bufferinuse[k++];
					if (lookahead == '=') {
						current_value[length] = lookahead; 
						strcpy(newToken.value, current_value);
						newToken.linenumber = line_number;
						strcpy(newToken.token, "TK_EQ");
						flag = 1;
						current_state = 0;
					}
					else if ((int)lookahead == 32 && length == 1)
						break;
					else 
						current_state = 33;
					break;
			case 17: lookahead = bufferinuse[k++];
					if (lookahead == '=')
						current_state = 22;
					else {
						strcpy(newToken.value, ">");
						newToken.linenumber = line_number;
						strcpy(newToken.token, "TK_GT");
						flag = 1;
						current_state = 0;
					}
					break;
			case 18: current_value[length] = lookahead; 
					length++;
					lookahead = bufferinuse[k++];
					if (lookahead == '=')
						current_state = 23;
					else
						current_state = 33;
					break;
			case 20: current_value[length] = lookahead; 
					length++;
					lookahead = bufferinuse[k++];
					if (lookahead == '-') {
						assign_count++;
					}
					else {
						if (assign_count == 3) {
							strcpy(newToken.value, current_value);
							newToken.linenumber = line_number;
							strcpy(newToken.token, "TK_ASSIGNOP");
							flag = 1;
							k--;
							current_state = 0;
						} else {
							current_state = 33;
						}
					}
					break;
			case 21: strcpy(newToken.value, "<=");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_LE");
					flag = 1;
					current_state = 0;
					break;
			case 22: strcpy(newToken.value, ">=");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_GE");
					flag = 1;
					current_state = 0;
					break;
			case 23: strcpy(newToken.value, "!=");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_NE");
					flag = 1;
					current_state = 0;
					break;
			case 24: do {
						lookahead = bufferinuse[k++];
					} while ((int)lookahead != 10);
					strcpy(newToken.value, "%");
					newToken.linenumber = line_number;
					line_number++;
					strcpy(newToken.token, "TK_COMMENT");
					flag = 1;
					current_state = 0;
					break;
			case 25: strcpy(newToken.value, ",");
					newToken.linenumber = line_number;
					strcpy(newToken.token, "TK_COMMA");
					flag = 1;
					current_state = 0;
					break;
			case 26: //functions 
					do {
						current_value[length] = lookahead;;
						length++;
						lookahead = bufferinuse[k++];
					} while (((int)lookahead >= 65 && (int)lookahead <= 90) || ((int)lookahead >= 97 && (int)lookahead <= 122));
					while ((int)lookahead >= 48 && (int)lookahead <= 57) {
						current_value[length] = lookahead;;
						length++;
						lookahead = bufferinuse[k++];
					}
					if (length > 30) {
						current_state = 35;
						break;
					}
					if (!strcmp(current_value, "_"))
						current_state = 33;
					else {
						char tok[14] = "";
						strcpy(newToken.value, current_value);
						int check = searchList(root, current_value, tok);
						if (check == 1) 
							strcpy(newToken.token, tok);
						else
							strcpy(newToken.token, "TK_FUNID");
						newToken.linenumber = line_number;
						flag = 1;
				    	current_state = 0;
						k--;
					}
					break;
			case 27: //records 
					do {
						current_value[length] = lookahead;;
						length++;
						lookahead = bufferinuse[k++];
					} while ((int)lookahead >= 97 && (int)lookahead <= 122);
					strcpy(newToken.value, current_value);
					strcpy(newToken.token, "TK_RECORDID");
					newToken.linenumber = line_number;
					flag = 1;
					current_state = 0;
					k--;
					break;
			case 28: //numbers and real numbers 
					if (rnum_flag != 1) {
						do {
							current_value[length] = lookahead;
							length++;
							lookahead = bufferinuse[k++];
						} while ((int)lookahead >= 48 && (int)lookahead <= 57);
						rnum_flag = 1;
					}
					if (lookahead == '.') {
						current_value[length] = lookahead;
						length++;
						rnum_count = 1;
					}
					if (rnum_count > 0) {
						lookahead = bufferinuse[k++];
						if ((int)lookahead >= 48 && (int)lookahead <= 57) {
							char l2 = bufferinuse[k];
							if ((int)l2 >= 48 && (int)l2 <= 57) {
								current_value[length++] = lookahead;
								current_value[length] = l2;
								length++;
								k+=2;
							} else {
								current_state = 33;
								break;
							}
						} else {
							current_state = 33;
							break;
						}
					}
					if (rnum_count > 0) {
						strcpy(newToken.token, "TK_RNUM");
						rnum_count = 0;
					}
					else
						strcpy(newToken.token, "TK_NUM");
					rnum_flag = 0;
					strcpy(newToken.value, current_value);
					newToken.linenumber = line_number;
					flag = 1;
					k--;
					current_state = 0;
					break;
			case 29: //ids 
					current_value[length] = lookahead;
					length++;
					lookahead = bufferinuse[k++];
					if ((int)lookahead >= 50 && (int)lookahead <= 55) 
						current_state = 31;
					else if ((int)lookahead >= 97 && (int)lookahead <= 122)
						current_state = 30;
					else {
						lookahead = ' ';
						current_state = 30;
						k--;
					}
					break;
			case 30: //filedids 
					do {
						current_value[length] = lookahead;;
						length++;
						lookahead = bufferinuse[k++];
					} while ((int)lookahead >= 97 && (int)lookahead <= 122);
					char tok[14];
					strcpy(newToken.value, current_value);
					int check = searchList(root, current_value, tok);
					if (check == 1) 
						strcpy(newToken.token, tok);
					else
						strcpy(newToken.token, "TK_FIELDID");
					newToken.linenumber = line_number;
					flag = 1;
					current_state = 0;
					k--;
					break;
			case 31: do {
						current_value[length] = lookahead;;
						length++;
						lookahead = bufferinuse[k++];
					} while ((int)lookahead >= 98 && (int)lookahead <= 100);
					while ((int)lookahead >= 50 && (int)lookahead <= 55) {
						current_value[length] = lookahead;;
						length++;
						lookahead = bufferinuse[k++];
					}
					if (length > 20) {
						current_state = 34;
						break;
					}
					strcpy(newToken.value, current_value);
					strcpy(newToken.token, "TK_ID");
					newToken.linenumber = line_number;
					flag = 1;
					current_state = 0;
					k--;
					break;
			//Error Implementation
			case 32: printf("\nUnknown symbol %c at line %d.", lookahead, line_number+1);
					strcpy(newToken.value, "NULL");
					strcpy(newToken.token, "ERR");
					newToken.linenumber = line_number;
					return newToken;
					break;
			case 33: current_value[length] = lookahead; 
					length++;
					printf("\nUnknown pattern %s at line %d.", current_value, line_number+1);
					strcpy(newToken.value, "NULL");
					strcpy(newToken.token, "ERR");
					newToken.linenumber = line_number;
					return newToken;
					break;
			case 34: printf("\nIdentifier %s at line %d is longer than the prescribed length of 20 characters.", current_value, line_number+1);
					strcpy(newToken.value, "NULL");
					strcpy(newToken.token, "ERR");
					newToken.linenumber = line_number;
					return newToken;
					break;
			case 35: printf("\nFunction %s at line %d is longer than the prescribed length of 30 characters.", current_value, line_number+1);
					strcpy(newToken.value, "NULL");
					strcpy(newToken.token, "ERR");
					newToken.linenumber = line_number;
					return newToken;
					break;
		}
		if (flag == 1) {
			flag = 0;
			return newToken;
		}
	}
}

