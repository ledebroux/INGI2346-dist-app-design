/*
 * Thibaut Knop & Lenoard Debroux
 * Group 24
 * INGI2146 - Mission 3
 * myftp.c
 */

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

  /**
  Call to pwd server side to know the initial directory
  */
  char** pwd;
  pwd = rpwd_1((void *)NULL, cl);
  if (pwd == (char**) NULL){
    clnt_perror(cl,argv[1]);
    exit(1);
  }

  while(1) {
    //printf("%s", *pwd);
    printf(">> ");

    fgets(buffer,512,stdin);

    /**
    Local procedure: pwd
    */
    if(cmdcmp("lpwd", buffer)){
      char *curr_dir;
      int i = getPwd(&curr_dir);
      if(!i){
        printf("%s\n", curr_dir);
      } else {
        fprintf(stderr, "Error : %s\n",strerror(i));
      }
    }

    /**
    Local procedure: cd
    */
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

    /**
    Local procedure: ls
    */
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
    To support several client, the server work directory is know by the client
    so pwd doesn't need a remote procedure call
    */
    else if(cmdcmp("pwd", buffer)){
      printf("%s\n", *pwd);
    }

    /**
    Remote procedure: cd
    */
    else if(cmdcmp("cd", buffer)){
      char* arg;
      getArg("cd", buffer, &arg);
      struct cd_arg cdArg;
      cdArg.path = arg;
      cdArg.pwd = *pwd;

      struct cd_res *result;
      result = rcd_1(&cdArg, cl);
      if(result->code != 0){
        fprintf(stderr, "Error : %s\n",strerror(result->code));
      }
      *pwd = result->pwd;
      free(arg);
    }

    /**
    Remote procedure: ls
    */
    else if(cmdcmp("ls", buffer)){
      char ** result;
      result = rls_1(pwd, cl);
      if (result == (char**) NULL){
        clnt_perror(cl,argv[1]);
        exit(1);
      }
      printf("%s\n", *result);
    }


    else if(cmdcmp("bye", buffer)){
      break;
    }
    
    /**
    Remote procedure: get
    The remote procedure is called with a file_desc as argument.
    it states the filename and an offset.
    Upon receiving a chunck of the file, 
    the method is called again if the chunck wasn't the last.
    For the successive calls, the offset value is updated so that
    the server knows where to begin the new chunck.
    The client doesn't need to know in advance the size of chuncks
    used by the server.
    */
    else if(cmdcmp("get", buffer)){
      char* arg;
      getArg("get", buffer, &arg);
      struct file_desc desc;

      desc.filename = arg;
      desc.offset = 0;
      desc.pwd = *pwd;
      struct file_part *result;

      // reception of the first chunck
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

          // local file creation
          FILE* f = NULL;
          f = fopen(str, "wb+");
          
          //reception of the following chuncks
          while((int)result->last == 0){
            fwrite(result->chunck.chunck_val, 1, result->chunck.chunck_len, f);
            desc.offset = desc.offset + result->chunck.chunck_len;
            result = rget_1(&desc, cl);
          }
          fwrite(result->chunck.chunck_val, 1, result->chunck.chunck_len, f);
          fclose(f);
        }
      }
      free(arg);
    }

    /**
    Remote procedure: put
    To perform such an operation, serveral packets must be send to the server
    For that, serveral calls to a put remote procedure are made.
    A check is done server side to avoid two client to write the same file 
    and thus create conflicts.
    The server doesn't need to know in advance the size of the chuncks
    */
    else if(cmdcmp("put", buffer)){
      char* arg;
      getArg("get", buffer, &arg);
      struct file_put fput;

      fput.filename = arg;
      fput.pwd = *pwd;

      char *curr_dir;
      int i = getPwd(&curr_dir);
      if(!i){
        char str[strlen(curr_dir) + strlen(arg) + 1];
        concatCustom(str, curr_dir, arg);

        // local file opening
        FILE* f = NULL;
        f = fopen(str, "rb");

        if(f != NULL){
          //Size of the file
          fseek(f, 0, SEEK_END);
          int size = ftell(f);
          rewind(f);

          //Number of full packets
          int nb_packets = size/PSIZE;
          int j;

          //Sending all packets that have full size
          for(j = 0; j<nb_packets; j++){
            char part[PSIZE];
            int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);
            fput.chunck.chunck_val = part;
            fput.chunck.chunck_len = PSIZE;
            fput.offset = j*PSIZE;
            int *r = rput_1(&fput, cl);
            if(*r != 0){
              if(*r == 1){
                printf("Error: failed to write correctly server-side\n");
              } 
              else if(*r == 2){
                printf("Error: corruption server-side, retry the copy\n");
              }
            }
          }

          //Sending the last chunck of needed
          if(size%PSIZE != 0){
            char part[size%PSIZE];
            int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);
            fput.chunck.chunck_val = part;
            fput.chunck.chunck_len = size%PSIZE;
            fput.offset = j*PSIZE;
            int *r = rput_1(&fput, cl);
            if(*r != 0){
              if(*r == 1){
                printf("Error: failed to write correctly server-side\n");
              } 
              else if(*r == 2){
                printf("Error: corruption server-side, retry the copy\n");
              }
            }
          }

          fclose(f);
        } else {
          printf("Error: Failed to open file\n");
        }
      }
      free(arg);
    }

    else{
      printf("Unknown command\n");
    }
   
  }

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
