#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "message.h"
#include "gamehelper.h" 

#define BUFF_SIZE 1024


message recv_message;

int client_sock;
int SERVER_PORT;
char SERVER_ADDR[20];

char current_user[USER_MAX_LENGTH];
char buff[BUFF_SIZE];
char buff_message[BUFF_SIZE];
	
int msg_len = 0,
	bytes_sent = 0,
	bytes_received = 0 ;

enum switch_screen {
	MAIN_MENU_SCREEN,
	REGISTER_SCREEN,
	LOGIN_SCREEN,
	USER_MENU_SCREEN,
	HIGH_SCORE_SCREEN,
	WAITING_ROOM_SCREEN,
	READY_SCREEN,
	PLAYING_ROOM_ROUND_1_SCREEN,
	MAIN_PLAYING_ROOM_ROUND_2_SCREEN,
	SUB_PLAYING_ROOM_ROUND_2_SCREEN,
	EXIT_GAME,
	GAMEOVER_SCREEN,
	FULL_ROOM_SCREEN,

};

/*--------------SOCKET MESSAGE TRANFER--------------------------*/
int receiveData(int s, char *buff, int size, int flags);
int sendData(int s, char *buff, int size, int flags);


/*--------------PROCESS INTERFACE-------------------------------*/
int processMainMenuScreen();
int processUserMenuScreen();
int processLoginScreen();
int processWaittingRoomScreen();
int processPlayingRoomRound1Screen();
int processMainPlayingRoomRound2Screen();

int processSubPlayingRoomRound2Screen();
	
int processHighScoreScreen();
int processRegisterScreen();

int processReadyScreen();
int processGameOverScreen();
int processFullRoomScreen();

/*-------------INTERFACE---------------------------------------*/
void printMainMenuScreen();
void printUserMenuScreen();
void printLoginScreen();
void printRegisterScreen();
void printHighScoreScreen(char* score);
void printWaittingRoomScreen();
void printReadyScreen(int present_number, int full_number);
void printPlayingRoomRound1Screen(char *ques);
void printMainPlayingRoomRound2Screen(char *ques);
void printSubPlayingRoomRound2Screen();
void printGameOverScreen();
void printFullRoomScreen();


/*------------------------------------------------------------*/

void litmittedTimeInput(char* buff,int timeLimitted);
/*------------------------------------------------------------*/

int connectToServer(){
	struct sockaddr_in server_addr; /* server's address information */

	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately!\n");
		exit(1);
	}
}

int main(int argc,char *argv[]){

	if(argc !=3){
		printf("Invalid Argument!\n");
		exit(1);
	}

	strcpy(SERVER_ADDR,argv[1]);
	SERVER_PORT = atoi(argv[2]);

	connectToServer();

	int screen_controller = MAIN_MENU_SCREEN;

	do{
		switch (screen_controller){
		
		case MAIN_MENU_SCREEN:
		screen_controller = processMainMenuScreen();
		break;
		
		case LOGIN_SCREEN:
		screen_controller = processLoginScreen();
		break;
		
		case USER_MENU_SCREEN:
		screen_controller = processUserMenuScreen();
		break;
		
		case WAITING_ROOM_SCREEN:
		screen_controller = processWaittingRoomScreen();
		break;

		case PLAYING_ROOM_ROUND_1_SCREEN:
		screen_controller = processPlayingRoomRound1Screen();
		break;

		case HIGH_SCORE_SCREEN:
		screen_controller = processHighScoreScreen();
		break;

		case MAIN_PLAYING_ROOM_ROUND_2_SCREEN:
		screen_controller = processMainPlayingRoomRound2Screen();
		break;

		case SUB_PLAYING_ROOM_ROUND_2_SCREEN:
		screen_controller = processSubPlayingRoomRound2Screen();
		break;

		case REGISTER_SCREEN:
		screen_controller = processRegisterScreen();
		break;

		case READY_SCREEN:
		screen_controller = processReadyScreen();
		break;

		case GAMEOVER_SCREEN:
		screen_controller = processGameOverScreen();
		break;		

		case FULL_ROOM_SCREEN:
		screen_controller = processFullRoomScreen();
		break;
		}

	}while(screen_controller != EXIT_GAME);

	//Step 5: Close socket
	close(client_sock);
	return 0;
}

int processFullRoomScreen(){
	int menu = 0;
	do{
		printFullRoomScreen();
		scanf("%d",&menu); 
	}while(menu!=1);
	return USER_MENU_SCREEN;
}
void printFullRoomScreen(){
	printf("----------------------\n");
	printf("\nROOM IS FULL\n");
	printf("Press 1 go to main menu\n");
	printf("----------------------\n");
}

int processReadyScreen(){
	int menuSelected = 0;
	int member_in_room = 0;
	
	strcpy(buff,"NONE");
	memset(buff_message,'\0',(strlen(buff_message)+1));

	sprintf(buff_message,"%d%s%s",GET_IN_ROOM,SEPARATOR_CHAR,buff);
	//SEND message to server
	sendData(client_sock,buff_message,strlen(buff_message),0);

	//RECEIVE echo reply
	while(1){
		bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
		buff_message[bytes_received] = '\0';
		recv_message = parseMessage(buff_message);
		
		if(recv_message.msg_type == NUM_PLAYER_IN_ROOM){
			member_in_room = atoi(recv_message.value);
			if(member_in_room <= NUM_FULL_ROOM){
				printReadyScreen(member_in_room,NUM_FULL_ROOM);		
				if(member_in_room == NUM_FULL_ROOM){
					return PLAYING_ROOM_ROUND_1_SCREEN;
				} 		
			}
		}else if(recv_message.msg_type == ROOM_IS_FULL){
			return FULL_ROOM_SCREEN;
		}

	}

}

int processRegisterScreen(){
	int menuSelected = 0;
	char user_tmp[USER_MAX_LENGTH];
	user_tmp[0]='\0';
	do{
		printRegisterScreen();
		scanf("%d",&menuSelected);
		getchar();//clear '\n' input 
		
		switch(menuSelected){
			case 1:
				printf("USERNAME:");
				memset(buff,'\0',(strlen(buff)+1));
				fgets(buff, BUFF_SIZE, stdin);	
				buff[strlen(buff)-1] = '\0';	
				msg_len = strlen(buff);
				if (msg_len == 0) break;
				
				memset(buff_message,'\0',(strlen(buff_message)+1));
				sprintf(buff_message,"%d%s%s",REGISTER_USER,SEPARATOR_CHAR,buff);
				//SEND message to server
				sendData(client_sock,buff_message,strlen(buff_message),0);
				
				//RECEIVE echo reply
				bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
				buff_message[bytes_received] = '\0';
	
				recv_message = parseMessage(buff_message);
				if(recv_message.msg_type == INVALID_REGISTER_USER){
					printf("username has been exsisted\n");
				}else if(recv_message.msg_type == VALID_REGISTER_USER){
					strcpy(user_tmp,buff);
				}

				break;
			case 2:
				if(user_tmp[0] =='\0'){
					printf("Please input register username first\n");
					return REGISTER_SCREEN;
				}else{
					//SEND PASS:
					printf("PASS:");
					memset(buff,'\0',(strlen(buff)+1));
					fgets(buff, BUFF_SIZE, stdin);	
					buff[strlen(buff)-1] = '\0';	
					msg_len = strlen(buff);
					if (msg_len == 0) break;

					memset(buff_message,'\0',(strlen(buff_message)+1));
					sprintf(buff_message,"%d%s%s",REGISTER_PASS,SEPARATOR_CHAR,buff);
					//SEND message to server
					sendData(client_sock,buff_message,strlen(buff_message),0);
					//RECEIVE echo reply
					bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
					buff_message[bytes_received] = '\0';
					
					recv_message = parseMessage(buff_message);
					if(recv_message.msg_type == REGISTER_SUCCESS){
						printf("register success\n");
						return MAIN_MENU_SCREEN;	
					}			
				}
				
			default:
				return MAIN_MENU_SCREEN;
		} 
	}while(menuSelected == 1);
	return 0;
}

void printRegisterScreen(){
	printf("\n---------------------------------\n");
		printf("REGISTER SCREEN:\n");
		printf("\t1.input username\n");
		printf("\t2.input password\n");
		printf("Your choice(1-2,other to back to main menu screen)\n");
	printf("---------------------------------\n");	
}

int processSubPlayingRoomRound2Screen(){
	printSubPlayingRoomRound2Screen();
	while(1){
		//RECEIVE 
		memset(buff_message,'\0',(strlen(buff_message)+1));
		bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
		buff_message[bytes_received] = '\0';

		recv_message = parseMessage(buff_message);
		switch(recv_message.msg_type){
			case ROUND2_QUES:
			printf("------------------------------------------\n");
			printf("Question %s\n",recv_message.value);
			break;
			case ROUND2_QUES_50_50:
			printf("------------------------------------------\n");
			printf("Main player selected help 50/50\n");
			printf("Question %s\n",recv_message.value);
			break;
			case ROUND2_ANSW:
			printf("Correct answer:%s\n",recv_message.value);
			break;
			case ROUND2_SCORE:
			printf("Main player score:%d\n",atoi(recv_message.value));
			break;
			case ROUND2_FINISH:
			return GAMEOVER_SCREEN;
		}
	}
} 

void printSubPlayingRoomRound2Screen(){
	printf("--------------------------------------\n");
	printf("ROUND 2 START!\n");

}

int processMainPlayingRoomRound2Screen(){	
	char c;
	printf("------------------------------------------------------\n");
	printf("In round 2 main player have %d to answer the question.\n",TIME_ANSWER_ROUND_2_QUES);
	printf("And have two help selection:\n");
	printf("button S to stop round 2 and save the score\n");
	printf("button H to remove two wrong answer\n");
	
	do{
		printf("Are you ready[y/n]?\n");
		scanf("%c",&c);
		getchar();
	}while(c != 'y');

	printf("ROUND 2 WILL BE STARTED IN 3 SECONDS...\n");
	sleep(1);
	for(int i= 0 ;i<3;i++){
		printf("%d\n",3-i);
		fflush(stdout);
		sleep(1);
	}

	printf("ROUND 2 START!\n");

	while(1){
		/*----GET ROUND 2 QUESTION----*/
		strcpy(buff,"NONE");
		memset(buff_message,'\0',(strlen(buff_message)+1));
		sprintf(buff_message,"%d%s%d",GET_ROUND2_QUES,SEPARATOR_CHAR,buff);
		//SEND 
		sendData(client_sock,buff_message,strlen(buff_message),0);
		//RECEIVE 
		memset(buff_message,'\0',(strlen(buff_message)+1));
		bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
		buff_message[bytes_received] = '\0';

		recv_message = parseMessage(buff_message);
		
		if(recv_message.msg_type == ROUND2_QUES){
			do{
				printMainPlayingRoomRound2Screen(recv_message.value);
				litmittedTimeInput(buff,TIME_ANSWER_ROUND_2_QUES);

				memset(buff_message,'\0',(strlen(buff_message)+1));
				sprintf(buff_message,"%d%s%s",ANSWER_ROUND2_QUES,SEPARATOR_CHAR,buff);
				
				//SEND 
				sendData(client_sock,buff_message,strlen(buff_message),0);
				
				//RECEIVE
				bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
				buff_message[bytes_received] = '\0';
				recv_message = parseMessage(buff_message);

			}while(recv_message.msg_type == ROUND2_QUES_50_50);
			
			while(recv_message.msg_type == HELP_50_50_USED){
				printf("Help 50/50 had been used.\n");
				printf("YOUR ANSWER:\n");
				litmittedTimeInput(buff,TIME_ANSWER_ROUND_2_QUES);

				memset(buff_message,'\0',(strlen(buff_message)+1));
				sprintf(buff_message,"%d%s%s",ANSWER_ROUND2_QUES,SEPARATOR_CHAR,buff);
				
				//SEND 
				sendData(client_sock,buff_message,strlen(buff_message),0);
				
				//RECEIVE
				bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
				buff_message[bytes_received] = '\0';
				recv_message = parseMessage(buff_message);
			}
			if(recv_message.msg_type == ROUND2_ANSW){
				printf("Correct answer :%s\n",recv_message.value);

				/*----GET ROUND 2 RESULT----*/
				memset(buff_message,'\0',(strlen(buff_message)+1));
				sprintf(buff_message,"%d%s%s",GET_ROUND2_RESULT,SEPARATOR_CHAR,"none");
				//SEND 
				sendData(client_sock,buff_message,strlen(buff_message),0);
				//RECEIVE
					
				bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
				buff_message[bytes_received] = '\0';
				recv_message = parseMessage(buff_message);
			}
			if(recv_message.msg_type == ROUND2_SCORE){
				printf("Your score: %d\n",atoi(recv_message.value));
			}
		}else if(recv_message.msg_type == ROUND2_FINISH){
			return GAMEOVER_SCREEN;
		}
	}
	
}

int processGameOverScreen(){
	int menu = 0;
	
	do{
		printGameOverScreen();
		clearBuffer();
		scanf("%d",&menu); 
	}while(menu!=1);

	memset(buff_message,'\0',(strlen(buff_message)+1));
	sprintf(buff_message,"%d%s%s",GET_OUT_ROOM,SEPARATOR_CHAR,"none");
	//SEND 
	sendData(client_sock,buff_message,strlen(buff_message),0);

	return USER_MENU_SCREEN;	
}
void printGameOverScreen(){
	printf("----------------------\n");
	printf("\nGAME OVER\n");
	printf("Press 1 go to main menu\n");
	printf("----------------------\n");
}

void printMainPlayingRoomRound2Screen(char *ques){
	printf("------------------------------------------\n");
	printf("QUESTION %s\n",ques);
	printf("------------------------------------------\n");
	printf("HELP:\n");
	printf("button S to stop this game\n");
	printf("button H to remove two wrong answer\n");
	printf("------------------------------------------\n");
	printf("YOUR ANSWER:\n");
}

int processHighScoreScreen(){
	int menuSelected = 0;
	memset(buff_message,'\0',(strlen(buff_message)+1));
	sprintf(buff_message,"%d%s%s",GET_HIGH_SCORE,SEPARATOR_CHAR,buff);
	//SEND message to server
	sendData(client_sock,buff_message,strlen(buff_message),0);
	//RECEIVE echo reply
	bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
	buff_message[bytes_received] = '\0';
					
	recv_message = parseMessage(buff_message);
	if(recv_message.msg_type == SCORE_BOARD){
	}			
	do{
		printHighScoreScreen(recv_message.value);
		//printf("%s\n",recv_message.value);
		scanf("%d",&menuSelected);
		getchar();//clear '\n' input
		switch(menuSelected){
			case 1:
			return USER_MENU_SCREEN;
		}
	}while(1);
}

int processPlayingRoomRound1Screen(){	
	strcpy(buff,"NONE");
	
	/*----GET ROUND 1 QUESTION----*/
	memset(buff_message,'\0',(strlen(buff_message)+1));

	sprintf(buff_message,"%d%s%d",GET_ROUND1_QUES,SEPARATOR_CHAR,buff);
	//SEND 
	sendData(client_sock,buff_message,strlen(buff_message),0);
	//RECEIVE 
	bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
	buff_message[bytes_received] = '\0';		

	recv_message = parseMessage(buff_message);

	if(recv_message.msg_type == ROUND1_QUES){
		printPlayingRoomRound1Screen(recv_message.value);	
	
		litmittedTimeInput(buff,TIME_ANSWER_ROUND_1_QUES);

		/*-----ANSWER ROUND 1 QUESTION-----*/
		memset(buff_message,'\0',(strlen(buff_message)+1));
		sprintf(buff_message,"%d%s%s",ANSWER_ROUND1_QUES,SEPARATOR_CHAR,buff);
		//SEND 
		sendData(client_sock,buff_message,strlen(buff_message),0);
		//RECEIVE
		bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
		buff_message[bytes_received] = '\0';
		recv_message = parseMessage(buff_message);

		printf("Correct answer :%s\n",recv_message.value);


		/*----GET ROUND 1 RESULT----*/
		memset(buff_message,'\0',(strlen(buff_message)+1));
		sprintf(buff_message,"%d%s%s",GET_ROUND1_RESULT,SEPARATOR_CHAR,"none");
		//SEND 
		sendData(client_sock,buff_message,strlen(buff_message),0);
		//RECEIVE
			
		bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
		buff_message[bytes_received] = '\0';

		recv_message = parseMessage(buff_message);
		if(recv_message.msg_type == ROUND2_PLAYER){
			printf("Chuc mung nguoi choi %s tro thanh nguoi choi chinh\n",recv_message.value);
		/*CHECK IF PASS ROUND 1,CONTINUE TO ROUND 2*/
		if(!strcmp(current_user,recv_message.value))
			return MAIN_PLAYING_ROOM_ROUND_2_SCREEN;
		else
			return SUB_PLAYING_ROOM_ROUND_2_SCREEN;	
		}else if(recv_message.msg_type == GAME_OVER){
			return GAMEOVER_SCREEN;
		}
		
	}

}

void printPlayingRoomRound1Screen(char *ques){
	printf("You have %d to read and answer the question\n",TIME_ANSWER_ROUND_1_QUES);
	printf("ROUND 1 WILL BE STARTED IN 3 SECONDS...\n");
	sleep(1);
	for(int i= 0 ;i<3;i++){
		printf("%d\n",3-i);
		fflush(stdout);
		sleep(1);
	}
	
	printf("QUESTION:%s\n",ques);
	printf("YOUR ANSWER:\n");
}

int processWaittingRoomScreen(){
	int menuSelected = 0;
	
	printWaittingRoomScreen();
	scanf("%d",&menuSelected);
	getchar();//clear '\n' input 
		
	switch(menuSelected){
		case 1:
			return READY_SCREEN;
		default:
			return USER_MENU_SCREEN;	
	}
}

int processMainMenuScreen(){
	int menuSelected = 0;
	do{
		printMainMenuScreen();
		scanf("%d",&menuSelected);
		getchar();//clear '\n' input 
		
		switch(menuSelected){
			case 1:
				return REGISTER_SCREEN;
			case 2:
				return LOGIN_SCREEN;
			default:
				return EXIT_GAME;
		}
	}while(menuSelected==1||menuSelected==2);
	return 0;
}

int processUserMenuScreen(){
	int menuSelected = 0;
	do{
		printUserMenuScreen();
		scanf("%d",&menuSelected);
		getchar();//clear '\n' input 
		
		switch(menuSelected){
			case 1:
				return WAITING_ROOM_SCREEN;
			case 2:
				return HIGH_SCORE_SCREEN;
			case 3:
				//SEND LOGOUT MESSAGE
				memset(buff_message,'\0',(strlen(buff_message)+1));
				sprintf(buff_message,"%d%s%s",LOG_OUT_USER,SEPARATOR_CHAR,current_user);
				//SEND message to server
				sendData(client_sock,buff_message,strlen(buff_message),0);
				
				//RECEIVE echo reply
				bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
				buff_message[bytes_received] = '\0';
				

				recv_message = parseMessage(buff_message);
				if(recv_message.msg_type == LOGOUT_SUCCESS){
					memset(current_user,'\0',(strlen(current_user)+1));
					return MAIN_MENU_SCREEN;
				}
				return USER_MENU_SCREEN;

			default:
				return MAIN_MENU_SCREEN;
		}
	}while(menuSelected==1||menuSelected==2);
}

int processLoginScreen(){
	int menuSelected = 0;
	char user_tmp[USER_MAX_LENGTH];
	user_tmp[0]='\0';
	
	do{
		printLoginScreen();
		scanf("%d",&menuSelected);
		getchar();//clear '\n' input 
		
		switch(menuSelected){
			case 1:
				printf("USERNAME:");
				memset(buff,'\0',(strlen(buff)+1));
				fgets(buff, BUFF_SIZE, stdin);	
				buff[strlen(buff)-1] = '\0';	
				msg_len = strlen(buff);
				if (msg_len == 0) break;
				
				memset(buff_message,'\0',(strlen(buff_message)+1));
				sprintf(buff_message,"%d%s%s",LOG_IN_USER,SEPARATOR_CHAR,buff);
				//SEND message to server
				sendData(client_sock,buff_message,strlen(buff_message),0);
				
				//RECEIVE echo reply
				bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
				buff_message[bytes_received] = '\0';
			

				recv_message = parseMessage(buff_message);
				if(recv_message.msg_type == VALID_LOGIN_USER){
					printf("username exist!\nPress 2 to enter password\n");
					strcpy(user_tmp,buff);
				}else if(recv_message.msg_type == USER_ALREADY_LOGIN){
					printf("user already loggined\n");
				}
				else if(recv_message.msg_type == INVALID_LOGIN_USER)
				{
					memset(user_tmp,'\0',(strlen(buff)+1));
					printf("username not exist!");
				}
				break;
			case 2:
				if(user_tmp[0] == '\0'){
					printf("Input username first");
					return LOGIN_SCREEN;
				}else{
					//SEND PASS:
					printf("PASS:");
					memset(buff,'\0',(strlen(buff)+1));
					fgets(buff, BUFF_SIZE, stdin);	
					buff[strlen(buff)-1] = '\0';	
					msg_len = strlen(buff);
					if (msg_len == 0) break;
					memset(buff_message,'\0',(strlen(buff_message)+1));
					sprintf(buff_message,"%d%s%s",LOG_IN_PASS,SEPARATOR_CHAR,buff);
					//SEND message to server
					sendData(client_sock,buff_message,strlen(buff_message),0);
					
					//RECEIVE echo reply
					bytes_received =  receiveData(client_sock,buff_message,BUFF_SIZE-1,0);
					buff_message[bytes_received] = '\0';
					

					recv_message = parseMessage(buff_message);
					if(recv_message.msg_type == LOGIN_SUCCESS){
						strcpy(current_user,user_tmp);
						printf("current_user:%s\n",current_user);
						return USER_MENU_SCREEN;
					}else if(recv_message.msg_type == INVALID_LOGIN_PASSWORD){
						printf("Invalid password");
						return LOGIN_SCREEN;	
					}
				}
			default:
				return MAIN_MENU_SCREEN;
		} 
	}while(menuSelected == 1);
	return 0;
}

void printHighScoreScreen(char* score){
	printf("\n---------------------------------\n");
	printf("TOP 10 HIGH SCORE\n");
	printf("---------------------------------\n");
	printf("USERNAME  | SCORE | PLAYTIME\n");
	printf("---------------------------------\n");
	if(score!=NULL)
	{
	printf("%s\n",score);
	}
	printf("(press  key 1 to exit)\n");
	printf("---------------------------------\n");
}

void printMainMenuScreen(){
	printf("\n---------------------------------\n");
		printf("USER MANAGEMENT PROGRAM\n");
		printf("\t1.Register\n");
		printf("\t2.Sign in\n");
		printf("Your choice(1-2,other to quit)\n");
	printf("---------------------------------\n");
}

void printUserMenuScreen(){
	printf("\n---------------------------------\n");
		printf("USER MANAGEMENT PROGRAM\n");
		printf("\t1.GET IN ROOM\n");
		printf("\t2.VIEW HIGH SCORE\n");
		printf("\t3.LOG OUT\n");
		printf("Your choice(1-3,other to back to main menu screen)\n");
	printf("---------------------------------\n");
}

void printLoginScreen(){
	printf("\n---------------------------------\n");
		printf("LOGIN SCREEN:\n");
		printf("\t1.input username\n");
		printf("\t2.input password\n");
		printf("Your choice(1-2,other to back to main menu screen)\n");
	printf("---------------------------------\n");	
}


void printWaittingRoomScreen(){
	printf("\n---------------------------------\n");
		printf("WAITTING ROOM:\n");
		printf("\t1.ready to start\n");
		printf("Your choice(1,other to back to user menu screen)\n");
	printf("---------------------------------\n");	
}

void printReadyScreen(int present_number, int full_number){
	printf("\n---------------------------------\n");
	printf("WAITTING ROOM\n");
	printf("member in room:%d/%d\n",present_number,full_number);
	printf("Waitting...\n");
	printf("(press  key 1 to unready)\n");
	printf("---------------------------------\n");
}

int receiveData(int s, char *buff, int size, int flags){
	int n;
	n = recv(s, buff, size, flags);
	if (n < 0)
		perror("Error: ");
	return n;
}	

int sendData(int s, char *buff, int size, int flags){
	int n;
	n = send(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}


void litmittedTimeInput(char* buff, int timeLimitted){
   fd_set rfds;
    struct timeval tv;
    int retval, len;
 

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* Wait up to five seconds. */
    tv.tv_sec = timeLimitted;
    tv.tv_usec = 0;

    retval = select(1, &rfds, NULL, NULL, &tv);

    if (retval == -1){
        perror("select()");
        exit(EXIT_FAILURE);
    }
    else if (retval){
        /* FD_ISSET(0, &rfds) is true so input is available now. */

        /* Read data from stdin using fgets. */
        fgets(buff, sizeof(buff), stdin);

        /* Remove trailing newline character from the input buffer if needed. */
        len = strlen(buff) - 1;
        if (buff[len] == '\n')
            buff[len] = '\0';
    }
    else
        strcpy(buff,"NONE");
}
