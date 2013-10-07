#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>

int getLs(char* str, char* path){
  DIR *dir;
  struct dirent *dent;
  // char buffer[50];
  // strcpy(buffer, args[1]);
  dir = opendir(path);   //this part
  if(dir!=NULL)
  {
      while((dent=readdir(dir))!=NULL)
          printf(dent->d_name);
  }
  close(dir);
}

int getPwd(char* pwd){
	char* temp[2014];
	if (getcwd(temp, sizeof(temp)) != NULL){
		int size = 0;
		for(int i=0;temp[i]!= '\0';i++){
			size++;
		}
		pwd = malloc(sizeof(char)*size);
		for(int i=0;i<=size;i++){
			pwd[i] = temp[i];
		}
	}
	return 0;      
}

void main (){
  char* str;
  get(str, "/home/inekar";)
}