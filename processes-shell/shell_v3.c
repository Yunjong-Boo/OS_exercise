#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define DELIM_CHARS	" \n"

/* execv -> built-in commands -> redirection -> parallel */

void print_error(){
	char error_message[30] = "An error has occured\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

void mal_free(int idx, char *word[10], char *tmp){
	for(int i = 0; i < idx; i++){
		free(word[i]);
	}
	free(tmp);
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

//for(int i = 0; i < word_idx+1; i++){
//	printf("word[%d]: %s\n", i, word[i]);
//	for(int j = 0; j < strlen(word[i]); j++){
//		printf("word[%d][%d]: %c\n", i, j, word[i][j]);
//	}
//}

	/* check instructor */
	if(strcmp(word[0], "cd") == 0){
		if(word_idx == 2){
			if(strcmp(word[1], "~") == 0){
				if(chdir(getenv("HOME")) == -1){
//					printf("in cd HOME\n");
//					printf("errno: %d\n", errno);
					print_error();
					mal_free(word_idx, word, tmp);
					exit(1);
				}
			}
			else{
				if(chdir(word[1]) == -1){
//					printf("in cd word[1]\n");
//					printf("errno: %d\n", errno);
					print_error();
					mal_free(word_idx, word, tmp);
					exit(1);
				}
			}
		}
		else{
//			printf("adfadfadfa\n");
			print_error();
			mal_free(word_idx, word, tmp);
			exit(1);
		}
	}
	else if(strcmp(word[0], "path") == 0){
		printf("in func if path\n");
		mal_free(word_idx, word, tmp);
		exit(0);
	}
	/* other instructors */
	else{
//	printf("other instruction\n");
		char path[100];
		sprintf(path, "/bin/%s", word[0]);
		if(access(path, X_OK) == -1){
			sprintf(path, "/usr/bin/%s", word[0]);
			if(access(path, X_OK) == -1){
				printf("after access error\n");
				print_error();
				exit(1);
			}
		}
		execv(path, word);
	}
	mal_free(word_idx, word, tmp);
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
//				printf("in child process\n");
				excute(input_line);
				exit(0);
			}
			/* in parent process */
			else if(pid > 0){
				wait(0);
//				printf("child process terminated\n");
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
//				printf("in child process\n");
				excute(input_line);
				exit(0);
			}
			else if(pid > 0){
				wait(0);
//				printf("child process termintaed\n");
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
