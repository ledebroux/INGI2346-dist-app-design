/*
 * Thibaut Knop & Lenoard Debroux
 * Group 24
 * INGI2146 - Mission 1
 * myftpd.c
 */

#include "header.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>


int sigflag;

void resquiescat(){
  int status;
  wait(&status);
  sigflag = 1;
} /*called by SIGCHLD event handler*/

main (argc, argv) int argc; char *argv[ ];
{
  int sdw, sd2,clilen,childpid;
  struct sockaddr_in m1,m2;

  sigset(SIGCHLD, resquiescat);

  if ( ( sdw = socket (PF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket error in telnetd");
    exit(-1);
  }

  bzero((char *) &m2 , sizeof( m2 ));

  m2.sin_family      = AF_INET; /* address family : Internet */
  m2.sin_addr.s_addr = htonl(INADDR_ANY); 
  m2.sin_port        = htons(TELNETD_PORT);

  if (bind(sdw, (struct sockaddr *) &m2 , sizeof( m2 )) < 0){
    perror("bind error in telnetd");
    exit(-1);
  }

  if(listen(sdw,5)<0){
    perror("listen error in telnetd");
    exit(-1);
  }

      clilen = sizeof(m1); 
  for ( ; ; ) {
    sigflag = 0;

    if ((sd2 = accept(sdw, (struct sockaddr *) &m1 , &clilen))<0)
    { 

      if(sigflag == 1)continue;                                     
      perror("accept error in telnetd");
      exit(-1);
    }

    if((childpid = fork()) < 0){
       perror("fork error in telnetd");
       exit(-1);
    }

    else if (childpid == 0){
      close(sdw);
      
      msgHeader in_header;
      
      /*
       * The server waits for a header to be sent.
       * Upon arrival, the exectution depends on the type of the header.
       */
      printf("Waiting for command\n");
      while(read(sd2, &in_header, sizeof(msgHeader))){
        int type = ntohl(in_header.type);
        int len = ntohl(in_header.length);
        /*
         * pwd: print working directory
         * Computed by the method getPwd
         * The result is sent back to the client
         */
        if (type == PWD){
          char *curr_dir;
          int i = getPwd(&curr_dir);
          printf("%s\n", curr_dir);
          if(!i){
            int j = sendType(sd2, GET_SIZE, strlen(curr_dir)+1);
            if(j==0){
              write(sd2, curr_dir, strlen(curr_dir)+1);
            }
          }else{
            int z = sendType(sd2, ERRNO_RET, i);
            if (z!=0){
              fprintf(stderr, "Error : %s\n",strerror(z));
            }
          }
        } 

        /*
         * ls
         * the result of the ls command is computed by getLs
         * the result is sent to the client in the function call
         */
        else if (type == LS){
          printf("ls\n");
          char *curr_dir;
          int i = getPwd(&curr_dir);
          if(!i){
            getLs(curr_dir, sd2);
          }
        } 

        /*
         * cd
         * the length of the path in argument is given in the header
         * the path is given in the packet that is read below
         * the current directoryis then changed
         */
        else if (type == CD){
          printf("cd\n");
          char buffer[len];

          read(sd2, buffer, len);

          char * current;
          int i = getPwd(&current);

          int j = cd(buffer, &current);
          if(j == 0){
            write(sd2, "ok!", strlen("ok!")+1);
          } else{
            write(sd2, "fail!", strlen("fail!")+1);
          }
        }

        /*
         * get
         * the length of the filename is given in the header
         * the next read packet states the file to get.
         * the server then sends the number of packet of PACKET_SIZE size
         * it will send to the client.
         * then, the file is splitted and sent
         * before sending the last packet, a header is sent to inform 
         * the client of the length of the last packet
         * and the last packet is sent.
         */
        else if (type == GET){
          printf("get\n");
          char buffer[len];
          read(sd2, buffer, len);

          char *curr_dir;
          int i = getPwd(&curr_dir);
          if(!i){
            char str[strlen(curr_dir) + strlen(buffer) + 1];
            strcpy(str, curr_dir);
            strcat(str, "/");
            strcat(str, buffer);
            FILE* f = NULL;
            errno = 0;
            f = fopen(str, "rb");
            if(f != NULL){
              fseek(f, 0, SEEK_END);
              int size = ftell(f);
              rewind(f);
              int nb_packets = size/PACKET_SIZE;
              sendType(sd2, GET_SIZE, nb_packets);
              int j;
              for(j = 0; j<nb_packets; j++){
                unsigned char part[PACKET_SIZE];
                int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);
                write(sd2, part, PACKET_SIZE);
              }
              int last_size = size-nb_packets*PACKET_SIZE;
              sendType(sd2, GET_LAST, last_size);
              if(last_size != 0){
                unsigned char part[last_size];
                int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);

                write(sd2, part, last_size);
              } 
              fclose(f);
              printf("File sent: %s\n", buffer);
            } else {
              sendType(sd2, ERRNO_RET, errno);
            }
          }
        }

        /*
         * put
         * the length of the filename is given in the header
         * a sent packet gives the name of the file
         * a new file is created.
         * the number of packets of length PACKET_SIZE is received in a header
         * then, those packets are received.
         * a header is received to tell the size of the last packet
         * then, the last packet is received.
         */
        else if (type == PUT){
          printf("put\n");
          char buffer[len];
          read(sd2, buffer, len);

          char *curr_dir;
          int i = getPwd(&curr_dir);
          if(!i){
            char str[strlen(curr_dir) + strlen(buffer) + 1];
            strcpy(str, curr_dir);
            strcat(str, "/");
            strcat(str, buffer);

            msgHeader in_header;
            read(sd2, &in_header, sizeof(msgHeader));

            if(ntohl(in_header.type) != ERRNO_RET){
              FILE* f = NULL;
              f = fopen(str, "wb");

              len = ntohl(in_header.length);

              int j;

              char received[PACKET_SIZE];
              for(j = 0; j<len; j++){ 
                read(sd2, received, PACKET_SIZE);
                fwrite(received, sizeof(received[0]), sizeof(received)/sizeof(received[0]), f);
              }

              msgHeader end_header;
              read(sd2, &end_header, sizeof(end_header));

              if(ntohl(end_header.type) == GET_LAST){
                int elen = ntohl(end_header.length);
                if(elen != 0){
                  char last[elen];
                  read(sd2, last, elen);
                  fwrite(last, sizeof(last[0]), sizeof(last)/sizeof(last[0]), f);
                }
                printf("File received: %s\n", buffer);
              }
              fclose(f);
            } else {
              printf("Error: shouldn't be reached");
            }
          }
        }

        /*
         * bye
         * the server acknolodges that the client is out and terminates.
         */
        else if (type == BYE){
          printf("Client disconnected !\n");
          close(sd2);
          break;
        }
        else {
          printf("Error: shouldn't be reached");
        }
        printf("Waiting for command\n");
      }
      exit(0);
    }
    
    close(sd2); /* parent process */
  }
}
