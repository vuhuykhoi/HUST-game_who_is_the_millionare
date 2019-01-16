#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "User_ll.h"
#include <stdio.h>

#include "gamehelper.h"

val newVal(char *acc,char *pass){
	val tmp ;
	tmp.acc = malloc(sizeof(char)*20);
	tmp.pass = malloc(sizeof(char)*20);
	strcpy(tmp.acc,acc);
	strcpy(tmp.pass,pass);
	tmp.sts = NOT_LOG_IN;
	
	return tmp;
}
val newEmptyVal(){
	val tmp ;
	tmp.acc = malloc(sizeof(char)*20);
	tmp.pass = malloc(sizeof(char)*20);
	tmp.sts = NOT_LOG_IN;
	
	return tmp;	
}

void printVal(val El){
	printf("%s|%s|%d|\n",El.acc,El.pass,El.sts);
}

node* newNode(val El){
	node *tmp = malloc(sizeof(node));
	if(tmp ==NULL){
		printf("cannot malloc");
		exit(1);
	}
	tmp->val = El;
	tmp->next = NULL;
	return tmp;
}

linkList *newLinkList(){
	linkList *tmp = malloc(sizeof(linkList));
	return tmp;
}

void addToLastOfList(linkList *list,val El){
	if(list->first == NULL){
		list->first = newNode(El);
		list->last = list->first;
	}else{
		list->last->next =  newNode(El);
		list->last = list->last->next;
	}
}

void printList(linkList *list){
	node *tmp = list->first;
	while(tmp !=NULL){
		printVal(tmp->val);
		tmp = tmp->next;
	}
}

int isExistedNode(linkList *list,char *acc){
	node *tmp = list->first;
	while(tmp !=NULL){
		if(strcmp(tmp->val.acc,acc)==0) return 1;
		tmp = tmp->next;
	}	
	return 0;
}

node* findNodeByValue(linkList *list,char *acc){
	node *tmp = list->first;
	while(tmp !=NULL){
		if(strcmp(tmp->val.acc,acc)==0) return tmp;
		tmp = tmp->next;
	}	
	return tmp;	
}

//read file to get account information and save to linklist
linkList* getListUserFromFile(FILE *file){
	char *line = malloc(sizeof(char)*50);
	linkList *tmpLL = newLinkList();
	val El;
	while(fgets(line,50,file) != NULL ){
		
		if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
		int i = 0;
		char *p = strtok (line, " ");
    	char *array[2];

    	while (p != NULL)
    	{
        	array[i++] = p;
        	p = strtok (NULL, " ");
    	}
		El = newVal(array[0],array[1]);
		if(tmpLL!=NULL) addToLastOfList(tmpLL,El);
	}
	return tmpLL;
}

/*
check if username had registed.
return 0 if username havent been registed
return 1 if username had been registed */
int isExistedUserName(linkList *list,char *username){
	return isExistedNode(list,username);
}

/*
checkif password of account is correct 
return 0 if is fail
return 1 if true*/
int isCorrectPassword(val account,char *input_pass){
	if(!strcmp(account.pass,input_pass)) return 1;
	return 0;
}

/*
update account information to file
open and write list account to file again
*/
void updateAccountToFile(linkList *list,char *fname){
	FILE *fp =fopen(fname,"r+");
	node *tmp = list->first;
	while(tmp!=NULL){
		fprintf(fp,"%s %s %d\n",tmp->val.acc,tmp->val.pass,tmp->val.sts);
		tmp = tmp->next;
	}
	fclose(fp);
}