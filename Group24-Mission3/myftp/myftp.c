/*
 * Thibaut Knop & Lenoard Debroux
 * Group 24
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
#include <arpa/inet.h>
#include <errno.h>

#include <rpc/rpc.h>
#include "rpspec.h"
#include <unistd.h>

#define PSIZE 1024

main(argc,argv)
int argc;
char *argv[ ];
{
  CLIENT *cl;

  if(argc != 2){
    printf("usage: client hostname_of_server\n");
    exit(1);
  }

  cl = clnt_create(argv[1],PROG,VERS,"tcp");
  if ( cl == (CLIENT *) NULL ){
    clnt_pcreateerror(argv[1]);
    exit(1);
  }

  char buffer[512];

  while(1) {
    printf(">> ");
    fgets(buffer,512,stdin);


    if(cmdcmp("lpwd", buffer)){
      char *curr_dir;
      int i = getPwd(&curr_dir);
      if(!i){
        printf("%s\n", curr_dir);
      } else {
        fprintf(stderr, "Error : %s\n",strerror(i));
      }
    }


    else if(cmdcmp("lcd", buffer)){
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


    else if(cmdcmp("lls", buffer)){
      char *current;
      int i = getPwd(&current);
      if (i==0){
        char *result;
        int j = getLs(current, &result, 1);
        if(j!=0){
          fprintf(stderr, "Error : %s\n",strerror(j));
        }
      } else {
        fprintf(stderr, "Error : %s\n",strerror(i));
      }
    }

    /**
    Remote procedure: pwd
    */
    else if(cmdcmp("pwd", buffer)){
      char ** result;
      result = rpwd_1((void *)NULL, cl);
      if (result == (char**) NULL){
        clnt_perror(cl,argv[1]);
        exit(1);
      }
      printf("%s\n", *result);
    }

    /**
    Remote procedure: cd
    */
    else if(cmdcmp("cd", buffer)){
      char* arg;
      getArg("cd", buffer, &arg);
      int *i = rcd_1(&arg, cl);
      if(*i!=0){
        fprintf(stderr, "Error : %s\n",strerror(*i));
      }
      free(arg);
    }

    /**
    Remote procedure: ls
    */
    else if(cmdcmp("ls", buffer)){
      char ** result;
      result = rls_1((void *)NULL, cl);
      if (result == (char**) NULL){
        clnt_perror(cl,argv[1]);
        exit(1);
      }
      printf("%s\n", *result);
    }


    else if(cmdcmp("bye", buffer)){
      break;
    }
    

    else if(cmdcmp("get", buffer)){
      char* arg;
      getArg("get", buffer, &arg);
      struct file_desc desc;

      desc.filename = arg;
      desc.offset = 0;
      struct file_part *result;

      result = rget_1(&desc, cl);
      if(result == (file_part*) NULL){
        clnt_perror(cl,argv[1]);
        exit(1);
      }
      if(result->last < 0){
        //error from server
        printf("chunck: %s\n", result->chunck.chunck_val);
      } else {
        char *curr_dir;
        int i = getPwd(&curr_dir);
        if(!i){
          char str[strlen(curr_dir) + strlen(arg) + 1];
          concatCustom(str, curr_dir, arg);
          FILE* f = NULL;
          f = fopen(str, "wb+");
          
          while((int)result->last == 0){
            fwrite(result->chunck.chunck_val, 1, result->chunck.chunck_len, f);
            desc.offset = desc.offset + result->chunck.chunck_len;
            result = rget_1(&desc, cl);
          }
          fwrite(result->chunck.chunck_val, 1, result->chunck.chunck_len, f);
          fclose(f);
        }
      }
    }


    else if(cmdcmp("put", buffer)){
      char* arg;
      getArg("get", buffer, &arg);
      struct file_put fput;

      fput.filename = arg;

      char *curr_dir;
      int i = getPwd(&curr_dir);
      if(!i){
        char str[strlen(curr_dir) + strlen(arg) + 1];
        concatCustom(str, curr_dir, arg);

        FILE* f = NULL;
        f = fopen(str, "rb");

        errno = 0;

        if(f != NULL){
          fseek(f, 0, SEEK_END);
          int size = ftell(f);
          rewind(f);

          //Number of full packets
          int nb_packets = size/PSIZE;
          int j;
          for(j = 0; j<nb_packets; j++){
            char part[PSIZE];
            int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);
            printf("offset: %s\n", part);
            fput.chunck.chunck_val = part;
            fput.chunck.chunck_len = PSIZE;
            fput.offset = j*PSIZE;
            int *r = rput_1(&fput, cl);
          }

          char part[size%PSIZE];
          int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);
          fput.chunck.chunck_val = part;
          fput.chunck.chunck_len = size%PSIZE;
          int *r = rput_1(&fput, cl);

          fclose(f);
        } else {
          printf("Error: Failed to open file");
        }
      }
    }


    else{
      printf("Unknown command\n");
    }
   
  }

  // data.arga1 = 5;
  // data.arga2 = 3;
  // result = rproca_1(&data, cl);
  // if (result == (double*) NULL){
  //   clnt_perror(cl,argv[1]);
  //   exit(1);
  // }
  // printf("the result of RPROCA is %f\n", *result);

  // pdata = rprocb_1((void *)NULL, cl);
  // if  (pdata == (arga*) NULL){
  //   clnt_perror(cl,argv[1]);
  //   exit(1);
  // }
  // printf("the arga1 field is %d\n", pdata->arga1);
  // printf("the arga2 field is %d\n", pdata->arga2);

  // res3 = rprocc_1((void *)NULL, cl);
  // if (res3 == (char**) NULL){
  //   clnt_perror(cl,argv[1]);
  //   exit(1);
  // }
  // printf("the result of RPROCC is %s\n", *res3);

  clnt_destroy(cl);
}

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


// int getStringLength(char*, char);
// int fillString(char*, char*, int);
// int sendMsg(char*, int);
// int getString(char*, char**, char);
// int cmdcmp(char*, char*);

// main(argc, argv) int    argc; char   *argv[ ];
// {
//   char* addr;
//   if(argc > 1){
//     addr = argv[1];
//   } 
//   else {
//     addr = "127.0.0.1";
//   }
//   int sd1;
//   struct sockaddr_in     m2;

//   bzero((char *) &m2       , sizeof(m2));
//   m2.sin_family     = AF_INET;
//   m2.sin_addr.s_addr = inet_addr(addr);
//   m2.sin_port       = htons(TELNETD_PORT);

//   if ( (sd1 = socket(PF_INET, SOCK_STREAM, 0)) < 0){
//     perror("socket error in telnet");
//     exit(-1);
//   }

//   if (connect(sd1 , (struct sockaddr *) &m2 , sizeof ( m2 )) < 0){
//     perror("connect error in telnet");
//     exit(-1);
//   }

//   /*
//    * Normally the buffer should be 4096 long, since the MAX_PATH is 4096.
//    * However, for the purpose of this assignment, we think 512 is a good tradeoff
//    * between user possibility and performance.
//    */
//   char buffer[512];
//   msgHeader in_header;

//   /*
//    * Compares the content of the buffer (filled from stdin)
//    * and performs the different operations
//    */

//   while(strcmp(buffer, "bye")) {
//     printf(">> ");
//     fgets(buffer,512,stdin);

//     /*
//      * Local command pwd : retrieve the current directory and print in on sdtout 
//      */
//     if(cmdcmp("lpwd", buffer)){
//       char *curr_dir;
//       int i = getPwd(&curr_dir);
//       if(!i){
//         printf("%s\n", curr_dir);
//       } else {
//         fprintf(stderr, "Error : %s\n",strerror(i));
//       }
//     }

//     /*
//      * Local command cd : change the current directory
//      * Save first the current directory, 
//      * then retrieve the arg from the user input
//      * and pass thoses references to the cd method (from utils.c) 
//      */
//     else if(cmdcmp("lcd", buffer)){
//       char * current;
//       int i = getPwd(&current);
//       char* arg;
//       getArg("lcd", buffer, &arg);
//       int j = cd(arg, &current);
//       free(arg);
//       if (j!=0){
//         fprintf(stderr, "Error : %s\n",strerror(j));
//       }
//     }

//     /*
//      * Local command ls : retrieve entries from current path
//      * Save first the current directory, 
//      * and pass that reference to the ls method (from utils.c) 
//      * with s = -1 (local command)
//      */
//     else if(cmdcmp("lls", buffer)){
//       char *current;
//       int i = getPwd(&current);
//       if (i==0){
//         int j = getLs(current, -1);
//         if(j!=0){
//           fprintf(stderr, "Error : %s\n",strerror(j));
//         }
//       } else {
//         fprintf(stderr, "Error : %s\n",strerror(i));
//       }
//     }

//     /*
//      * Distant command pwd : return the current path from the environment of the server
//      * Send a header containing the type of the message : PWD.
//      * The length field of the header contains 0, since there is no need 
//      * for the server to read something. The type PWD is enough.
//      * The client waits for a header from the server to know how much bytes to read
//      */
//     else if(cmdcmp("pwd", buffer)){
//       sendType(sd1, PWD, 0);
//       if(read(sd1, &in_header, sizeof(msgHeader))){
//         int len = ntohl(in_header.length);
//         if(ntohl(in_header.type) == GET_SIZE){
//           read(sd1, buffer, len);
//           printf("%s\n", buffer);
//         }else{
//           fprintf(stderr, "Error : %s\n",strerror(len));
//         }
//       }
//     }

//     /*
//      * Distant command cd : change the current directory of the environment of the server
//      * Retrieve the path from the user input,
//      * Send a header containing the type of the command CD, and the length of the path to be read
//      * The server will read the header, and then knows that he has to read a certain amount of bytes 
//      * (indicated by the length field in the header)
//      * Then send the arg to the server
//      * The client waits for the response from the server, receiving first a header from the server
//      * who announces the length the client has to read. 
//      */
//     else if(cmdcmp("cd", buffer)){
//       char* arg;
//       getArg("cd", buffer, &arg);

//       sendType(sd1, CD, strlen(arg)+1);
//       sendMsg(arg, sd1);
//       if(read(sd1, buffer, 6)){
//         if(strcmp(buffer, "ok!")){
//           printf("Error: cd failed\n");
//         }
//       }
//       free(arg);
//     }

//     /*
//      * Distant command ls : retrieve entries from current path for the environment of the server
//      * Send a header containg the type of the command, LS.
//      * The length field of the header contains 0, since there is no need 
//      * for the server to read something. The type LS is enough.
//      * The client waits for the response from the server : since the server writes every entries he found,
//      * the client read every entries until it reads end.
//      */
//     else if(cmdcmp("ls", buffer)){
//       sendType(sd1, LS, 0);
//       while(read(sd1, buffer, 256)){
//         if(buffer[0] == 10){
//           read(sd1, &in_header, sizeof(msgHeader));
//           if(ntohl(in_header.type) == ERRNO_RET && ntohl(in_header.length) != 0){
//             fprintf(stderr, "Error : %s\n",strerror(ntohl(in_header.length)));
//           }
//           break;
//         }
//         printf("%s\n", buffer);
//       }
//     }

//     /*
//      * Distant command bye:
//      * the client wishes to terminate
//      * it informs the server and terminates
//      */
//     else if(cmdcmp("bye", buffer)){
//       sendType(sd1, BYE, 0);
//       close(sd1);
//       break;
//     }
    
//     /*
//      * Distant command get:
//      * the client wants to retrieve a file from the server
//      * the length of the filename is send in the header
//      * the filename is send to the server
//      * a new file is created client-side
//      * the number of packets of length PACKET_SIZE is received in a header
//      * then, those packets are received.
//      * a header is received to tell the size of the last packet
//      * then, the last packet is received.
//      */
//     else if(cmdcmp("get", buffer)){
//       char* arg;
//       getArg("get", buffer, &arg);
//       if(strlen(arg) > 0){
//         sendType(sd1, GET, strlen(arg)+1);   
//         sendMsg(arg, sd1);

//         char *curr_dir;
//         int i = getPwd(&curr_dir);
//         if(!i){
//           char str[strlen(curr_dir) + strlen(arg) + 1];
//           strcpy(str, curr_dir);
//           strcat(str, "/");
//           strcat(str, arg);

//           read(sd1, &in_header, sizeof(msgHeader));
          
//           if(ntohl(in_header.type) != ERRNO_RET){
//             FILE* f = NULL;
//             f = fopen(str, "wb");

//             int len = ntohl(in_header.length);

//             int j;

//             char received[PACKET_SIZE];
//             for(j = 0; j<len; j++){   
//               read(sd1, received, PACKET_SIZE);
//               fwrite(received, sizeof(received[0]), sizeof(received)/sizeof(received[0]), f);
//             }

//             msgHeader end_header;
//             read(sd1, &end_header, sizeof(end_header));

//             if(ntohl(end_header.type) == GET_LAST){
//               int elen = ntohl(end_header.length);
//               if(elen != 0){
//                 char last[elen];
//                 read(sd1, last, elen);
//                 fwrite(last, sizeof(last[0]), sizeof(last)/sizeof(last[0]), f);
//               } else {
                
//               }
//               printf("File received: %s\n", arg);
//             } 
//             fclose(f);
//           } else {
//             fprintf(stderr, "Error : %s\n",strerror(ntohl(in_header.length)));
//           }
          
//         }
//         free(arg); 
//       } else {
//         printf("get requires an argument\n");
//       }
//     }

//     /*
//      * get
//      * the length of the filename is sent in the header
//      * the filename is sent in the next packet
//      * the client then sends the number of packet of PACKET_SIZE size
//      * to be sent.
//      * then, the file is splitted and sent
//      * before sending the last packet, a header is sent to inform 
//      * the server of the length of the last packet
//      * and the last packet is sent.
//      */
//     else if(cmdcmp("put", buffer)){
//       char* arg;
//       getArg("put", buffer, &arg);

//       if(strlen(arg) > 0){
//         sendType(sd1, PUT, strlen(arg)+1);  
//         sendMsg(arg, sd1);
        
//         char *curr_dir;
//         int i = getPwd(&curr_dir);
//         if(!i){

//           char str[strlen(curr_dir) + strlen(arg) + 1];
//           strcpy(str, curr_dir);
//           strcat(str, "/");
//           strcat(str, arg);
//           FILE* f = NULL;
//           errno = 0;
//           f = fopen(str, "rb");
//           if(f != NULL){

//             fseek(f, 0, SEEK_END);
//             int size = ftell(f);
//             rewind(f);

//             int nb_packets = size/PACKET_SIZE;

//             sendType(sd1, GET_SIZE, nb_packets);
//             int j;
//             for(j = 0; j<nb_packets; j++){
//               unsigned char part[PACKET_SIZE];
//               int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);
//               write(sd1, part, PACKET_SIZE);
//             }

//             int last_size = size-nb_packets*PACKET_SIZE;
//             sendType(sd1, GET_LAST, last_size);
//             if(last_size != 0){
//               unsigned char part[last_size];
//               int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);

//               write(sd1, part, last_size);
//             }
//             fclose(f);
//             printf("File sent: %s\n", arg);
//           } 
//           else {
//             printf("File not found\n");
//             sendType(sd1, ERRNO_RET, errno);
//           }
//         }
        
//         free(arg);
//       } else {
//         printf("put requires an argument\n");
//       }
      
//     }
//     else{
//       printf("Unknown command\n");
//     }
   
//   } 
// }

// int getString(char* data, char** result, char sep){
//   int i = getStringLength(data, sep);
//   char str[i+1];
//   int j;
//   for(j=0; j<i+1; j++){
//     if(j == i){
//       str[j] = 0;
//     } 
//     else {
//       str[j] = data[j]; 
//     }
//   }
//   *result = str;
//   return 0;
// }

// int getStringLength(char* str, char sep){
//   int i=0;
//   while(str[i] != sep){
//     i++;
//   }
//   return i;
// }

// int cmdcmp(char* cmd, char* str){
//   int i;
//   for(i=0; i<strlen(cmd); i++){
//     if(cmd[i]!=str[i]){
//       return 0;
//     }
//   }
//   if(cmd[i]!=0 && cmd[i]!=32 && cmd[i]!=10){
//     return 0;
//   }
//   return 1;
// }


