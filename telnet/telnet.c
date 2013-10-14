#include "header.h"
#include "utils.h"
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#define M2_ADDR "130.104.172.88"
#define M2_ADDR "127.0.0.1"



//int shrink(char*, char*);
int getStringLength(char*, char);
int fillString(char*, char*, int);
int sendMsg(char*, int);
int getString(char*, char**, char);
int cmdcmp(char*, char*);

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
      char* arg;
      getArg("lcd", buffer, &arg);
      int j = cd(arg, &current);
      free(arg);
      if (j!=0){
        fprintf(stderr, "Error : %s\n",strerror(j));
      }
    }

    else if(cmdcmp("lls", buffer)){
      printf("Local command: ls\n"); 
      char *current;
      int i = getPwd(&current);
      getLs(current, -1);
    }
    else if(cmdcmp("pwd", buffer)){
      printf("Distant command: pwd\n"); 
      sendType(sd1, PWD, 0);
      if(read(sd1, buffer, 4096)){
        printf("%s\n", buffer);
      }
    }
    else if(cmdcmp("cd", buffer)){
      printf("Distant command: cd\n"); 
      char* arg;
      getArg("cd", buffer, &arg);

      sendType(sd1, CD, strlen(arg)+1);
      sendMsg(arg, sd1);
      if(read(sd1, buffer, 6)){
        printf("%s\n", buffer);
      }
      free(arg);
    }
    else if(cmdcmp("ls", buffer)){
      printf("Distant command: ls\n");

      sendType(sd1, LS, 0);
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

      sendType(sd1, BYE, 0);
      close(sd1);
      break;
    }
    else if(cmdcmp("get", buffer)){
      printf("Distant command: get\n"); 
      char* arg;
      getArg("get", buffer, &arg);

      sendType(sd1, GET, strlen(arg)+1);   
      sendMsg(arg, sd1);

      char *curr_dir;
      int i = getPwd(&curr_dir);
      if(!i){
        char str[strlen(curr_dir) + strlen(arg) + 1];
        strcpy(str, curr_dir);
        strcat(str, "/");
        strcat(str, arg);

        FILE* f = NULL;
        f = fopen(arg, "wb");

        msgHeader in_header;
        read(sd1, &in_header, sizeof(msgHeader));
        printf("nb of full packets: %i\n", in_header.length);

        int i;
        for(i = 0; i<in_header.length; i++){
          char received[GET_PACKET_SIZE];
          read(sd1, received, GET_PACKET_SIZE);
          printf("got it\n");
          fwrite(received, GET_PACKET_SIZE, 1, f);
        }

        read(sd1, &in_header, sizeof(msgHeader));

        if(in_header.type == GET_LAST){
          if(in_header.length != 0){
            printf("last\n");
            char received[in_header.length];
            read(sd1, received, in_header.length);
            fwrite(received, in_header.length, 1, f);
          } else {
            printf("get done\n");
          }
        } else {
          printf("bug\n");
        }

        fclose(f);
      }
      close(arg);
      free(arg);

      /*
      Implement receiving the message
      */
    }
    else if(cmdcmp("put", buffer)){
      printf("Distant command: put\n"); 
      char* arg;
      getArg("put", buffer, &arg);

      sendType(sd1, PUT, strlen(arg)+1);  
      sendMsg(arg, sd1);
      free(arg);
      /*
      Implement sending the file
      */
    }
    else{
      printf("Not Today\n");
    }
   
  } 
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


