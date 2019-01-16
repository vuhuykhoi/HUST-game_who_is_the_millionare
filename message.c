#include "message.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>



message parseMessage(char *mgs){
	message result = newMessage();
	char *buff = malloc(sizeof(char)*(strlen(mgs)+1));
	strcpy(buff,mgs);
	char *token = strtok(buff,SEPARATOR_CHAR); 
   	//split string in numbers
    int count =0 ;
    while (token != NULL) 
    { 
        if(count == 0) result.msg_type = atoi(token); 
        if(count == 1)	strcpy(result.value,token);
        token = strtok(NULL, SEPARATOR_CHAR);
    	count++;
    }
    free(buff);
	return result;
}

message newMessage(){
	message msg;
	msg.msg_type = NONE;
	memset(msg.value,'\0',(strlen(msg.value)+1));
	return msg;
}
