#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

#define MAX 100

char args_buff[MAX];
char *args[MAX];
int globalPid=0;
int len = 0; // count length of input command
int cursor = 0; // current cursor position
int pipeFlag = 0; // flag used to detect pipe
int prePipe = -1; // detect previous command is pipe or not ?
int inputFlag = 0; // detect input

int pipe_buff[2];

int getArg(void);
int sendSignal(int pid, int signal);
void handler(int signal);
void *shell_thread(void *vargp);
void *input_thread(void *vargp);


int main(){
	printf("\n****LH Shell****\n");

	// init thread_id
	pthread_t shell_id, input_id;

	// create thread
	pthread_create(&input_id, NULL, input_thread, NULL);
	pthread_create(&shell_id, NULL, shell_thread, NULL);

	// wait for thread
	pthread_join(input_id, NULL);
	pthread_join(shell_id, NULL);
	return 0;
}


void *input_thread(void *vargp) {
	while(1){
		while(inputFlag != 0);
		int argc = getArg();
		inputFlag = 1;
	}		
}


void *shell_thread(void *vargp) {
	// signal register
	// ctrl + z
	signal(SIGTSTP, handler);

	// ctrl + c or killed
	signal(SIGINT, handler);
	signal(SIGTERM, handler);
	
	// program running
	while(1){
		if(pipeFlag == 0) printf("> ");
		while(inputFlag != 1);
		
		// support pipe
		if(pipeFlag == 1) pipe(pipe_buff);

		int pid = fork();
		globalPid = pid; // register your pid

		if(pid == 0) { // this is child who execute shell			   
			// pipe support
			if(pipeFlag == 1){
				dup2(pipe_buff[1], 1);
				close(pipe_buff[0]);
				close(pipe_buff[1]);
			}
			if(prePipe >= 0){
				dup2(prePipe, 0);
				close(prePipe);
			}
			
			// child do it job
			execvp(args[0], args);
		} 
		else { // this is parent who wait for next comment
			wait(&pid); // wait for process done
			
			if(pipeFlag == 1) close(pipe_buff[1]);
			if(pipeFlag == 1) prePipe = pipe_buff[0];
			else{
				if(prePipe >= 0) close(prePipe);
				prePipe = -1;
			}
			inputFlag = 0;
		}
	}
}


int getArg(void){
	// clean buff
	memset(args, 0, MAX);

	// build buff
	if(cursor == len){
		memset(args_buff, '\0', MAX);
		fgets(args_buff, MAX, stdin);
		len = strlen(args_buff);
		cursor = 0;
	}
	
	// cut string
	char pre = ' ';
	int index = 0;
	while(cursor < len){
		if((args_buff[cursor] != ' ' && args_buff[cursor] != '|') && pre == ' ') { // detect arg
			args[index++] = &args_buff[cursor];
		}
		pre = args_buff[cursor]; // record previous char

		if(args_buff[cursor] == ' ' || args_buff[cursor] == '\n' || args_buff[cursor] == '\r')
			args_buff[cursor] = '\0'; // cut string

		if(args_buff[cursor] == '|'){ // pipe support
			args_buff[cursor++] = '\0';
			pipeFlag = 1;
			args[index] = NULL;
			return index;
		} else pipeFlag = 0;
		cursor++;
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
