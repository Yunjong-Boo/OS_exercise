#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Unit{
	int cnt[100], idx;
	char ch[100];
} Unit;

Unit compress_line(FILE *fp){
	/* creat Unit & initialization */
	Unit compress;
	compress.idx = 0;
	for(int i = 0; i < 100; i++){
		compress.cnt[i] = 0;
		compress.ch[0] = '\0';
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	/* read getline until file EOF */
	while((nread = getline(&line, &len, fp)) != -1){
		int line_idx = 0, cnt = 0;
		char temp;
		while(line[line_idx] != '\0'){
			temp = line[line_idx++];
			cnt++;
			if(temp != line[line_idx]){
				int idx = compress.idx++;
				compress.cnt[idx] = cnt;
				compress.ch[idx] = temp;
				cnt = 0;
			}
		}
	}
	free(line);
	return compress;
}

int main(int argc, char **argv){
	/* if no input file -> print error message */
	if(argc == 1){
		printf("wzip: file1 [file2 ...]\n");
		exit(1);
	}

	FILE *fp;
	/* if input only one file */
	if(argc == 2){
		/* file open & if open failed -> print error message */
		if((fp = fopen(argv[1], "r")) == NULL){
			printf("Failed to open file\n");
			exit(1);
		}

		char *line = NULL;
		size_t len = 0;
		ssize_t nread;
		/* read getline until file EOF */
		while((nread = getline(&line, &len, fp) != -1)){
			int line_idx = 0, cnt = 0;
			char temp;
			while(line[line_idx] != '\0'){
				temp = line[line_idx++];
				cnt++;
				if(temp != line[line_idx]){
					fwrite(&cnt, sizeof(int), 1, stdout);
					fwrite(&temp, sizeof(char), 1, stdout);
					cnt = 0;
				}
			}
		}
		free(line);
//		Unit compressed = compress_line(fp);
//		for(int i = 0; i < compressed.idx; i++){
//			fwrite(&(compressed.cnt[i]), sizeof(int), 1, stdout);
//			fwrite(&(compressed.ch[i]), sizeof(char), 1, stdout);
//		}
		fclose(fp);
	}
	/* if input more than two files */
	else{
		/* store each file's compression */
		Unit *compressed = calloc(argc-1, sizeof(Unit));
		for(int i = 0; i < argc-1; i++){
			/* file open & if open failed -> print error message */
			if((fp = fopen(argv[i+1], "r")) == NULL){
				printf("Failed to open file\n");
				exit(1);
			}
			compressed[i] = compress_line(fp);
			fclose(fp);
		}
		/* creat Unit merge & merging compressed */
		Unit merge;
		merge.idx = 0;
		for(int i = 0; i < 100; i++){
			merge.cnt[i] = 0;
			merge.ch[i] = '\0';
		}
		for(int i= 0 ; i < argc-1; i++){
			merge.cnt[merge.idx] += compressed[i].cnt[0];
		}
		merge.ch[merge.idx++] = compressed[0].ch[0];
		for(int i = 0; i < merge.idx; i++){
			fwrite(&(merge.cnt[i]), sizeof(int), 1, stdout);
			fwrite(&(merge.ch[i]), sizeof(char), 1, stdout);
		}
		free(compressed);
	}

	return 0;
}
