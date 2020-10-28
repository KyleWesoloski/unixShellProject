/* 
	Name: Kyle Wesoloski
	CWID: 11873614
	CS 300, Project 1: Unix Shell and History Feature 
*/   

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#define MAX_LINE 80 																/* The maximum length command */

void printHistory(char** history, int index); 
int main(void)
{ 
char *args[MAX_LINE/2 + 1]; 														/* command line arguments */
char *hist[10000]; 																	//stores commands
int c = 0; 																			//index of commands in history
int should_run = 1;																	 /* flag to determine when to exit program */
while (should_run) {
	
int concurrent = 0;  																//flag to determine if parent waits
printf("osh>");
fflush(stdout);

char str[2500]; 																	//read in line
fgets(str, 2500, stdin); 


int len = strlen(str);

if (len > 1 && str[len-1] == '\n') {												//delete new line at end of input
    str[--len] = '\0';
}
else continue; 																		//no command is entered

int argc = 0; 
char *p = strtok(str, " ");															//parse string 
while(p != NULL){
	args[argc] = (char*)malloc(sizeof(char)*100); 
	args[argc] = p;   
	argc++; 
	p = strtok(NULL, " "); 
}



if(args[0][0]== '!'){  																//display command #
	if(args[0][1] != '!' && !isdigit(args[0][1])) {
		printf("No such command in history\n"); 
		continue; 
		}
	if(args[0][1] == '!') args[0][1] = c+'0';						
	if(atoi(&args[0][1]) == 0 || atoi(&args[0][1]) > c){							//check if expression after ! is valid
		printf("No such command in history\n"); 
		continue; 
	}
	else{																			//copy history command to args
		char str[2500];		
		strcpy(str, hist[atoi(&args[0][1]) - 1]);									//create copy of history command 
		if(args[1] != NULL && args[argc-1][0] == '&') strcat(strcat(str, " "),"&"); //add concurrent to previous history command
		argc = 0; 
		char *p = strtok(str, " ");													//parse command insert into args
		while(p != NULL){
			args[argc] = p;  			
			argc++; 
			p = strtok(NULL, " "); 
		}
	}
}

if(argc > 1 && *args[argc-1]=='&'){													//check for concurrent
	concurrent = 1; 
	args[argc-1] = NULL; 
}
else{ 	
	concurrent = 0; 
	args[argc] = NULL; 	
}

if(strcmp(args[0],"exit") == 0) {													//exits program upon request
	should_run = 0; 
	continue; 
}

pid_t pid;  
pid = fork(); 																		//create child process

if (pid < 0) {
	fprintf(stderr, "Fork Failed");  												//fork fails 
	should_run = 0; 
}	
else if (pid == 0) {

	if(strcmp(args[0], "history")==0){ printHistory(hist, c); exit(0); }		    //prints history and kills child process
	else { execvp(args[0], args); }	 												//child process
}

else { 		     																	//parent process 
	if(concurrent) args[argc-1] = "&";	
																					//appends & back to command for history
	if(strcmp(args[0], "history") != 0){
		hist[c] = (char*)malloc(strlen(args[0] + 1)); 							    //allocate memory to history
		for(int i = 0; i < argc; i++) {
			if(i != 0) strcpy(hist[c], strcat(strcat(hist[c], " "),args[i]));  		//store command in history
			else strcpy(hist[c],args[i]); 
		}			
		c++;
	}	
	
	if(!concurrent) waitpid(pid,NULL,0); 											//waits for child process
}
printf("\n"); 

}

for(int i = 0; i < c; i++) free(hist[i]); 											//free memory 
return 0;
}

void printHistory(char** history, int index){
	int max = 10;
	if(index==0){																	//no history 
		printf("No commands in history."); 
		return; 
	}
	printf("\n"); 
	while(index>0){																	//print history
		printf("%d %s\n", index, history[index-1]);
		index--; 
		max--; 
		if(max == 0) return; 
	}
	

}
