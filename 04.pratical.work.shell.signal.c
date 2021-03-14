#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX 100

char args_buff[MAX];
char *args[MAX];
int globalPid=0;

int getArg(void);
int sendSignal(int pid, int signal);
void handler(int signal);


int main() {
	// signal register
	// ctrl + z
	signal(SIGTSTP, handler);

	// ctrl + c or killed
	signal(SIGINT, handler);
	signal(SIGTERM, handler);
	
	// program running
	printf("\n****LH Shell****\n");
	while(1){
		printf("> ");
		int argc = getArg();
		int pid = fork();
		globalPid = pid; // register your pid

		if(pid == 0) { // this is child who execute shell			   
			// child do it job
			execvp(args[0], args);
		} 
		else { // this is parent who wait for next comment
			
			wait(&pid);
		}
	}
	return 0;
}


int getArg(void){
	// clean buff
	memset(args_buff, '\0', MAX);
	memset(args, 0, MAX);

	// build buff
	gets(args_buff);
	
	// cut string
	char pre = ' ';
	int index = 0;
	int len = strlen(args_buff);
	for(int i=0; i<len; i++){
		if(args_buff[i] != ' ' && pre == ' ') { // detect arg
			args[index++] = &args_buff[i];
		}
		pre = args_buff[i]; // record previous char
		if(args_buff[i] == ' ') args_buff[i] = '\0'; // cut string
	}
	args[index] = NULL;
	return index;
}


int sendSignal(int pid, int signal){
	return kill(pid, signal);
}


void handler(int signal){
	if(globalPid != 0 && signal != SIGINT) {
		sendSignal(globalPid, signal);
		return;
	}
	
	printf("[ SIGNAL %d ] ", signal);
	switch(signal){
		case SIGTSTP:
			printf("pause\n");
			break;
		case SIGINT:
			printf("Terminated shell\n");
			exit(0);
			break;
		case SIGTERM:
			printf("Terminated process\n");
			exit(0);
			break;
	}
}
