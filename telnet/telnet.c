#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define TELNETD_PORT  8008
//#define M2_ADDR "130.104.172.88"
#define M2_ADDR "127.0.0.1"

int shrink(char*, char*);

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
  char a[10];

  while(strcmp(a, "bye")) {
    printf("Enter a command\n");
    gets(a);

    char * tok;
    tok = strtok (a," ");

    if(a[0]=='l'){
      printf("Local command: %s\n", a);
    }
    else if(!strcmp(tok, "bye")){
      printf("bye\n");
    }
    else if(!strcmp(tok, "get")){
      printf("get\n");
      tok = strtok (NULL, " ,.-");
      printf("file %s\n", tok);
    }
    else if(!strcmp(tok, "put")){
      printf("put\n");
    }
    else{
      printf("Distant host must perform: %s\n", a);
      write(sd1, a, sizeof(a));
    }

    // while (tok != NULL)
    // {
    //   printf ("%s\n",tok);
    //   tok = strtok (NULL, " ,.-");
    // }

  } 
}

int shrink(char* chaine, char* output){
  int i;
  int j;
  for(i=0; chaine[i] != '\0'; i++){}
  i++;
  output = realloc(output, i*sizeof(char));
  for(j=0; j<i; j++){
      output[j] = chaine[j];
  }
  return 0;
}