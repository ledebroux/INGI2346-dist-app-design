/*
 * Thibaut Knop & Lenoard Debroux
 * Group 24
 * INGI2146 - Mission 3
 * myftpd.c
 */

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

#include <rpc/rpc.h>
#include "rpspec.h"
#include <unistd.h>

#define PSIZE 1024

char *pwd_ini = NULL;

/**
pwd
is called upon a client connection
pwd_ini is set if not.
That procedure inform the client of the initial work directory
*/
char **rpwd_1_svc(none, rqstp)
void *none;
struct svc_req *rqstp;
{
  if(pwd_ini == (char*) NULL){
    int i = getPwd(&pwd_ini);
    if(i!=0){
      static char *errormsg;
      errormsg = strerror(i);
      return(&errormsg);
    }
  }
  return(&pwd_ini);
}

/**
cd
retreives the server pwd from the argument and perform a change directory
*/
struct cd_res *rcd_1_svc(arg, rqstp)
struct cd_arg *arg;
struct svc_req *rqstp;
{
  chdir(arg->pwd);
  static char * current;
  getPwd(&current);
  static struct cd_res result;
  result.code = cd(arg->path, &current);
  getPwd(&current);
  result.pwd = current;
  printf("Current %s", current);
  return(&result);
}

/**
ls
retreives the server pwd from the client
send back the result of ls that has been concatenated
*/
char **rls_1_svc(pwd, rqstp)
char **pwd;
struct svc_req *rqstp;
{ 
  chdir(*pwd);
  static char *result;
  char *curr_dir;
  int i = getPwd(&curr_dir);
  if(!i){
    int j = getLs(curr_dir, &result, 0);
    if(j!=0){
      result = strerror(j);
    }
  } else {
    result = strerror(i);
  }
  return(&result);
}

/**
get
the argument contains several informations:
-filename : the file to get
-offset : where to begin the chunck in the file to get
Several calls are made by the client with an incremented offset
*/
struct file_part *rget_1_svc(desc, rqstp)
struct file_desc *desc;
struct svc_req *rqstp;
{  
  chdir(desc->pwd);
  static struct file_part fpart;

  char *curr_dir;
  int i = getPwd(&curr_dir);
  if(!i){
    char str[strlen(curr_dir) + strlen(desc->filename) + 1];
    concatCustom(str, curr_dir, desc->filename);

    //open the server file
    FILE* f = NULL;
    errno = 0;
    f = fopen(str, "rb");
    if(f != NULL){
      fseek(f, 0, SEEK_END);
      int size = ftell(f);
      rewind(f);

      //place the offset at the right place in the file
      fseek(f, desc->offset, SEEK_SET);

      //read the chunck
      static char part[PSIZE];
      int n = fread(part, sizeof(part[0]), sizeof(part)/sizeof(part[0]), f);
      fpart.chunck.chunck_val = part;
      fpart.chunck.chunck_len = PSIZE;
      if(ftell(f)==size){
        fpart.chunck.chunck_len = size%PSIZE;
        fpart.last = fpart.chunck.chunck_len;
      } else {
        fpart.last = 0;
      }
      fclose(f);
    } else {
      fpart.chunck.chunck_val = "Error: Failed to open file";
      fpart.chunck.chunck_len = strlen(fpart.chunck.chunck_val)+1;
      fpart.last = -fpart.chunck.chunck_len;
    }
  }
  return &fpart;
}

/**
put
Several data are found in the argument:
- filename : the file to put on the server
- chunck : a chunck of the file
- offset : where the chunck comes in the file
If the offset is 0, the file is created.
If not, the file is opened in append mode.
*/
int *rput_1_svc(fput, rqstp)
struct file_put *fput;
struct svc_req *rqstp;
{  
  chdir(fput->pwd);
  static int result = 0;
  char *curr_dir;
  int i = getPwd(&curr_dir);
  if(!i){
    char str[strlen(curr_dir) + strlen(fput->filename) + 1];
    concatCustom(str, curr_dir, fput->filename);

    //create/open the file with the right mode
    FILE* f = NULL;
    if(fput->offset == 0){
      f = fopen(str, "wb+");
    } else {
      f = fopen(str, "ab");
    }

    //writes in the server file
    if(fput->offset == ftell(f)){
      int w = fwrite(fput->chunck.chunck_val, 1, fput->chunck.chunck_len, f);
      if(w != fput->chunck.chunck_len){
        printf("Error\n");
        result = 1;
      }
    } else {
      result = 2;
    }
    fclose(f);
  }
  return &result;
}
