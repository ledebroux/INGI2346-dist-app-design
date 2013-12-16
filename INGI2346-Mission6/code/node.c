/* node.c */
#include <stdlib.h>
#include "pvm3.h"
int main()
{
  int ptid;
  //char reachable = *argv[0];
  //char greetings[100];

  ptid=pvm_parent(); /*get tid of task that started me*/
  //strcpy(greetings,"Hello world from ");
  // gethostname(greetings + strlen(greetings));

  // pvm_initsend(PvmDataDefault); /*initialize send buffer*/
  // pvm_pkstr(greetings); /*XDR encode string into send buffer*/
  // pvm_send(ptid,1);

  // printf("reachable %i\n", reachable);
  // printf("ptid %i\n", ptid);
  // pvm_exit(); /*disconnect from the PVM*/

  int cc;
  int *reachables;
  cc = pvm_recv(-1,-1); /*wait for a message*/
  // pvm_bufinfo(cc,(int *)0,(int *)0,&tid); /*get tid of sender*/
  pvm_upkint(reachables, 1, 1); /*extract string from receive buffer into greetings*/
  
  // int reach[reachables[0]];
  // cc = pvm_recv(-1, -1);
  // pvm_upkint(reach, 1);

  // char status[20];
  // cc = pvm_recv(-1,-1);
  // pvm_upkstr(status);

  exit(0);
}