#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX 1000

char args_buff[MAX];
char *args[MAX];

int getArg(void);

int main() {
	printf("\n****LH Shell****\n");
	while(1){
		printf("> ");
		int argc = getArg();
		int pid = fork();
		if(pid == 0) { // this is child who execute shell
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

