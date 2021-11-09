#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compress_line(FILE *fp, int n_file){
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	/* read file's line until EOF */
	while((nread = getline(&line, &len, fp)) != -1){
		/* compress each line */
		int cnt = 0, line_idx = 0;
		char temp;
		/* excute while loop until line end */
		while(line[line_idx] != '\0'){
			temp = line[line_idx++];
			cnt++;
			if(temp != line[line_idx]){
				if(n_file > 2)	cnt *=(n_file-1);	//if input multiple same files
				fwrite(&cnt, sizeof(int), 1, stdout);
				fwrite(&temp, sizeof(char), 1, stdout);
				cnt = 0;
			}
		}
	}
	free(line);
}

int main(int argc, char **argv){
	/* if no input files, print error */
	if(argc == 1){
		printf("wzip: file1 [file2 ...]\n");
		exit(1);
	}

	FILE *fp;
	/* file open & if fail to open file -> print error message */
	if((fp = fopen(argv[1], "r")) == NULL){
		printf("failed to open file\n");
		exit(1);
	}

	/* compress each line */
	compress_line(fp, argc);

	free(fp);
	return 0;
}
