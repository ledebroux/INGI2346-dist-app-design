/*
 * Thibaut Knop & Lenoard Debroux
 * INGI2146 - Mission 1
 * myftp.c
 */

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

  /*
   * Normally the buffer should be 4096 long, since the MAX_PATH is 4096.
   * However, for the purpose of this assignment, we think 512 is a good tradeoff
   * between user possibility and performance.
   */
  char buffer[512];

  /*
   * Compares the content of the buffer (filled from stdin)
   * and performs the different operations
   */

  while(strcmp(buffer, "bye")) {
    printf("Enter a command\n");
    fgets(buffer,512,stdin);

    /*
     * Local command pwd : retrieve the current directory and print in on sdtout 
     */
    if(cmdcmp("lpwd", buffer)){
      printf("Local command: pwd\n");
      char *curr_dir;
      int i = getPwd(&curr_dir);
      if(!i){
        printf("%s\n", curr_dir);
      }
    }

    /*
     * Local command cd : change the current directory
     * Save first the current directory, 
     * then retrieve the arg from the user input
     * and pass thoses references to the cd method (from utils.c) 
     */
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

    /*
     * Local command ls : retrieve entries from current path
     * Save first the current directory, 
     * and pass that reference to the ls method (from utils.c) 
     * with s = -1 (local command)
     */
    else if(cmdcmp("lls", buffer)){
      printf("Local command: ls\n"); 
      char *current;
      int i = getPwd(&current);
      getLs(current, -1);
    }

    /*
     * Distant command pwd : return the current path from the environment of the server
     * Send a header containing the type of the message : PWD.
     * The length field of the header contains 0, since there is no need 
     * for the server to read something. The type PWD is enough.
     * The client waits for the response from the server : since a path can be MAX_PATH long,
     * and MAX_PATH is 4096, we read for 4096 bits.
     */

     // TODO : change 4096 : more usefull to send back a header that contains the length of the path to read
    else if(cmdcmp("pwd", buffer)){
      printf("Distant command: pwd\n"); 
      sendType(sd1, PWD, 0);
      if(read(sd1, buffer, 4096)){
        printf("%s\n", buffer);
      }
    }

    /*
     * Distant command cd : change the current directory of the environment of the server
     * Retrieve the path from the user input,
     * Send a header containing the type of the command CD, and the length of the path to be read
     * The server will read the header, and then knows that he has to read a certain amount of bytes 
     * (indicated by the length field in the header)
     * Then send the arg to the server
     * The client waits for the response from the server, receiving first a header from the server
     * who announces the length the client has to read. 
     */
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
      printf("Bye\n");

      sendType(sd1, BYE, 0);
      close(sd1);
      break;
    }
    else if(cmdcmp("get", buffer)){
      printf("Distant command: get\n"); 
      char* arg;
      getArg("get", buffer, &arg);
      if(strlen(arg) > 0){
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
          f = fopen(str, "wb");

          msgHeader in_header;
          read(sd1, &in_header, sizeof(msgHeader));
          printf("nb of full packets: %i\n", in_header.length);

          int j;

          char received[PACKET_SIZE];
          for(j = 0; j<in_header.length; j++){
            
            read(sd1, received, PACKET_SIZE);
            fwrite(received, sizeof(received[0]), sizeof(received)/sizeof(received[0]), f);
            if(j%1==0){
              printf("%i/%i\n", j, in_header.length);
            }
          }

          msgHeader end_header;
          read(sd1, &end_header, sizeof(end_header));

          if(end_header.type == GET_LAST){
            if(end_header.length != 0){
              char last[end_header.length];
              read(sd1, last, end_header.length);
              fwrite(last, end_header.length, 1, f);
            } else {
              printf("Whole file received\n");
            }
          } 
          fclose(f);
        }
        free(arg); 
      } else {
        printf("get requires an argument\n");
      }
    }
    else if(cmdcmp("put", buffer)){
      printf("Distant command: put\n"); 
      char* arg;
      getArg("put", buffer, &arg);

      if(strlen(arg) > 0){
        sendType(sd1, PUT, strlen(arg)+1);  
        sendMsg(arg, sd1);
        
        char *curr_dir;
        int i = getPwd(&curr_dir);
        if(!i){

          char str[strlen(curr_dir) + strlen(arg) + 1];
          strcpy(str, curr_dir);
          strcat(str, "/");
          strcat(str, arg);
          FILE* f = NULL;
          f = fopen(str, "rb");
          if(f != NULL){

            fseek(f, 0, SEEK_END);
            int size = ftell(f);
            rewind(f);

            printf("file len: %i\n", size);
            

            int nb_packets = size/PACKET_SIZE;

            printf("nb_packets: %i\n", nb_packets);

            sendType(sd1, GET_SIZE, nb_packets);
            int j;
            for(j = 0; j<nb_packets; j++){
              unsigned char part[PACKET_SIZE];
              int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);
              write(sd1, part, PACKET_SIZE);
              if(j%1==0){
                printf("%i/%i\n", j, nb_packets);
              }
            }

            int last_size = size-nb_packets*PACKET_SIZE;
            sendType(sd1, GET_LAST, last_size);
            printf("last_size: %i\n", last_size);
            if(last_size != 0){
              unsigned char part[last_size];
              int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);

              write(sd1, part, last_size);
            }
            fclose(f);
          } 
        }
        printf("File sent: %s\n", arg);
        free(arg);
      } else {
        printf("put requires an argument\n");
      }
      
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


