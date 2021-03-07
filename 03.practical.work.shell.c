#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX 1000

char* getArg(void);
void delArg(char* arg);


int main() {
	printf("\n****LH Shell****\n");
	while(1){
		printf("> ");
		char *command = getArg();
		int pid = fork();
		if(pid == 0) { // this is child who execute shell
			char *args[]= {command, NULL};
			execvp(command, args);
		} 
		else { // this is parent who wait for next comment
			wait(&pid);
			delArg(command);
		}
	}
	return 0;
}

char* getArg(void){
	char buff[MAX];
	char *arg = (char*) malloc(sizeof(char) * MAX);
	scanf("%s", arg);
	return arg;
}

void delArg(char* arg){
	free(arg);
}
