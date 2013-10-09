#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
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
  closedir(dir);
  return 0;
}


int getPwd(char** pwd){
	char temp[1024]; // AJUSTER LA TAILLE CORRECTEMENT
	if (getcwd(temp, sizeof(temp)) != NULL){
		 int size = 0;
		 int i;
		 for(i=0;temp[i]!= '\0';i++){
		 	size++;
		 }
     char* temp2;
     temp2 = malloc(size*sizeof(char));
		for(i=0;i<=size;i++){
		 temp2[i] = temp[i];
		}
    *pwd = temp2;
		return 0; 
	}
	return -1;     
}
