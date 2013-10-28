/*
 * Thibaut Knop & Lenoard Debroux
 * Group 24
 * INGI2146 - Mission 1
 * utils.c
 */


#include "header.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>



//////////////////////////////////////////////////////////////////////////
/////////////////////////// AUX FUNCTIONS ////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
 * Replace a substring orig from str by a substring rep, if orig is in str.
 * Return a pointer to the new str.
 */
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

/*
 * Return 0 if str starts with pre, 1 else.
 */
int startsWith(const char *str, const char *pre){
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? -1 : strncmp(pre, str, lenpre) == 0;
}


/*
 * cmd represents the command type (cd, ls, lls, ...)
 * str is the input from the stdin
 * arg_result will contain the argument of the command.
 * Ex: cd path/test   cmd = cd    str = cd path/test   arg_result = path/test
 */
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
  errno = 0;
  *arg_result = malloc(strlen(temp)+1);
  if(errno == 0){
    strcpy(*arg_result, temp);
    return 0;
  }
  return errno;
}

/*
 * Sugar for the sending of a message msg on the socket designated by its
 * socket descriptor s 
 */
 int sendMsg(char* msg, int s){
  errno = 0;
  write(s, msg, strlen(msg)+1);
  return errno;
}

/*
 * Sugar for the sending of a header h  on the socket designated by its
 * socket descriptor s .
 $ h is a of a msgHeader type, see in header.h for definition
 */
int sendHeader(msgHeader* h, int s){
  errno = 0;
  write(s, h, sizeof(h));
  return errno;
}

/*
 * Sugar for defining a header and sending it on the socket designated
 * by its socket descriptor s.
 * type and length are the fields the structure msgHeader h.
 *
 * To avoid any errors due to the endianness used by the operating system
 * we convert the int in the structure msgHeader from the host layer to the 
 * network layer thanks to the function htonl()
 * Upon receiving a header, the inverse conversion must be done, thanks
 * to ntohl().
 * It is done in myftp.c and myftpd.c
 */
int sendType(int s, int type, int length) {
  msgHeader h;
  h.length = htonl(length);
  h.type = htonl(type);
  int result = sendHeader(&h, s);
  return result;
}


/*
 * concatCustom copy begin in str, then concat it with / and end
 * str must be of a length enough to contain begin+"/"+end
 * Return 0 if
 */

int concatCustom(char *str, char *begin, char *end){
  strcpy(str, begin);
  strcat(str, "/");
  strcat(str, end);
  return 0;
}






//////////////////////////////////////////////////////////////////////////
////////////////////////// CORE FUNCTIONS ////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*
 * LS
 *
 * s = -1 if local command, for client
 * s = the number of the socket descriptor if distant command, for server
 * if distant : write each entry of the directory specified by path on the socket, 
 * and send end when there is no more entries
 * if local: just print the entry
 */
int getLs(char* path, char** result, int local){
  DIR *dir;
  errno = 0;
  struct dirent *dent;
  dir = opendir(path);
  if(dir!=NULL)
  {
    int temp = 0;
    if(local != 0){
      while((dent=readdir(dir))!=NULL) {
        printf ("%s\n", dent->d_name);
      }
    } else {
      int length = 0;
      while((dent=readdir(dir))!=NULL) {
        length = length + strlen(dent->d_name) + 1;
      }
      closedir(dir);
      dir = opendir(path);
      if(dir!=NULL){
        char str[length];
        int notfirst = 0;
        int len;
        while((dent=readdir(dir))!=NULL) {
          if(notfirst){
            strcat(str, dent->d_name);
            len = strlen(str);
            if(len == length-1){
              str[len] = 0;
            } else {
              str[len] = 10;
              str[len+1] = 0;
            }
          } else {
            strcpy(str, dent->d_name);
            len = strlen(str);
            str[len] = 10;
            str[len+1] = 0;
            notfirst = 1;
          }
        }

        int size = 0;
        int i;
        for(i=0;str[i]!= '\0';i++){
          size++;
        }
        char* temp2;
        temp2 = malloc((size+1)*sizeof(char));
        for(i=0;i<=size;i++){
         temp2[i] = str[i];
        }

        *result = temp2;
      }
      closedir(dir);
    }
  }
  return errno;
}


/*
 * Change of Directory
 * 
 * path is a pointer to a string representing the current directory
 * dir is the argument from the cd command
 * if dir starts with ~, carry out the addition of the HOME path before the dir
 * if dir starts with /, dir is an absolute path
 */

int cd(char* dir, char** path){
  if(startsWith(dir,"~") != 0){
    *path = getenv("HOME");
    if(strlen(dir) > 1){
      char temp[strlen(dir)];
      int i = 0;
      for (i;i<strlen(dir)-2;i++){
        temp[i] = dir[i+2];
      }
      temp[i] = 0;
      char str[strlen(temp) + strlen(*path) + 1];
      concatCustom(str, *path, temp);
      *path = malloc(strlen(str)+1);
      strcpy(*path,str);
    }
    else{
      *path = malloc(strlen(getenv("HOME")));
      strcpy(*path,getenv("HOME"));
    }
  } else if(startsWith(dir,"/") == 0){
    char str[strlen(dir) + strlen(*path) + 1];
    concatCustom(str, *path, dir);
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

/*
 * Put the current directory in pwd 
 */
int getPwd(char** pwd){
	char temp[4096]; // 4096 is the MAX_PATH length, so it is logical to take that value
  errno = 0;
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
	}
	return errno;     
}
