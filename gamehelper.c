#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "User_ll.h"
#include "gamehelper.h"
#include "database.h"


int num_of_round1_question;
int random_ques;
int round2_score;
int round2_ques_number;

int help_50_50_used;
char main_player[USER_MAX_LENGTH];
int *arr_sended_ques;
int arr_size;


int num_of_player;
int num_players_get_round1_ques;
int num_round1_ans;

linkList *accList;
sqlite3** db;	
QUESTION *listQues;

static int on_get_row(void* NotUsed, int num_cols, char** row, char** cols) {
	val El;
	El = newVal(row[0],row[1]);
	if(accList!=NULL) addToLastOfList(accList,El);	
	printf("%s-%s\n", row[0], row[1]);
	return 0;
}	

/*numberOfQues return number of questions*/
static int on_get_number_of_ques(void* numberOfQues, int num_cols, char** row, char** cols) {
	char* number= (char*) numberOfQues;
	strcpy(number,"");
	strcpy(number,row[0]);
	return 0;
}

/*ques return the question and all choice and answer*/
static int on_get_question(void* ques,int num_cols,char** row, char** cols)
{

	char*question=(char*)ques;
	strcpy(question,"");
	strcat(question,row[0]);
	strcat(question,row[1]);
	strcat(question,row[2]);
	strcat(question,row[3]);
	strcat(question,row[4]);
	strcat(question,row[5]);

}
int getNumOfRound1Questions(){
	char numberOfQuestion[2]="";
	sqlite3** db = open_database();
	char sql_get_user[] = "SELECT id FROM question;";
	select_query(sql_get_user, on_get_number_of_ques, &numberOfQuestion);
	return atoi(numberOfQuestion);
}

QUESTION *getQuestions()
{
	char numberOfQuestion[2]="";
	sqlite3** db = open_database();
	char sql_get_user[] = "SELECT id FROM question;";
	select_query(sql_get_user, on_get_number_of_ques, &numberOfQuestion);
	//printf("%d\n",atoi(numberOfQuestion));
	QUESTION *quetions = malloc(sizeof(QUESTION)*atoi(numberOfQuestion));

	for(int i= 0 ;i< atoi(numberOfQuestion);i++){
		quetions[i].ques = malloc(sizeof(char)*512);
		for(int j=0 ;j<4;j++){
			quetions[i].choices[j] = malloc(sizeof(char)*256);
		}
		quetions[i].answ = malloc(sizeof(char)*16);
	}

	for(int i= 0 ;i< 16;i++)
	{

			char ques[500]="";
			char* id=(char*)malloc(sizeof(char));
			sprintf(id,"%d",i+1);
			char* sql = (char*)malloc(MAX_STRING_LENGTH_SQL);
			strcpy(sql, "");
			strcat(sql, "SELECT ques,choicea,choiceb,choicec,choiced,answ from QUESTION where id = ");
			strcat(sql,id);
			strcat(sql,";");
			//printf("%s\n",sql);
			select_query(sql, on_get_question, &ques);
			//printf("%s\n",ques);
			const char s[2] = "@";
		    char *token;
		   
		    /* lay token dau tien */
		    token = strtok(ques, s);

		    //printf( "%s\n", token );
		    strcpy(quetions[i].ques,token);
		    
		    token = strtok(NULL, s);
		    //printf( "%s\n", token );
		    strcpy(quetions[i].choices[0],token);
		    
		    token = strtok(NULL, s);
		    //printf( "%s\n", token );
		    strcpy(quetions[i].choices[1],token);
		    
		    token = strtok(NULL, s);
		    //printf( "%s\n", token );
		    strcpy(quetions[i].choices[2],token);

		    token = strtok(NULL, s);
		    //printf( "%s\n", token );
		    strcpy(quetions[i].choices[3],token);

		    token = strtok(NULL, s);
		    //printf( "%s\n", token );
		    strcpy(quetions[i].answ,token);	
		    

	}



	close_database(db);
	return quetions;
}



void printQuestions(QUESTION *listQues,int total){

	for(int i = 0 ;i< total;i++){
		printf("QUESTION %d:%s\n",i,listQues[i].ques);
	}
}

int countScore(int ques_number){
	switch(ques_number){
		case 0:
		return 0;
		case 1:
		return 20;
		case 2:
		return 40;
		case 3:
		return 60;
		case 4:
		return 100;
		case 5:
		return 200;
		case 6:
		return 300;
		case 7:
		return 600;
		case 8:
		return 1000;
		case 9:
		return 1400;
		case 10:
		return 2200;
		case 11:
		return 3000;
		case 12:
		return 4000;
		case 13:
		return 6000;
		case 14:
		return 8500;
		case 15:
		return 15000;
	}

}

int convertAnswerToNumber(char c){
	switch(c){
		case 'A':
		return 0;
		case 'B':
		return 1;
		case 'C':
		return 2;
		case 'D':
		return 3;

	}
}


char* removeTwoWrongAnswer(QUESTION q){
	char *out = malloc(sizeof(char)*256);
	int arr[3];
	
	int correct_answ = convertAnswerToNumber(q.answ[0]);

	//add 3 wrong answer to arr
	int j = 0;
	for(int i=0;i<4;i++){
		if(i!= correct_answ){
			arr[j] = i;
			j++;
		}		
	}
	
	//random 1 wrong answer from wrong answer arr 
	time_t t;
	srand((unsigned) time(&t));
	int answ = rand()%3;

	//make return value: question and two answer 
	strcpy(out,q.ques);

	//print correct answ and 1 wrong answer to out
	for(int i=0;i<4;i++){
		if(i == correct_answ || i== arr[answ]){
			strcat(out,"\n");	
			strcat(out,q.choices[i]);
		}
	}

	return out;
}


int deleteElmentFromArray(int *arr,int *size,int pos){
    for(int i=pos-1; i<(*size)-1; i++){
        arr[i] = arr[i + 1];
    }

  	(*size)--;
}

int randomQuestionNonRepeat(int *arr,int *size){
	int i = 0;
	time_t t;
	srand((unsigned) time(&t));
	int pos = rand()%(*size);
	int ques = arr[pos];
	deleteElmentFromArray(arr,size,pos+1);
	return ques;
}

int gameSetup(){	
	num_of_round1_question = 0 ;
	random_ques = 0;
	round2_score = 0 ;
	round2_ques_number = 0;

	help_50_50_used =0 ;
	memset(main_player,'\0',(strlen(main_player)+1));

	accList=newLinkList();
	listQues = NULL;

	num_of_player = 0;
	num_players_get_round1_ques = 0;
	num_round1_ans = 0;
	

	sqlite3** db = open_database();
	// get user
	char sql_get_user[] = "SELECT * FROM account;";
	select_query(sql_get_user, on_get_row, 0);
	
	num_of_round1_question = getNumOfRound1Questions();	
	listQues = getQuestions();

	arr_sended_ques = malloc(sizeof(int)*num_of_round1_question);
	arr_size = num_of_round1_question;

	//for random non repeating question
	for(int i=0; i<arr_size; i++){
		arr_sended_ques[i] = i;
	}


	return 1;
}

int resetGameVariables(){
	
	random_ques = 0;
	
	round2_ques_number = 0;

	help_50_50_used =0 ;
	memset(main_player,'\0',(strlen(main_player)+1));

	

	num_of_player = 0;
	num_players_get_round1_ques = 0;
	num_round1_ans = 0;
	
	

	arr_sended_ques = malloc(sizeof(int)*num_of_round1_question);
	arr_size = num_of_round1_question;
	
	//for random non repeating question
	for(int i=0; i<arr_size; i++){
		arr_sended_ques[i] = i;
	}

	return 1;
}

char *getCurrentTime(){
	time_t rawtime;
  	struct tm * timeinfo;

  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
  	return asctime(timeinfo);
}

int savePlayerScore(char* username,int score,char *time){

	char* scr;
	sprintf(scr,"%d",score);
	char* sql = (char*)malloc(MAX_STRING_LENGTH_SQL);
	strcpy(sql, "");
	strcat(sql, "INSERT INTO userscore(id, name, score, time) VALUES (NULL,'");
	strcat(sql, username);
	strcat(sql, "', ");
	strcat(sql, scr);
	strcat(sql,", '");
	strcat(sql, time);
	strcat(sql, "');");
	printf("%s\n",sql);	
	insert_query(sql);
	
	return 1;
}

void clearBuffer(){
	int c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}