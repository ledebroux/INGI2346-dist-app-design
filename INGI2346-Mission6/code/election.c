/* election.c */
#include <stdio.h>
#include "pvm3.h"
int main()
{
  unsigned int n = 5;
  int e[][2] = {{1,2}, {1,4}, {2,3}, {3,3}, {4,5}, {5,1}};

  unsigned int i, j;
  int adjMatrix[n-1][n-1];

  for (i=0;i<n;i++)
  {
    for(j=0;j<n;j++)
    {
      adjMatrix[i][j]=0;
      //printf("%d ",adjMatrix[i][j]);
    }
    //printf("\n");
  }

  int reachable = 0;

  int cc;
  int tid[n-1];

  for(i = 1; i <= n; i++)
  {
    // loop to identify reachable nodes
    for(j=0; j<sizeof(e)/sizeof(e[0]); j++){
      if(e[j][0] == i){
        adjMatrix[i][e[j][1]] = 1;
        reachable++;
      }
    }
    cc = pvm_spawn("node",(char **)0,0,"",1,&tid[i]);
    if(cc == 1){
      pvm_initsend(PvmDataDefault);
      pvm_pkint(&reachable, 1, 1);
      pvm_send(tid[i],1);
      
      // pvm_initsend(PvmDataDefault);
      // pvm_pkint(&reachable, 1);
      // pvm_send(tid[i],1);
    }


    // char * arg = (char*)&offset;

    //   // cc = pvm_recv(-1,-1); /*wait for a message*/
    //   // pvm_bufinfo(cc,(int *)0,(int *)0,&tid); /*get tid of sender*/
    //   // pvm_upkstr(greetings); /*extract string from receive buffer 
    //   //                                into greetings*/
    //   // printf("from t%x: %s\n",tid,greetings);
    // }else printf("Can't start task node\n");
    //spawn a node with its reachable neighbors

  }
  //All tids are known.



  // int cc, tid;
  // char greetings[100];

  // printf("I'm t %x\n",pvm_mytid());
  // /* create the second task, executing program "hi" */
  // cc = pvm_spawn("hi",(char **)0,0,"",1,&tid);
  // if(cc == 1){
  //         cc = pvm_recv(-1,-1); /*wait for a message*/
  //         pvm_bufinfo(cc,(int *)0,(int *)0,&tid); /*get tid of sender*/
  //         pvm_upkstr(greetings); /*extract string from receive buffer 
  //                                        into greetings*/
  //         printf("from t%x: %s\n",tid,greetings);
  // }else printf("Can't start task hi\n");
  // pvm_exit(); /*disconnect from the PVM*/
  // exit(0);
  printf("lalalal\n");
  return 0;
}