#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define TELNETD_PORT  8000
//#define M2_ADDR "130.104.172.88"
#define M2_ADDR "127.0.0.1"

int getInputString(char*);

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
  char *dest;
  while(strcmp(dest, "close")) {
    printf("Enter a command\n");
	getInputString(dest);
    printf("comand %c", dest[0]);
    if(dest[0]=='l'){
      printf("Local command: %s\n", dest);
    }
    else{
      printf("Distant host must perform: %s\n", dest);
      write(sd1, dest, sizeof(dest));
    }
    
    printf("c: %lu", sizeof(dest));
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

int getInputString(char* pStr){
	unsigned int len_max = 128;
    unsigned int current_size = 0;
 
    pStr = malloc(len_max);
    current_size = len_max;
 
    if(pStr != NULL)
    {
	int c = EOF;
	unsigned int i =0;
        //accept user input until hit enter or end of file
	while (( c = getchar() ) != '\n' && c != EOF)
	{
		pStr[i++]=(char)c;
 
		//if i reached maximize size then realloc size
		if(i == current_size)
		{
            current_size = i+len_max;
			pStr = realloc(pStr, current_size);
		}
	}
 
	pStr[i] = '\0';       
    }
    return 0;	
}