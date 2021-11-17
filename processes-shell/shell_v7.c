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

void mal_free(int idx, char *word[256]){
	for(int i = 0; i < idx; i++){
		free(word[i]);
	}
}

void redirect_toknize(char *input, char *word[256], int *word_idx){
	char *tmp = malloc((strlen(input)+1)*sizeof(char));
	strcpy(tmp, input);
	char *sep_ptr;
	/* ">" is first */
	if(input[0] == '>'){
		word[*word_idx] = (char*)malloc(strlen(">")*sizeof(char));
		strcpy(word[(*word_idx)++], ">");
		sep_ptr = strsep(&tmp, ">");
		sep_ptr = strsep(&tmp, ">");
		word[*word_idx] = (char*)malloc(((int)strlen(sep_ptr)+1)*sizeof(char));
		strcpy(word[(*word_idx)++], sep_ptr);
	}
	/* ">" is last */
	else if(input[(int)strlen(input)-1] == '>'){
		word[*word_idx] = (char*)malloc((int)strlen(input)*sizeof(char));
		strncpy(word[(*word_idx)++], input, strlen(input)-1);
		word[*word_idx] = (char*)malloc(strlen(">")*sizeof(char));
		strcpy(word[(*word_idx)++], ">");
	}
	/* ">" located between two str */
	else{
		sep_ptr = strsep(&tmp, ">");
		word[*word_idx] = (char*)malloc(((int)strlen(sep_ptr)+1)*sizeof(char));
		strcpy(word[(*word_idx)++], sep_ptr);
		word[*word_idx] = (char*)malloc(strlen(">")*sizeof(char));
		strcpy(word[(*word_idx)++], ">");	
		sep_ptr = strsep(&tmp, ">");
		word[*word_idx] = (char*)malloc(((int)strlen(sep_ptr)+1)*sizeof(char));
		strcpy(word[(*word_idx)++], sep_ptr);
	}
}

void toknize(char *input, char *word[256], int *word_idx){
	/* copy input to tmp because strtok change original str */
	char *tmp = malloc(strlen(input)*sizeof(char));
	strcpy(tmp, input);
	/* toknize line to word */
	char *tok_ptr;
	tok_ptr = strtok(tmp, DELIM_CHARS);
	while(tok_ptr){
		/* if ">" is stucked other str */
		if((strstr(tok_ptr, ">") != NULL) && (strlen(tok_ptr) != 1)){
			redirect_toknize(tok_ptr, word, word_idx);
			tok_ptr = strtok(NULL, DELIM_CHARS);
		}
		else{
			word[*word_idx] = (char*)malloc(((int)strlen(tok_ptr)+1)*sizeof(char));
			strcpy(word[(*word_idx)++], tok_ptr);
			tok_ptr = strtok(NULL, DELIM_CHARS);
		}
	}
	word[*word_idx] = NULL;
	free(tmp);
}

void execute(char *input){
	int word_idx = 0, paths_idx = 0;
	char *word[256];
	toknize(input, word, &word_idx);

//for(int i = 0; i < word_idx; i++)	printf("word[%d]: %s\n", i, word[i]);
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
	/* if redirect exist */
	if(strstr(input, ">") != NULL){
		int num_red = 0, red_idx;
		for(int i = 0; i < word_idx; i++){
			if(strstr(word[i], ">") != NULL){
				if(strlen(word[i]) != 1){
					printf("%s\n", word[i]);
				}
				num_red++;
				red_idx = i;
			}
		}
		if((num_red != 1) || (red_idx != (word_idx-2))){
			print_error();
			exit(1);
		}
		else{
//	printf("idx: %d\n", word_idx);
			FILE *file_out = fopen(word[word_idx-1], "w");
			int out;
			if((out = fileno(file_out)) == -1)	print_error();
			if(dup2(out, 1) == -1){
				print_error();
				exit(1);
			}
			fclose(file_out);
			word[word_idx-2] = NULL;
		}
	}
	execv(path, word);
	mal_free(word_idx, word);
}

int check_built_in(char *input){
	char *word[256];
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
//printf("getline: %s\n", input_line);
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
		fclose(fp);

	}
	/* if input more than three arguments -> print error */
	else{
		print_error();
		exit(1);
	}

	free(input_line);
	return 0;
}
