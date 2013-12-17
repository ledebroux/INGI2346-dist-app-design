/* election.c */
#include <stdio.h>
#include "pvm3.h"

int receive(){
  char msg[100];
  int cc, tid;
  cc = pvm_recv(-1,-1); /*wait for a message*/
  pvm_bufinfo(cc,(int *)0,(int *)0,&tid); /*get tid of sender*/
  pvm_upkstr(msg); /*extract string from receive buffer 
                                 into greetings*/
  printf("from t%i: %s\n",tid,msg);
  return 0;
}

int main(){
  unsigned int n = 5;
  unsigned int e[][2] = {{1,2}, {1,4}, {2,3}, {3,1}, {4,5}, {5,1}};

  unsigned int i, j;
  int adjMatrix[n-1][n-1];

  for (i=0;i<n;i++){
    for(j=0;j<n;j++){
      adjMatrix[i][j]=0;
      //printf("%d ",adjMatrix[i][j]);
    }
    //printf("\n");
  }

  int cc;
  int tid[n-1];
  int reachable[n-1];

  for(i = 1; i <= n; i++){
    reachable[i-1]=0;
    // loop to identify reachable nodes
    for(j=0; j<sizeof(e)/sizeof(e[0]); j++){
      if(e[j][0] == i){
        //printf("neighbor: %i\n", e[j][1]);
        adjMatrix[i-1][e[j][1]-1] = 1;
        reachable[i-1]++;
      }
    }
    cc = pvm_spawn("node",(char **)0,0,"",1,&tid[i-1]);
    if(cc == 1){
      //printf("%i Reachable from %i\n", reachable[i-1], i);
      pvm_initsend(PvmDataDefault);
      pvm_pkint(&reachable[i-1], 1, 1);
      pvm_send(tid[i-1],1);

      //receive();
    }

  }
  //All tids are known.

  // for (i=0;i<n;i++){
  //   for(j=0;j<n;j++){
  //     printf("%d ",adjMatrix[i][j]);
  //   }
  //   printf("\n");
  // }

  for(i=0; i<n; i++){
    printf("%i  ", tid[i]);
  }
  printf("\n");
  for(i=0; i<n; i++){
    printf("%i  ", reachable[i]);
  }
  printf("\n");

  for(i=1; i<=n; i++){
    int children[reachable[i-1]];
    int offset = 0;
    for(j=0; j<n; j++){
      if(adjMatrix[i-1][j]==1){
        //printf("%i has child %i\n", i, tid[j]);
        children[offset] = tid[j];
        offset++;
      }
    }
    pvm_initsend(PvmDataDefault);
    pvm_pkint(children, reachable[i-1], 1);
    pvm_send(tid[i-1],1);

    receive();
  }

  printf("End of main()\n");
  return 0;
}

