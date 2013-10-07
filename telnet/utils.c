#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>

int getLs(char* str, char* path, int s){
  DIR *dir;
  struct dirent *dent;
  // char buffer[50];
  // strcpy(buffer, args[1]);
  dir = opendir(path);   //this part
  if(dir!=NULL)
  {
    while((dent=readdir(dir))!=NULL) {
      printf ("[%s]\n", dent->d_name);
    }
      // printf(dent->d_name);
    printf("not null");
  } else {
    printf("null");
  }
  // close(dir);
  return 0;
}

int getPwd(char** pwd){
	char temp[1024];
	if (getcwd(temp, sizeof(temp)) != NULL){
		int size = 0;
		int i;
		for(i=0;temp[i]!= '\0';i++){
			size++;
		}
		pwd[0] = malloc(sizeof(char)*size);
		for(i=0;i<=size;i++){
			pwd[0][i] = temp[i];
		}
		return 0; 
	}
	return -1;     
}

int main (int c, char **args){
  char** str;
  int i = getPwd(&str);
  //printf("current directory : %s",str[0]);
  return 0;
}