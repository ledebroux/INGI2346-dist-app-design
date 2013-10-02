#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define TELNETD_PORT  7000
#define M2_ADDR "130.104.172.88"

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
          printf("ok");
}