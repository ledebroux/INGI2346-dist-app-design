#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define TELNETD_PORT  8000
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

  char a[256];


  while(strcmp(a, "bye")) {
    printf("Enter a command\n");
    gets(a);
    // printf("comand %c\n", a[0]);
    // printf("coman2 %c\n", a[1]);
    // if(a[1]=='\0'){
    //   printf("woot\n");
    // } else {
    //   printf("ooo + %i\n", a[1]);
    // }

    // if(a[2]=='\0'){
    //   printf("woot2 + %i\n", a[1]);
    // } else {
    //   printf("ooo2 + %i\n", a[1]);
    // }

    // char o[256];
    // shrink(a, o);
    // printf("size %lu\n", sizeof(o));

    char * tok;
    tok = strtok (a," ");
    printf("tok %s\n", tok);
    // printf("length %lu\n", sizeof(tok));
    // int i;
    // for(i=0; i<sizeof(tok); i++){
    //   printf("elemnt %c\n", tok[i]);
    // }
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

    while (tok != NULL)
    {
      printf ("%s\n",tok);
      tok = strtok (NULL, " ,.-");
    }
    printf("c: %lu", sizeof(a));
    printf("c: %lu", sizeof("close"));
    // int i;
    // for(i=0; i<=sizeof(a); i++){
    //   printf("%c\n", a[i]);
    // }

  }
  // printf("Integer that you have entered is %s\n", a);

  // write(sd1, a, sizeof(a));
  // sleep(20);
  // int i;
  // for( i=0 ; ; i++ ){
  //     if (i%1000000000==0){
  //         printf("%i", i);
  //     }
  // } 
}
