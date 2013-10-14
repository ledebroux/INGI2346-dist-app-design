/*
 * Thibaut Knop & Lenoard Debroux
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

  /* Prepare acknowledging child termination */
  sigset(SIGCHLD, resquiescat);

/*

    sigset, comme signal, est un appel système UNIX standard (pour plus d'info: commande "man sigset").
    Dans le cas présent, sigset dit au système d'exécuter la fonction "resquiescat" lorsque l'événement SIGCHLD se produit, c-à-d lorsqu'un processus fils du processus en cours se termine.
    En outre, l'événement SIGCHLD est masqué pendant l'exécution de "resquiescat". 

*/
  // printf("1");
  /* Open a TCP socket */
  if ( ( sdw = socket (PF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket error in telnetd");
    exit(-1);
  }

  /* Bind our local address so that the client can send to us */
  bzero((char *) &m2 , sizeof( m2 ));

/*

    La structure m2 a été allouée sur la pile. Il faut l'initialiser à zéro avec bzero parce que les prochaines instructions ne rempliront que quelques champs. Sans bzero, les autres champs auraient eu une valeur aléatoire. 

*/
  // printf("2");
  m2.sin_family      = AF_INET; /* address family : Internet */
  m2.sin_addr.s_addr = htonl(INADDR_ANY); 
  m2.sin_port        = htons(TELNETD_PORT);

  if (bind(sdw, (struct sockaddr *) &m2 , sizeof( m2 )) < 0){
    perror("bind error in telnetd");
    exit(-1);
  }

  /* set up queue for connection request */
  if(listen(sdw,5)<0){
    perror("listen error in telnetd");
    exit(-1);
  }

/* DEUXIEME ETAPE (et troisième) */

      clilen = sizeof(m1); 
  for ( ; ; ) {
    // printf("1");
    sigflag = 0;
      /* Wait for a connection from a client process.
       * This is an example of a concurrent server. */

    if ((sd2 = accept(sdw, (struct sockaddr *) &m1 , &clilen))<0)
        /*Accept request from all network interfaces of any machine 
         *(each network interface has a separate IP address) */
    { 

/*

Si on arrive ici, c'est que accept ne s'est pas terminé normalement, c.-à-d. qu'il a renvoyé un code d'erreur (< 0) plutôt qu'un numéro de socket valide. Ceci peut se produire si accept a vraiment échoué ou s'il a été interrompu par un signal SIGCHLD. 

*/

      if(sigflag == 1)continue; 
             /* if we got SIGCHLD during accept, 
              * start a new iteration of the for loop */                                      
      perror("accept error in telnetd");
      exit(-1);
    }

/*

Souvenons nous que nous sommes dans une boucle for infinie

A partir de la deuxième itération, un fils pourrait mourir à tout moment: ceci va produire un signal SIGCHLD.
Puisque le processus père passe la plupart de son temps dans l'appel système accept, SIGCHLD va probablement interrompre accept. Ceci n'est pas une erreur: il faut alors simplement recommencer accept.

*/

/* QUATRIEME ETAPE */

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

        /*
         * pwd: print working directory
         * Computed by the method getPwd
         * The result is sent back to the client
         */
        if (in_header.type == PWD){
          char *curr_dir;
          int i = getPwd(&curr_dir);
          printf("%s\n", curr_dir);
          if(!i){
            write(sd2, curr_dir, strlen(curr_dir)+1);
          }
        } 

        /*
         * ls
         * the result of the ls command is computed by getLs
         * the result is sent to the client in the function call
         */
        else if (in_header.type == LS){
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
        else if (in_header.type == CD){
          printf("cd\n");
          char buffer[in_header.length];

          read(sd2, buffer, in_header.length);

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
         * before sending the last packet,  header is sent to inform 
         * the client of the length of the last packet
         * and the last packet is sent.
         */
        else if (in_header.type == GET){
          printf("get\n");
          char buffer[in_header.length];
          read(sd2, buffer, in_header.length);

          printf("file: %s\n", buffer);

          char *curr_dir;
          int i = getPwd(&curr_dir);
          if(!i){
            char str[strlen(curr_dir) + strlen(buffer) + 1];
            strcpy(str, curr_dir);
            strcat(str, "/");
            strcat(str, buffer);
            FILE* f = NULL;
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
                if(j%1==0){
                  printf("%i/%i\n", j, nb_packets);
                }
              }
              int last_size = size-nb_packets*PACKET_SIZE;
              sendType(sd2, GET_LAST, last_size);
              //printf("last_size: %i\n", last_size);
              if(last_size != 0){
                unsigned char part[last_size];
                int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);

                write(sd2, part, last_size);
              } 
              fclose(f);
            } 
          }
          printf("File sent: %s\n", buffer);
        }

        /*
         * put
         * the length of the filename is given in the header
         * a sent packet gives the name of the file
         * a new file is created.
         * the number of packets of length PACKET_SIZE is sent in a header
         * then, those packets are received.
         * a header is send to tell the size of the last packet
         * then, the last packet is received.
         */
        else if (in_header.type == PUT){
          printf("put\n");

          char buffer[in_header.length];
          read(sd2, buffer, in_header.length);

          printf("file: %s\n", buffer);

          char *curr_dir;
          int i = getPwd(&curr_dir);
          if(!i){
            char str[strlen(curr_dir) + strlen(buffer) + 1];
            strcpy(str, curr_dir);
            strcat(str, "/");
            strcat(str, buffer);

            FILE* f = NULL;
            f = fopen(str, "wb");

            msgHeader in_header;
            read(sd2, &in_header, sizeof(msgHeader));
            printf("nb of full packets: %i\n", in_header.length);

            int j;

            char received[PACKET_SIZE];
            for(j = 0; j<in_header.length; j++){
              
              read(sd2, received, PACKET_SIZE);
              fwrite(received, sizeof(received[0]), sizeof(received)/sizeof(received[0]), f);
              if(j%1==0){
                printf("%i/%i\n", j, in_header.length);
              }
            }
            printf("Packets received %i\n", j);

            msgHeader end_header;
            read(sd2, &end_header, sizeof(end_header));

            if(end_header.type == GET_LAST){
              if(end_header.length != 0){
                char last[end_header.length];
                read(sd2, last, end_header.length);
                fwrite(last, end_header.length, 1, f);
              } else {
                printf("Whole file received\n");
              }
            } else {
              printf("bug\n");
            }
            fclose(f);
          }
        }

        /*
         * bye
         * the server acknolodges that the client is out and terminates.
         */
        else if (in_header.type == BYE){
          printf("Farewell, my beloved friend !\n");
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

/*

    Chaque processus ferme le socket dont il n'a plus besoin. Sans cela, le nombre de sockets ouverts par le père croîtrait chaque fois qu'une nouvelle connexion est acceptée. Comme le nombre de socket est limité, le père finirait par se bloquer par manque de ressources. 

*/



/*

    wait() revient immédiatement si un fils a terminé. Cet appel est nécessaire pour libérer, dans le noyau, les structures de données servant à communiquer au père le code de terminaison du fils; pour plus de détails, faites «man -s 2 wait» et «man -s 2 exit». 

*/ 