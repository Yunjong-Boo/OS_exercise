#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//using strsep() -> parsing the str
//using execv()

void print_error(){
	fprintf(stderr, "An error has occurred\n");
}

int main(int argc, char **argv){
	char *input_line;
	size_t len = 0;
	ssize_t nread;
	pid_t pid;
	/* if none input argumnets */
	if(argc == 1){
		do{
			printf("wish> ");
			getline(&input_line, &len, stdin);
		}
		while(strcmp(input_line, "exit") != 0);
	}
	/* if one input argument */
	if(argc == 2){
		FILE *fp;
		if((fp = fopen(argv[1], "r")) == NULL){
			printf("error: cannot open input file\n");
			exit(1);
		}
		/* getline until file EOF */
		while((nread = getline(&input_line, &len , fp)) != -1){
			pid = fork();
			/* failed to fork() */
			if(pid < 0)	print_error();
			/* in child process */
			else if(pid == 0){
				// process excute
			}
			/* in parent process */
			else{
				wait(0);
			}
		}
	}

	return 0;
}
