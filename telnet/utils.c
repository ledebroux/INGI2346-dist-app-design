#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>

int getLs(char* path, int s){
  DIR *dir;
  struct dirent *dent;
  // char buffer[50];
  // strcpy(buffer, args[1]);
  dir = opendir(path);   //this part
  if(dir!=NULL)
  {
    while((dent=readdir(dir))!=NULL) {
      if(s < 0) {
        printf ("[%s]\n", dent->d_name);
      } else {
        printf ("send[%s]\n", dent->d_name);
        // printf("size: %lu\n", sizeof(dent->d_name));
        write(s, dent->d_name, sizeof(dent->d_name));
      }
    }
    if(s >= 0){
      char* end = "end";
      write(s, end, sizeof(end));
    }
  } else {
    printf("wrong path");
  }
  // close(dir);
  return 0;
}

int cd(char* dir, char** path){
  printf("dir : %s\n", dir);
  printf("path: %s\n", *path);
  printf("dirl : %lu\n", strlen(dir));
  printf("pathl: %lu\n", strlen(*path));
  // int len = strlen(*path) + strlen(dir) + 1;
  char str[1024];
  strcpy(str, *path);
  strcat(str, "/");
  strcat(str, dir);
  printf("str : %s\n", str);
  // *path = malloc(strlen(str)*sizeof(char));
  // printf("pathsizeb4: %lu", strlen(*path));
  *path = str;
  // printf("pathsize: %lu", strlen(*path));
  printf("path : %s\n", *path);
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

// int main (int c, char **args){
//   char** str;
//   int i = getPwd(&str);
//   //printf("current directory : %s",str[0]);
//   return 0;
// }
