#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Node{
	struct _Node* next;
	char* data;
} Node;

typedef struct _ListStack{
	Node* peak;
	int size;
} ListStack;

void Push(ListStack* stack, char *line){
	Node *new = malloc(sizeof(Node));
	if(new == NULL){
		fprintf(stderr, "malloc failed\n");
		exit(1);
	}
	new->data = (char*)malloc((sizeof(line)/sizeof(char)));
	if(new->data == NULL){
		fprintf(stderr, "malloc failed\n");
		exit(1);
	}
	strcpy(new->data, line);
	new->next = stack->peak;
	stack->peak = new;
	stack->size++;
}

void Pop(ListStack* stack, FILE *ofp){
	if(stack->size == 0)	exit(1);
	else{
		fprintf(ofp, "%s", stack->peak->data);
		Node *temp = stack->peak;
		stack->peak = temp->next;
		stack->size--;
		free(temp);
	}
}

int main(int argc, char **argv){
	ListStack *Stack = calloc(1, sizeof(ListStack));
	char *input_line = NULL;
	size_t len = 0;
	ssize_t nread;

	/* if input two command-line arguments(input.txt & output.txt) */
	if(argc == 3){
		/* cut the path by using strtok */
		char temp1[20], temp2[20];
		strcpy(temp1, argv[1]);
		strcpy(temp2, argv[2]);
		char *ptr1 = strtok(temp1, "/");
		ptr1 = strtok(NULL, "/");
		char *ptr2 = strtok(temp2, "/");
		ptr2 = strtok(NULL, "/");
		/* if input, output file is same -> print error message */
		if((ptr1 != NULL) && (ptr2 != NULL) && (strcmp(ptr1, ptr2) == 0)){
			fprintf(stderr, "reverse: input and output file must differ\n");
			exit(1);
		}

		FILE *ifp;
		/* if failed to open input file -> print error message */
		if((ifp = fopen(argv[1], "r")) == NULL){
			fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
			exit(1);
		}

		/* getline until file EOF */
		while((nread = getline(&input_line, &len, ifp)) != -1){
			Push(Stack, input_line);
		}

		FILE *ofp;
		if((ofp = fopen(argv[2], "w")) == NULL){
			fprintf(stderr, "error: cannot open file 'output.txt'\n");
			exit(1);
		}
		/* print line in output file to reverse order */
		int size = Stack->size;
		for(int i = 0; i < size; i++){
			Pop(Stack, ofp);
		}
		fclose(ifp);
		fclose(ofp);
	}
	/* if input one command-line argument(input.txt) */
	else if(argc == 2){
		FILE *ifp;
		if((ifp = fopen(argv[1], "r")) == NULL){
			fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
			exit(1);
		}

		/* getline until file EOF */
		while((nread = getline(&input_line, &len, ifp)) != -1){
			Push(Stack, input_line);
		}

		/* print line in stdout to reverse order */
		int size = Stack->size;
		for(int i = 0; i < size; i++){
			Pop(Stack, stdout);
		}
		fclose(ifp);
	}
	/* if no input argument */
	else if(argc == 1){
		/* getline until stdin EOF */
		while((nread = getline(&input_line, &len, stdin)) != -1){
			Push(Stack, input_line);
		}

		/* print line in stdout to reverse order */
		int size = Stack->size;
		for(int i = 0; i < size; i++){
			Pop(Stack, stdout);
		}
	}
	/* too many arguments passed to program */
	else{
		fprintf(stderr, "usage: reverse <input> <output>\n");
		exit(1);
	}

	free(input_line);
	free(Stack);
	return 0;
}
