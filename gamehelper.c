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


int getNumOfRound1Questions(FILE *file){
	rewind(file);
	char c;
	int sum = 0;
	do
   	{
      c = fgetc(file);
      if( feof(file) )
      {
         break ;
      }
      if(c == '*'){
      	sum++;
      }
   	}while(1);
   return sum;
}

QUESTION *getQuestions(FILE *file){
	rewind(file);
	int num_of_ques = getNumOfRound1Questions(file);
	rewind(file);

	QUESTION *quetions = malloc(sizeof(QUESTION)*num_of_ques);

	for(int i= 0 ;i< num_of_ques;i++){
		quetions[i].ques = malloc(sizeof(char)*512);
		for(int j=0 ;j<4;j++){
			quetions[i].choices[j] = malloc(sizeof(char)*256);
		}
		quetions[i].answ = malloc(sizeof(char)*16);
	}


	int i = -1;
	int j = 0; 
	char c;
	char *array[4];
	char *p;

	char *line = malloc(sizeof(char)*256);
	do{
      
      	c = fgetc(file);
      	if( feof(file) )
      	{
         	break ;
      	}
      	memset(line,'\0',(strlen(line)+1));
      	switch(c){
      		case NEW_QUESTION_CHAR:
  			i++;
      		break;
      		
      		case QUESTION_CHAR:
      		if(fgets(line,256,file) != NULL ){
				if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
      			strcpy(quetions[i].ques,line);
      		}
      		
      		break;

      		case CHOICES_CHAR:
      		if(fgets(line,256,file) != NULL ){
				if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
      			j = 0;
				p = strtok (line,CHOICE_SEPERATE_CHAR);
    			
    			while (p != NULL)
    			{
        			array[j++] = p;
        			p = strtok (NULL, CHOICE_SEPERATE_CHAR);
    			}

      		}
      		for(int j=0 ;j<4;j++){			
				strcpy(quetions[i].choices[j],array[j]);
				
			}
      		break;

      		case ANSWER_CHAR:
      		if(fgets(line,256,file) != NULL ){
				if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';
      			strcpy(quetions[i].answ,line);
      		}
      		
      		break;
      		default:
      		break;
      	}
   	}while(1);
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
	
	FILE *ques_file = NULL;

	if((ques_file = fopen(round1_ques_file,"r+")) == NULL){
		printf("ERROR:File can not be found!\n");
		return 0;
	}

	sqlite3** db = open_database();
	// get user
	char sql_get_user[] = "SELECT * FROM account;";
	select_query(sql_get_user, on_get_row, 0);
	
	num_of_round1_question = getNumOfRound1Questions(ques_file);	
	listQues = getQuestions(ques_file);

	arr_sended_ques = malloc(sizeof(int)*num_of_round1_question);
	arr_size = num_of_round1_question;

	//for random non repeating question
	for(int i=0; i<arr_size; i++){
		arr_sended_ques[i] = i;
	}

//	fclose(file);
	fclose(ques_file);
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