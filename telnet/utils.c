#include "header.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>


char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}

int startsWith(const char *str, const char *pre){
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? -1 : strncmp(pre, str, lenpre) == 0;
}

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


/*
  Rajouter fonctionnalité pour chemon absolu + cd .. + ~
*/

int cd(char* dir, char** path){
  if(startsWith(dir,"~") != 0){
    *path = getenv("HOME");
    char temp[strlen(dir)];
    int i = 0;
    for (i;i<strlen(dir)-2;i++){
      temp[i] = dir[i+2];
    }
    temp[i] = 0;
    char str[strlen(temp) + strlen(*path) + 1];
    strcpy(str, *path);
    strcat(str, "/");
    strcat(str, temp);
    *path = malloc(strlen(str)+1);
    strcpy(*path,str);
  } else if(startsWith(dir,"/") == 0){
    char str[strlen(dir) + strlen(*path) + 1];
    strcpy(str, *path);
    strcat(str, "/");
    strcat(str, dir);
    *path = malloc(strlen(str)+1);
    strcpy(*path,str);
  } else {
    *path = malloc(strlen(dir)+1);
    strcpy(*path,dir);
  }
  errno = 0;
  int i = chdir(*path);
  free(*path);
  return errno;
}

int getPwd(char** pwd){
	char temp[4096]; 
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

int getArg(char* cmd, char* str, char** arg_result){
  int len = strlen(cmd);
  int i=len;
  while(str[i]==32){
    i++;
  }
  char temp[strlen(str)];
  int j;
  for(j=0; j<strlen(str); j++){
    if(str[j+i]!=0 && str[j+i]!=32 && str[j+i]!=10){
      temp[j] = str[j+i];
    } else {
      break;
    }
  }
  temp[j]=0;
  *arg_result = malloc(strlen(temp)+1);
  strcpy(*arg_result, temp); // <-- causes core dumped
  //*arg_result = temp;
  // printf("targ: %s\n", *arg);

  return 0;
}


int sendMsg(char* msg, int s){

  // int ja;
  // for(ja=0; ja<strlen(msg); ja++){
  //   printf("tok%i: %i\n", ja, msg[ja]);
  // }
  // printf("getstr: %s\n", msg);

  // printf("msg: %s\n", msg);
  // printf("msg len = %lu\n", strlen(msg));
  write(s, msg, strlen(msg)+1);
  return 0;
}

int sendHeader(msgHeader* h, int s){
  printf("Header sent\n");
  write(s, h, sizeof(h));
  return 0;
}

int sendType(int s, int type, int length) {
  // printf("Type: %i\n", type);
  msgHeader h;
  h.length = length;
  h.type = type;
  sendHeader(&h, s);
}