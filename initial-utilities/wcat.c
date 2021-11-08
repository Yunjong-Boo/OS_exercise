#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	if(argc == 1)	exit(1);

	for(int i = 1; i < argc; i++){
		FILE *fp;
		fp = fopen(argv[i], "r");
		if(fp == NULL){
			printf("wcat: cannot open file\n");
			exit(1);
		}

		/* read argv[i] file & print line to stdout */
		char *line = NULL;
		size_t len = 0;
		ssize_t nread;
		while((nread = getline(&line, &len, fp)) != -1){
			printf("%s", line);
		}
		free(line);
		fclose(fp);
	}

	return 0;
}
