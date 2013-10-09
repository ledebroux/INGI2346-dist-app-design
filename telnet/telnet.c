#include "header.h"
#include "utils.h"
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define TELNETD_PORT  8003
//#define M2_ADDR "130.104.172.88"
#define M2_ADDR "127.0.0.1"

//int shrink(char*, char*);
int getStringLength(char*, char);
int fillString(char*, char*, int);
int sendMsg(char*, int);

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

  while(strcmp(buffer, "bye")) {
    printf("Enter a command\n");
    fgets(buffer,255,stdin);

    /* Transfer from buffer to a char* input with the exact right size */

    int i = getStringLength(buffer, 10);
    char str[i];
    printf("leng i = %i\n", i);
    printf("size str = %lu\n", sizeof(str));
    fillString(buffer, str, i);

    char * tok;
    tok = strtok (str," ");

    int tok_length = getStringLength(tok, '\0');
    char ftok[tok_length];
    fillString(tok, ftok, tok_length);

    // int j;
    // for(j=0; j<i; j++){
    //   printf("tok%i: %i\n", j, tok[j]);
    //   if(tok[j] == '\n'){printf("youhou\n");}
    // }

    if(!strcmp(ftok, "lpwd")){
      printf("Local command: pwd\n");
      char *curr_dir;
      int i = getPwd(&curr_dir);
      if(!i){
        printf("%s", curr_dir);
      }
    }
    else if(!strcmp(ftok, "lcd")){
      printf("Local command: cd\n"); 
    }
    else if(!strcmp(ftok, "lls")){
      printf("Local command: ls\n"); 
      getLs("/home/inekar/Documents/git/INGI2346-dist-app-design/telnet", -1);
      char* buffer;
    }
    else if(!strcmp(ftok, "pwd")){
      printf("Distant command: pwd\n"); 
      msgHeader h;
      h.length = 0;
      h.type = PWD;
      sendHeader(&h, sd1);
      if(read(sd1, buffer, 256)){
        printf("%s\n", buffer);
      }
    }
    else if(!strcmp(ftok, "cd")){
      printf("Distant command: cd\n"); 
      sendMsg(ftok, sd1);
    }
    else if(!strcmp(ftok, "ls")){
      printf("Distant command: ls\n");
      msgHeader h;
      h.length = 0;
      h.type = LS; 
      sendHeader(&h, sd1);
      while(read(sd1, buffer, 256)){
        printf("ls : %s\n", buffer);
        // printf("size : %lu\n", sizeof(buffer));
        if(!strcmp(buffer, "end")){
          printf("end of ls");
          break;
        }
      }
    }
    else if(!strcmp(ftok, "bye")){
      printf("bye\n");
      sendMsg(ftok, sd1);
    }
    else if(!strcmp(ftok, "get")){
      printf("get\n");
      sendMsg(ftok, sd1);
      tok = strtok (NULL, " ,.-");
      tok_length = getStringLength(tok, '\0');
      char temp[tok_length];
      fillString(tok, temp, tok_length);
      printf("file %s\n", temp);
      //the first two char are skipped
      sendMsg(temp, sd1);
    }
    else if(!strcmp(ftok, "put")){
      printf("put\n");
      sendMsg(ftok, sd1);
    }
    else{
      printf("Not Today");
    }

    // while (tok != NULL)
    // {
    //   printf("toksize: %lu", sizeof(tok));
    //   printf ("%s\n",tok);
    //   tok = strtok (NULL, " ,.-");
    // }

  } 
}

int sendMsg(char* msg, int s){
  write(s, msg, sizeof(msg));
  return 0;
}

int sendHeader(msgHeader* h, int s){
  write(s, h, sizeof(h));
  return 0;
}

int getStringLength(char* str, char sep){
  int i=0;
  while(str[i] != sep){
    i++;
  }
  i++;
  return i;
}

int fillString(char* data, char* result, int length){
  int j;
  for(j=0; j<length; j++){
    if(j != length-1){
      result[j] = data[j];
    } else {
      result[j] = '\0';
    }
  }
  return 0;
}
