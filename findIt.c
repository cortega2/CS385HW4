#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

//evaluations
#define NOT 0
#define AND 1
#define OR 2
//expressions
#define PRINT 3
#define	NAME 4
#define SIZE 5
#define SUMMARIZE 6
#define ACCESS_TYPE 7
#define LARGEST_TYPE 8
#define TREEDIR 9
#define SPARSE 10
#define HELP 11
#define DEPTH 12
#define MAXDEPTH 13
#define AMIN 14
#define ATIME 15
#define CMIN 16
#define CTIME 17
#define MMIN 18
#define MTIME 19
#define LINKS 20
#define UID 21
#define GID 22
#define TYPE 23
#define FPRINT 24
#define EXEC 25
#define OK 26
 //other
#define NONE 42

struct Node{
	/* data */
	int type;
	bool result;
	struct Node *left;
	struct Node *right;
	//for -name
	char *name;
	//for -size
	int size; 
	//for testing purposes need to change this later to something more modular
	char found[100][250];
	int foundCount;
};
struct Node *root;
char *dir;



void depthSearch(char *path, struct Node *ptr){
	struct dirent *curEnt;
	DIR *dir;

	dir = opendir(path);
	if(dir == NULL){
		printf("Cannot open the specified directory\n");
		return;
	}

	curEnt = readdir(dir);
	while(curEnt != NULL){
		char newPath[strlen(path) + strlen(curEnt-> d_name)];
		strcpy(newPath, path);	
		if(path[strlen(path) - 1] != '/')
			strcat(newPath, "/");
		strcat(newPath, curEnt-> d_name);

		//printf("%s\n", newPath);

		if(ptr->type == NAME){
			if(strcmp(ptr->name, curEnt-> d_name) == 0){
				strcpy(ptr-> found[ptr-> foundCount], newPath);
				ptr-> foundCount ++;
				if(ptr-> foundCount >=100){							//realloc memory here
					//for now exit
					printf("ran out of space\n");
					return;
				}
			}
		}
		
		if( (curEnt->d_type == DT_DIR) && (strcmp(curEnt->d_name, ".") != 0) && (strcmp(curEnt->d_name, "..") != 0) ){
			depthSearch(newPath, ptr);
		}

		curEnt = readdir(dir);
	}//end of while loop

	closedir(dir);
}

void traverseTree(struct Node *start){
	//base case
	if(start == NULL)
		return;

	start-> foundCount = 0;
	if(start->type == AND){
		struct Node *left = start -> left;
		struct Node *right = start -> right;
		traverseTree(left);
		traverseTree(right);

		//error check
		if(left == NULL || right == NULL){
			printf("Bad input\n");
			exit(-1);
		}

		int x;
		for(x=0; x < left-> foundCount; x++){
			/*
			if(right -> result == true){
				strcpy(start -> found[start -> foundCount], left -> found[x]);
					start -> foundCount ++;

					if(start -> foundCount >= 100){
						printf("ran out of space\n");					//need to realloc memory here
						return;
					}
			}
			*/
			
			int y;
			for(y = 0; y < right-> foundCount; y++){
				if(strcmp(left -> found[x], right -> found[y]) == 0){
					strcpy(start -> found[start -> foundCount], left -> found[x]);
					start -> foundCount ++;

					if(start -> foundCount >= 100){
						printf("ran out of space\n");					//need to realloc memory here
						return;
					}
				}
			}
		}
	}
	else{
		depthSearch(dir, start);
		traverseTree(start -> left);
		traverseTree(start -> right);
	}

}

void printResults(){
	int x;
	for(x = 0; x<root->foundCount; x++){
		printf("%s\n", root-> found[x]);
	}
}	

int main(int argc, char *argv[]){
	printf("Carlos Ortega	corteg20\n");

	//populate the tree
	//simple case of having no atributes and only having one or no locations
	//NOTE THAT THIS HAS TO BE MODIFIED TO INCLUDE DIFFERENT LOCATIONS TO BE SEARCHED AT A TIME!
	if(argc == 1 || (argv[1][0] != '-' && argc <= 2)){
		struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
		tmp->type = NONE;
		tmp->left = NULL;
		tmp->right = NULL;
		root = tmp;
	}
	
	//find first expression
	int x;
	for( x=0;  x<argc; x++){
		if(strcmp(argv[x], "-name") == 0){
			struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
			tmp->type = NAME;
			tmp->name = argv[x+1];
			tmp->left = NULL;
			tmp->right = NULL;
			
			root = tmp;
			break;
		}
		else if(strcmp(argv[x], "-print") == 0){
			struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
			tmp->type = PRINT;
			tmp->left = NULL;
			tmp->right = NULL;
			
			root = tmp;
			break;
		}
		else if(strcmp(argv[x], "-size") == 0){
			struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
			tmp->type = SIZE;
			tmp->size = atoi(argv[x+1]);
			tmp->left = NULL;
			tmp->right = NULL;
			
			root = tmp;
			break;
		}
	}

	//find other expressions
	for( x=x+1;  x<argc; x++){
		if(strcmp(argv[x], "-name") == 0){
			struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
			tmp->type = NAME;
			tmp->name = argv[x+1];
			tmp->left = NULL;
			tmp->right = NULL;

			struct Node *and = (struct Node *)malloc(sizeof(struct Node));
			and->type = AND;
			and->left = root;
			and->right = tmp;

			root = and;
		}
		else if(strcmp(argv[x], "-print") == 0){
			struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
			tmp->type = PRINT;
			tmp->result = true;
			tmp->left = NULL;
			tmp->right = NULL;

			struct Node *and = (struct Node *)malloc(sizeof(struct Node));
			and->type = AND;
			and->left = root;
			and->right = tmp;
			root = and;
		}
		else if(strcmp(argv[x], "-size") == 0){
			struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
			tmp->type = SIZE;
			tmp->size = atoi(argv[x+1]);
			tmp->left = NULL;
			tmp->right = NULL;

			struct Node *and = (struct Node *)malloc(sizeof(struct Node));
			and->type = AND;
			and->left = root;
			and->right = tmp;

			root = and;
		}
	}
	
	if(argc == 1 || argv[1][0] == '-'){
		char cur[] = ".";
		dir = &cur;
	}
	else
		dir = argv[1];

	printf("%s\n", dir);
	traverseTree(root);
	printResults();
	

	return 0;
}//end of main