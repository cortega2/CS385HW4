#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

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
		printf("%s\n", curEnt->d_name);
		
		if( (curEnt->d_type == DT_DIR) && (strcmp(curEnt->d_name, ".") != 0) && (strcmp(curEnt->d_name, "..") != 0) ){
			char newPath[strlen(path) + strlen(curEnt-> d_name)];
			strcpy(newPath, path);
			
			if(path[strlen(path) - 1] != '/')
				strcat(newPath, "/");

			strcat(newPath, curEnt-> d_name);

			//printf("%s\n", newPath);

			depthSearch(newPath);
		}

		curEnt = readdir(dir);
	}

	closedir(dir);
}

int main(int argc, char *argv[]){
	printf("Carlos Ortega	corteg20\n");
	//printf("%s\n", argv[1]);

	depthSearch(argv[1]);

	return 0;
}//end of main