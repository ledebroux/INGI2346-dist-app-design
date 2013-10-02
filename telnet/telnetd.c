#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#define TELNETD_PORT 8008

int sigflag;

main (argc, argv) int argc; char *argv[ ];
{
  int sdw, sd2,clilen,childpid;
  struct sockaddr_in m1,m2;

  /* Prepare acknowledging child termination */
  //sigset(SIGCHLD, resquiescat);

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
      printf("pid 0\n");
      close(sdw);
      
      char a[10];
      

      while(read(sd2, a, sizeof(a))){
        printf("I received %s\n", a);
      }
          /*child process */
      exit(0);
    }
    
    close(sd2); /* parent process */
  }
}

/*

    Chaque processus ferme le socket dont il n'a plus besoin. Sans cela, le nombre de sockets ouverts par le père croîtrait chaque fois qu'une nouvelle connexion est acceptée. Comme le nombre de socket est limité, le père finirait par se bloquer par manque de ressources. 

*/

//int resquiescat(){wait();sigflag = 1;} /*called by SIGCHLD event handler*/

/*

    wait() revient immédiatement si un fils a terminé. Cet appel est nécessaire pour libérer, dans le noyau, les structures de données servant à communiquer au père le code de terminaison du fils; pour plus de détails, faites «man -s 2 wait» et «man -s 2 exit». 

*/ 