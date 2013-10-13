#include "header.h"
#include "utils.h"
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define TELNETD_PORT  8010
//#define M2_ADDR "130.104.172.88"
#define M2_ADDR "127.0.0.1"

//int shrink(char*, char*);
int getStringLength(char*, char);
int fillString(char*, char*, int);
int sendMsg(char*, int);
int getString(char*, char**, char);
int cmdcmp(char*, char*);
int getArg(char*, char*, char**);

main(argc, argv) int    argc; char   *argv[ ];
{
  int sd1;
  struct sockaddr_in     m2;

  /* fill in the structure "m2" with the address of the
   * server that we want to connect with */

  bzero((char *) &m2       , sizeof(m2));
  m2.sin_family     = AF_INET;
  m2.sin_addr.s_addr = inet_addr(M2_ADDR);
  m2.sin_port       = htons(TELNETD_PORT);

  /* Open a TCP socket (an Internet stream socket)*/

  if ( (sd1 = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket error in telnet");
    exit(-1);
  }

  /* Connect to the server */

  if (connect(sd1 , (struct sockaddr *) &m2 , sizeof ( m2 )) < 0){
    perror("connect error in telnet");
    exit(-1);
  }

  char buffer[256];

  /*
  TODO: 
  - if no arg is given when one is needed, do not send the header and throw and error.
  - if an arg is given when none is needed, do not send the header.
  */

  /*
    TODO : fgets(buffer, 255, stdin) -> il faudrait plus que 255
  */

  while(strcmp(buffer, "bye")) {
    printf("Enter a command\n");
    fgets(buffer,255,stdin);

    if(cmdcmp("lpwd", buffer)){
      printf("Local command: pwd\n");
      char *curr_dir;
      int i = getPwd(&curr_dir);
      if(!i){
        printf("%s\n", curr_dir);
      }
    }
    else if(cmdcmp("lcd", buffer)){
      printf("Local command: cd\n"); 
      char * current;
      int i = getPwd(&current);
      int j = cd(strtok(buffer,"lcd "), &current);
      if (j!=0){
        fprintf(stderr, "Error : %s\n",strerror(j));
      }

    }
    else if(cmdcmp("lls", buffer)){
      printf("Local command: ls\n"); 
      getLs("/home/inekar/Documents/git/INGI2346-dist-app-design/telnet", -1);
    }
    else if(cmdcmp("pwd", buffer)){
      printf("Distant command: pwd\n"); 
      msgHeader h;
      h.length = 0;
      h.type = PWD;
      sendHeader(&h, sd1);
      if(read(sd1, buffer, 4096)){
        printf("%s\n", buffer);
      }
    }
    else if(cmdcmp("cd", buffer)){
      printf("Distant command: cd\n"); 
      char* arg;
      getArg("cd", buffer, &arg);

      msgHeader h;
      h.length = strlen(arg)+1;
      h.type = CD;
      sendHeader(&h, sd1);
      getArg("cd", buffer, &arg);     
      sendMsg(arg, sd1);
      if(read(sd1, buffer, 4096)){
        printf("%s\n", buffer);
      }
    }
    else if(cmdcmp("ls", buffer)){
      printf("Distant command: ls\n");
      msgHeader h;
      h.length = 0;
      h.type = LS; 
      sendHeader(&h, sd1);
      while(read(sd1, buffer, 256)){
        if(!strcmp(buffer, "end")){
          printf("end of ls\n");
          break;
        }
        printf("ls : %s\n", buffer);
      }
    }
    else if(cmdcmp("bye", buffer)){
      printf("bye\n");
      // sendMsg(ftok, sd1);
    }
    else if(cmdcmp("get", buffer)){
      printf("Distant command: get\n"); 
      char* arg;
      getArg("get", buffer, &arg);

      msgHeader h;
      h.length = strlen(arg)+1;
      h.type = GET;
      sendHeader(&h, sd1);
      getArg("get", buffer, &arg);     
      sendMsg(arg, sd1);
      /*
      Implement receiving the message
      */
    }
    else if(cmdcmp("put", buffer)){
      printf("Distant command: put\n"); 
      char* arg;
      getArg("put", buffer, &arg);

      msgHeader h;
      h.length = strlen(arg)+1;
      h.type = PUT;
      sendHeader(&h, sd1);
      getArg("put", buffer, &arg);     
      sendMsg(arg, sd1);
      /*
      Implement sending the file
      */
    }
    else{
      printf("Not Today\n");
    }
   
  } 
}

int sendMsg(char* msg, int s){

  int ja;
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

int getString(char* data, char** result, char sep){
  // int ja;
  // for(ja=0; ja<strlen(data); ja++){
  //   printf("tok%i: %i\n", ja, data[ja]);
  // }
  printf("data: %s\n", data);
  // int ja;
  // for(ja=0; ja<strlen(data); ja++){
  //   printf("tok%i: %i\n", ja, data[ja]);
  // }
  // printf("getstr: %s\n", data);
  int i = getStringLength(data, sep);
  printf("len: %i\n", i);
  // printf("getstrlen: %i\n", i);
  char str[i+1];
  int j;
  for(j=0; j<i+1; j++){
    if(j == i){
      str[j] = 0;
    } 
    else {
      str[j] = data[j]; 
    }
    // str[j] = data[j];
    // printf("datachar: %i\n", data[j]);
  }
  printf("getstr: %s\n", str);
  *result = str;
  return 0;
}

int getStringLength(char* str, char sep){
  int ja;
  for(ja=0; ja<strlen(str); ja++){
    printf("tok%i: %i\n", ja, str[ja]);
  }
  printf("getstr: %s\n", str);

  int i=0;
  while(str[i] != sep){
    i++;
  }
  // i++;
  return i;
}

// int fillString(char* data, char* result, int length){
//   int j;
//   for(j=0; j<length; j++){
//     if(j != length-1){
//       result[j] = data[j];
//     } 
//     else {
//       result[j] = '\0';
//     }
//   }
//   return 0;
// }

int cmdcmp(char* cmd, char* str){
  int i;
  for(i=0; i<strlen(cmd); i++){
    if(cmd[i]!=str[i]){
      return 0;
    }
  }
  if(cmd[i]!=0 && cmd[i]!=32 && cmd[i]!=10){
    return 0;
  }
  return 1;
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
  //printf("temp: %s\n", temp);
  *arg_result = temp;
  // printf("targ: %s\n", *arg);

  return 0;
}
