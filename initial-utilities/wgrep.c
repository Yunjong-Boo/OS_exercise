#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	/* no search term, no file */
	if(argc == 1){
		printf("wgrep: searchterm [file ...]\n");
		exit(1);
	}
	
	int idx = 2;
	while(1){
		/* argv[1] == NULL -> no lines match */
		if(strlen(argv[1]) == 0){
			exit(0);
		}
		char *line = NULL;
		size_t len = 0;
		ssize_t nread;
		/* search term, no file */
		if(argc == 2){
			while((nread = getline(&line, &len, stdin)) != -1){
				if(strstr(line, argv[1]) != NULL){
					printf("%s", line);
				}
			}
			free(line);
			break;
		}
		/* other cases */
		else{
			FILE *fp = fopen(argv[idx++], "r");
			if(fp == NULL){
				printf("wgrep: cannot open file\n");
				exit(1);
			}
			while((nread = getline(&line, &len, fp)) != -1){
				if(strstr(line, argv[1]) != NULL){
					printf("%s", line);
				}
			}
			fclose(fp);
			free(line);
			if(idx == argc)	break;
		}
	}

	return 0;
}
