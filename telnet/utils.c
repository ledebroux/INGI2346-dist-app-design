#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int getLs(char* str, char* path, int s){
  DIR *dir;
  struct dirent *dent;
  // char buffer[50];
  // strcpy(buffer, args[1]);
  dir = opendir(path);   //this part
  if(dir!=NULL)
  {
    int size
    while((dent=readdir(dir))!=NULL) {
      printf ("[%s]\n", dent->d_name);
    }
      // printf(dent->d_name);
    printf("not null");
  } else {
    printf("null");
  }
  close(dir);
  return 0;
}

void main (int c, char **args){
  char* str;
  printf("dir: %s", args[1]);
  getLs(str, args[1]);
}