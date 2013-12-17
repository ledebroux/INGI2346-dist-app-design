/* node.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pvm3.h"

int send(int* array, char* label, int tid, int size)
{
    //inform the creator that we know the reachables
  char msg[100];
  strcpy(msg,label);
  char str[2];
  int i;
  for(i=0; i<size; i++){
    sprintf(str, " %d", array[i]);

    strcat(msg,str);
  }

  pvm_initsend(PvmDataDefault); /*initialize send buffer*/
  pvm_pkstr(msg); /*XDR encode string into send buffer*/
  pvm_send(tid,1);
}


int main()
{
  int ptid;

  ptid=pvm_parent(); /*get tid of task that started me*/

  int cc;
  int reachables[1];
  cc = pvm_recv(-1,-1); /*wait for a message*/
  pvm_upkint(reachables, 1, 1); /*extract string from receive buffer into greetings*/

  //inform the creator that we know the reachables
  //send(reachables, "My reachables: ", ptid, 1);

  int children[reachables[0]];
  cc = pvm_recv(-1,-1);
  pvm_upkint(children, reachables[0], 1);

  send(children, "My children", ptid, reachables[0]);

  exit(0);
}