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
	Node* new = malloc(sizeof(Node));
	if(new == NULL){
		printf("malloc failed\n");
		exit(1);
	}
	new->data = (char*)malloc((sizeof(line)/sizeof(char)));
	if(new->data == NULL){
		printf("malloc failed\n");
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

	/* if input two command-line arguments(input.txt & output.txt) */
	if(argc == 3){
		/* if input, output file is same -> print error message */
		if(strcmp(argv[1], argv[2]) == 0){
			printf("Input and output file must differ\n");
			exit(1);
		}

		FILE *ifp;
		/* if failed to open input file -> print error message */
		if((ifp = fopen(argv[1], "r")) == NULL){
			printf("error: cannot open file 'input.txt'\n");
			exit(1);
		}

		char *input_line = NULL;
		size_t len = 0;
		ssize_t nread;
		/* getline until file EOF */
		while((nread = getline(&input_line, &len, ifp)) != -1){
			Push(Stack, input_line);
		}

		FILE *ofp;
		if((ofp = fopen(argv[2], "w")) == NULL){
			printf("error: cannot open file 'output.txt'\n");
			exit(1);
		}
		/* print line in output file to reverse order */
		int size = Stack->size;
		for(int i = 0; i < size; i++){
			Pop(Stack, ofp);
		}
		free(input_line);
		fclose(ifp);
		fclose(ofp);
	}
	/* if input one command-line argument(input.txt) */
	else if(argc == 2){
		
	}
	/* if no input argument */
	else if(argc == 1){

	}
	/* too many arguments passed to program */
	else{
		printf("usage: reverse <input> <output>\n");
		exit(1);
	}

	free(Stack);
	return 0;
}
