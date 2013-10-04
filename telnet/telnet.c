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

  char buffer[255];

  while(strcmp(buffer, "bye")) {
    printf("Enter a command\n");
    fgets(buffer,255,stdin);

    /* Transfer from buffer to a char* input with the exact right size */
    // int i;
    // for(i=0; buffer[i] != '\0'; i++){}
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

    // printf("command = %s\n", ftok);
    // printf("size = %lu\n", sizeof(str));
    // printf("size tok = %lu\n", sizeof(ftok));

    if(!strcmp(str, "lpwd")){
      printf("Local command: pwd\n");
    }
    else if(!strcmp(tok, "lcd")){
      printf("Local command: cd\n"); 
    }
    else if(!strcmp(tok, "lls")){
      printf("Local command: ls\n"); 
    }
    else if(!strcmp(tok, "bye")){
      printf("bye\n");
    }
    else if(!strcmp(tok, "get")){
      printf("get\n");
      // printf("gettt %s\n", tok);
      write(sd1, tok, sizeof(str));
      tok = strtok (NULL, " ,.-");
      tok_length = getStringLength(tok, '\0');
      // int j;
      // for(j=0; j<tok_length; j++){
      //   printf("tok%i: %i\n", j, tok[j]);
      //   if(tok[j] == '\0'){printf("youhou\n");}
      // }
      char temp[tok_length];
      // printf("leng tok = %i\n", tok_length);
      // printf("size tok = %lu\n", sizeof(temp));
      fillString(tok, temp, tok_length);
      printf("file %s\n", temp);
      write(sd1, temp, sizeof(temp));
    }
    else if(!strcmp(tok, "put")){
      printf("put\n");
    }
    else{
      printf("Distant host must perform: %s\n", str);
      write(sd1, str, sizeof(str));
    }

    // while (tok != NULL)
    // {
    //   printf("toksize: %lu", sizeof(tok));
    //   printf ("%s\n",tok);
    //   tok = strtok (NULL, " ,.-");
    // }

  } 
}

int getStringLength(char* str, char sep){
  //printf("sep: %i", sep);
  int i=0;
  while(str[i] != sep){
    // printf("char%i: %i\n", i, str[i]);
    i++;
  }
  i++;
  //i++;
  // for(i=0; str[i] != sep; i++){}
  return i;
}

int fillString(char* data, char* result, int length){
  int j;
  // printf("length : %i\n", length);
  for(j=0; j<length; j++){
    // printf("j : %i", j);
    if(j != length-1){
      result[j] = data[j];
    } else {
      result[j] = '\0';
    }
    
  }
}
