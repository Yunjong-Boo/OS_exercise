#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define DELIM_CHARS	" \n"

void print_error(){
	fprintf(stderr, "An error has occurred\n");
}

void excute(char *input){
	/* copy input to tmp */
	char *tmp = malloc((sizeof(input)/sizeof(char)));
	strcpy(tmp, input);
	/* parse line to word */
	int word_idx = 0;
	char *tok_ptr;
	char *word[10];
	tok_ptr = strtok(tmp, DELIM_CHARS);
	while(tok_ptr){
		word[word_idx] = (char*)malloc((sizeof(tok_ptr)/sizeof(char)));
		strcpy(word[word_idx++], tok_ptr);
		tok_ptr = strtok(NULL, DELIM_CHARS);
	}
	word[word_idx] = NULL;

	/* check instructor */
	if(strcmp(word[0], "cd") == 0){
		if(word_idx == 2){
			if(strcmp(word[1], "~") == 0){
		printf("in cd strcmp ~\n");
				if(chdir(getenv("HOME")) == -1){
					print_error();
					exit(1);
				}
			}
			else{
		printf("in cd not ~\n");
		printf("word[1]: %s\n", word[1]);
				if(chdir(word[1]) == -1){
					print_error();
					exit(1);
				}
			}
		}
		else{
			print_error();
			exit(1);
		}
	}
	else if(strcmp(word[0], "path") == 0){
		printf("in func if path\n");
		exit(0);
	}
	/* other instructors */
	char path[100];
	sprintf(path, "/bin/%s", word[0]);
	if(access(path, X_OK) == -1){
		sprintf(path, "/usr/bin/%s", word[0]);
		if(access(path, X_OK) == -1){
			print_error();
			exit(1);
		}
	}
	execv(path, word);

	for(int i = 0; i < word_idx+1; i++){
		free(word[i]);
	}
	free(tmp);
}

int main(int argc, char **argv){
	char *input_line = NULL;
	size_t len = 0;
	ssize_t nread;
	/* if input only one arguments */
	if(argc == 1){
		while(1){
			printf("wish> ");
			getline(&input_line, &len, stdin);
			if(strstr(input_line, "exit") != NULL){
				if(strlen(input_line) > 6){
					print_error();
					exit(1);
				}
				exit(0);
			}
			/* make process to excute cmd line */
			pid_t pid = fork();
			/* in child process */
			if(pid == 0){
				printf("in child process\n");
				excute(input_line);
				exit(0);
			}
			/* in parent process */
			else if(pid > 0){
				wait(0);
				printf("child process terminated\n");
			}
			/* failed to create process */
			else{
				print_error();
				exit(1);
			}
		}
	}
	/* if input two arguments */
	else if(argc == 2){
		FILE *fp;
		if((fopen(argv[1], "r")) == NULL){
			print_error();
			exit(1);
		}
		while((nread = getline(&input_line, &len, fp)) != -1){
			if(strstr(input_line, "exit") != NULL){
				if(strlen(input_line) > 6){
					print_error();
					exit(1);
				}
				exit(0);
			}
			pid_t pid = fork();
			if(pid == 0){
				printf("in child process\n");
				excute(input_line);
				exit(0);
			}
			else if(pid > 0){
				wait(0);
				printf("child process termintaed\n");
			}
			else{
				print_error();
				exit(1);
			}
		}

	}
	/* if input more than three arguments -> print error */
	else{
		print_error();
		exit(1);
	}

	free(input_line);
	return 0;
}
