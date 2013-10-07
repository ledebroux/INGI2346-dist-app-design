#include <stdio.h>
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
        printf("size: %lu\n", sizeof(dent->d_name));
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

// int getPwd(char* pwd){
// 	char* temp[2014];
// 	if (getcwd(temp, sizeof(temp)) != NULL){
// 		int size = 0;
// 		for(int i=0;temp[i]!= '\0';i++){
// 			size++;
// 		}
// 		pwd = malloc(sizeof(char)*size);
// 		for(int i=0;i<=size;i++){
// 			pwd[i] = temp[i];
// 		}
// 	}
// 	return 0;      
// }

// void main (int c, char **args){
//   // printf("dir: %s", args[1]);
//   getLs(args[1], 0);
// }