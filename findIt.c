#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

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
#define SMALLEST_TYPE 27
 //other
#define NONE 42
#define LIMIT 100

struct Node{
	/* data */
	int type;
	bool result;
	struct Node *left;
	struct Node *right;
	//for expressions that require an extra param
	char *extraParam;
};
//Sorry for all the globals I didnt have time to think of a better way do this!
struct Node *root;
int foundCount = 0;
char *dir;
char **found;
bool printStuff = false;
bool summarize = false;
bool treedir = false;
int summer[7];
int smallestLargest[2];
char smallest[512];
char largest[512];

//traverses the logic tree and checks the condition for each file in the file dirs
bool traverseTree(struct Node *start, char *fileName, char *fileLocation){
	//base case
	if(start == NULL){
		printf("Something bad happened\n");
		exit(-1);
	}

	if(start->type == AND){
		//error check
		if(start->left == NULL || start->right == NULL){
			printf("Bad input\n");
			exit(-1);
		}
		return traverseTree(start->left, fileName, fileLocation) && traverseTree(start->right , fileName, fileLocation);

	}
	else if(start->type == OR){
		if(start->left == NULL || start->right == NULL){
			printf("Bad input\n");
			exit(-1);
		}
		return traverseTree(start->left, fileName, fileLocation) || traverseTree(start->right , fileName, fileLocation);
	}
	else if(start->type == NAME){
		if(strcmp(start->extraParam, fileName) == 0)
			return true;
		return false;
	}
	else if(start->type == LARGEST_TYPE){
		struct stat file_info;
		lstat(fileLocation, &file_info);

		if(start->extraParam[0] == 'f' && S_ISREG(file_info.st_mode) || start->extraParam[0] == 'd' && S_ISDIR(file_info.st_mode)){
			if(smallestLargest[1] == -1){
				smallestLargest[1] = file_info.st_size;
				strcpy(largest, fileLocation);
			}
			else if(file_info.st_size > smallestLargest[1]){
				smallestLargest[1] = file_info.st_size;
				strcpy(largest, fileLocation);
			}
		}
		else
			return false;
	}
	else if(start->type == SMALLEST_TYPE){
		struct stat file_info;
		lstat(fileLocation, &file_info);

		if(start->extraParam[0] == 'f' && S_ISREG(file_info.st_mode) || start->extraParam[0] == 'd' && S_ISDIR(file_info.st_mode)){
			if(smallestLargest[0] == -1){
				smallestLargest[0] = file_info.st_size;
				strcpy(smallest, fileLocation);
			}
			else if(file_info.st_size < smallestLargest[0]){
				smallestLargest[0] = file_info.st_size;
				strcpy(smallest, fileLocation);
			}
		}
		else
			return false;
	}
	else if(start->type == SIZE){
		int 
	}
	else if(start->type == SUMMARIZE)
		return true;
	else if(start->type == PRINT)
		return true;
	else if(start->type == TREEDIR)
		return true;

}

//if the evaluations are true add the file to the list of files that work
void evaluate(char *fileName, char *fileLocation){
	if(traverseTree(root, fileName, fileLocation)){
		found[foundCount] = (char *) malloc(sizeof(char *) * strlen(fileLocation));
		strcpy(found[foundCount], fileLocation);
		foundCount++;
	}
}

//searches the dir by searching by depth first
void depthSearch(char *path){
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
		evaluate(curEnt-> d_name, newPath);

		if(summarize && (strcmp(curEnt->d_name, ".") != 0) && (strcmp(curEnt->d_name, "..") != 0)){
			struct stat file_info;
			lstat(newPath, &file_info);

			if(S_ISREG(file_info.st_mode))
				summer[0] ++;
			else if(S_ISDIR(file_info.st_mode))
				summer[1] ++;
			else if(S_ISCHR(file_info.st_mode))
				summer[2] ++;
			else if(S_ISBLK(file_info.st_mode))
				summer[3] ++;
			else if(S_ISFIFO(file_info.st_mode))
				summer[4] ++;
			else if(S_ISLNK(file_info.st_mode))
				summer[5] ++;
			else if(S_ISSOCK(file_info.st_mode))
				summer[6] ++;
		}
		
		if( (curEnt->d_type == DT_DIR) && (strcmp(curEnt->d_name, ".") != 0) && (strcmp(curEnt->d_name, "..") != 0) ){
			depthSearch(newPath);
		}

		curEnt = readdir(dir);
	}//end of while loop
	closedir(dir);
}

void printResults(){
	int x;
	for(x = 0; x<foundCount; x++)
		printf("%s\n", found[x]);
}	

struct Node* makeTree(char *args[], int start, int end){
	int x;
	struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
	//check for ORs
	for(x = start; x<=end; x++){
		if(strcmp(args[x], "-or") == 0){
			tmp->type = OR;
			tmp->left = makeTree(args, start, x-1);
			tmp->right = makeTree(args, x+1, end);
			return tmp;
		}
	}
	//if expression requires a parameter
	if( strcmp(args[start], "-accesstype") == 0 || strcmp(args[start], "-largesttype") == 0 || 
		strcmp(args[start], "-smallesttype") == 0 || strcmp(args[start], "-amin") == 0 ||
		strcmp(args[start], "-atime") == 0 || strcmp(args[start], "-cmin") == 0 ||
		strcmp(args[start], "-ctime") == 0 || strcmp(args[start], "-mmin") == 0 ||
		strcmp(args[start], "-mtime") == 0 || strcmp(args[start], "-size") == 0 ||
		strcmp(args[start], "-type") == 0 || strcmp(args[start], "-uid") == 0 ||
		strcmp(args[start], "-gid") == 0 || strcmp(args[start], "-name") == 0){
		//check to see if only one expression
		if(start + 1 == end){
			//create a node and return it
			if(strcmp(args[start], "-name") == 0){
				struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
				tmp->type = NAME;
				tmp->extraParam = args[start+1];
				tmp->left = NULL;
				tmp->right = NULL;
				return tmp;
			}
			else if(strcmp(args[start], "-accesstype") == 0){
				struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
				tmp->type = ACCESS_TYPE;
				tmp->extraParam = args[start+1];
				tmp->left = NULL;
				tmp->right = NULL;
				return tmp;
			}
			else if(strcmp(args[start], "-largesttype") == 0){
				struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
				tmp->type = LARGEST_TYPE;
				tmp->extraParam = args[start+1];
				tmp->left = NULL;
				tmp->right = NULL;
				return tmp;
			}
			else if(strcmp(args[start], "-smallesttype") == 0){
				struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
				tmp->type = SMALLEST_TYPE;
				tmp->extraParam = args[start+1];
				tmp->left = NULL;
				tmp->right = NULL;
				return tmp;
			}
			else if(strcmp(args[start], "-size") == 0){
				struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
				tmp->type = SIZE;
				tmp->extraParam = args[start+1];
				tmp->left = NULL;
				tmp->right = NULL;
				return tmp;
			}
			else{
				printf("Sorry I don't support that command\n");
				exit(-1);
			}
		}
		//more than one expression
		else {
			//create AND node and return it
			struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
			tmp-> type = AND;
			tmp->left = makeTree(args, start, start+1);
			tmp->right = makeTree(args, start+2, end);
			return tmp;
		}
	}
	//expression does not require a parameter
	else{
		if(start == end){
			//create a node and return it
			if(strcmp(args[start], "-print") == 0){
				struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
				tmp->type = PRINT;
				tmp->result = true;
				tmp->left = NULL;
				tmp->right = NULL;

				printStuff = true;
				return tmp;
			}
			else if(strcmp(args[start], "-treedir") == 0){
				struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
				tmp->type = TREEDIR;
				tmp->result = true;
				tmp->left = NULL;
				tmp->right = NULL;

				treedir = true;	
				return tmp;
			}
			else if(strcmp(args[start], "-sparse") == 0){
				struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
				tmp->type = SPARSE;
				tmp->result = true;
				tmp->left = NULL;
				tmp->right = NULL;
				return tmp;
			}
			else if(strcmp(args[start], "-summarize") == 0){
				struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
				tmp->type = SUMMARIZE;
				tmp->result = true;
				tmp->left = NULL;
				tmp->right = NULL;

				summarize = true;
				return tmp;
			}
			else{
				printf("Sorry I don't support that command\n");
				exit(-1);
			}

		}
		else{
			//create AND node and return it
			struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
			tmp-> type = AND;
			tmp->left = makeTree(args, start, start);
			tmp->right = makeTree(args, start+1, end);
			return tmp;
		}
	}
	printf("ERROR BAD THINGS HAPPENED, MAYBE INPUT WAS BAD\n");
	exit(-1);
}

int main(int argc, char *argv[]){
	printf("Carlos Ortega	corteg20\n");

	//initialize
	found = (char **) malloc(sizeof(char **)* LIMIT);
	int x;
	for(x =0; x<7; x++)
		summer[x] = 0;
	for(x=0; x<2; x++)
		smallestLargest[x] = -1;

	//find what the directory is
	if(argc == 1 || argv[1][0] == '-'){
		char cur[] = ".\0";
		dir = &cur;
	}
	else
		dir = argv[1];

	//find where the first expression begins
	for(x=0; x<argc; x++){
		if(argv[x][0] == '-')
			break;
	}

	root = makeTree(argv, x, argc-1);
	printResults(root);
	depthSearch(dir);

	if(printStuff)
		printResults();
	if(summarize){
		printf("REG  FILE: %d\n", summer[0]);
		printf("DIRECTORY: %d\n", summer[1]);
		printf("CHAR  DEV: %d\n", summer[2]);
		printf("BLOCK DEV: %d\n", summer[3]);
		printf("FIFO PIPE: %d\n", summer[4]);
		printf("SYMB LINK: %d\n", summer[5]);
		printf("   SOCKET: %d\n", summer[6]);
	}

	if(smallestLargest[0] != -1)
		printf("Smallest\n%s\n", smallest);
	if(smallestLargest[1] != -1)
		printf("largest\n%s\n", largest);
	
	return 0;
}//end of main