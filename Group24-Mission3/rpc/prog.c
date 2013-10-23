/*    prog.c    */

#include <rpc/rpc.h>
#include "rpspec.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


main(argc,argv)
int argc;
char *argv[ ];
{
  struct arga data, *pdata;
  double *result;
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
  data.arga1 = 5;
  data.arga2 = 3;
  result = rproca_1(&data, cl);
  if (result == (double*) NULL){
    clnt_perror(cl,argv[1]);
    exit(1);
  }
  printf("the result of RPROCA is %f\n", *result);

  pdata = rprocb_1((void *)NULL, cl);
  if  (pdata == (arga*) NULL){
    clnt_perror(cl,argv[1]);
    exit(1);
  }
  printf("the arga1 field is %d\n", pdata->arga1);
  printf("the arga2 field is %d\n", pdata->arga2);
  clnt_destroy(cl);
}
