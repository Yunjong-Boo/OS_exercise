#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define DELIM_CHARS	" \n"
char *paths[256] = {"/bin", NULL};

/* execv -> built-in commands -> redirection -> parallel */

void print_error(){
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}

void mal_free(int idx, char *word[10]){
	for(int i = 0; i < idx; i++){
		free(word[i]);
	}
}

void toknize(char *input, char *word[10], int *word_idx){
	/* copy input to tmp because strtok change original str */
	char *tmp = malloc((sizeof(input)/sizeof(char)));
	strcpy(tmp, input);
	/* toknize line to word */
	char *tok_ptr;
	tok_ptr = strtok(tmp, DELIM_CHARS);
	while(tok_ptr){
		word[*word_idx] = (char*)malloc((sizeof(tok_ptr)/sizeof(char)));
		strcpy(word[(*word_idx)++], tok_ptr);
		tok_ptr = strtok(NULL, DELIM_CHARS);
	}
	word[*word_idx] = NULL;
	free(tmp);
}

void execute(char *input){
	int word_idx = 0, paths_idx = 0;
	char *word[10];
	toknize(input, word, &word_idx);

	/* other instructors */
	char path[256];
	while(paths[paths_idx] != NULL){
		sprintf(path, "%s/%s", paths[paths_idx++], word[0]);
		if(access(path, X_OK) == 0)	break;
	}
	if(access(path, X_OK) == -1){
		print_error();
		exit(1);
	}
	execv(path, word);
	mal_free(word_idx, word);
}

int check_built_in(char *input){
	char *word[10];
	int word_idx = 0, check = 0;
	toknize(input, word, &word_idx);
	/* if exit cmd */
	if((strcmp(word[0], "exit")) == 0){
		if(word_idx >= 2){
			print_error();
			mal_free(word_idx, word);
			//exit(1);
		}	
		exit(0);
	}
	/* if cd cmd */
	else if((strcmp(word[0], "cd")) == 0){
		check = 1;
		if(word_idx == 2){
			if(strcmp(word[1], "~") == 0){
				if(chdir(getenv("HOME")) == -1){
					print_error();
					mal_free(word_idx, word);
					//exit(1);
				}
			}
			else{
				if(chdir(word[1]) == -1){
					print_error();
					mal_free(word_idx, word);
					//exit(1);
				}
			}
		}
		else{
			print_error();
			mal_free(word_idx, word);
			//exit(1);
		}
	}
	/* if path cmd */
	else if((strcmp(word[0], "path")) == 0){
		check = 1;
		paths[0] = NULL;
		int idx;
		for(idx = 0; idx < word_idx-1; idx++){
			paths[idx] = strdup(word[idx+1]);
		}
		paths[idx+1] = NULL;
	}

	/* if check == 1 -> cd/path cmd */
	return check;
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
			/* check built in commands */
			if((check_built_in(input_line)) == 1)	continue;

			/* make process to excute execv */
			pid_t pid = fork();
			/* in child process */
			if(pid == 0){
				execute(input_line);
				exit(0);
			}
			/* in parent process */
			else if(pid > 0){
				wait(0);
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
		/* open file */
		FILE *fp;
		if((fp =fopen(argv[1], "r")) == NULL){
			print_error();
			exit(1);
		}
		/* getline until file EOF */
		while((nread = getline(&input_line, &len, fp)) != -1){
			/* check built in commands */
			if((check_built_in(input_line)) == 1)	continue;

			/* make process to excute execv */
			pid_t pid = fork();
			if(pid == 0){
				execute(input_line);
				exit(0);
			}
			else if(pid > 0){
				wait(0);
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
