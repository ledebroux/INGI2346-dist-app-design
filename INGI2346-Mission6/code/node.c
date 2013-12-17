/* node.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pvm3.h"

/*
 * Sends a string to a task
 */
int send(int* array, char* label, int tid, int size)
{
  char msg[100];
  strcpy(msg,label);
  char str[2];
  int i;
  /* Turn array into a string to be sent */
  for(i=0; i<size; i++){
    sprintf(str, " %d", array[i]);
    strcat(msg,str);
  }

  pvm_initsend(PvmDataDefault);
  pvm_pkstr(msg);
  pvm_send(tid,1);
  return 0;
}

/*
 * Sends an id to a task
 */
int sendId(int tid, int max_id)
{
  pvm_initsend(PvmDataDefault);
  pvm_pkint(&max_id, 1, 1);
  pvm_send(tid,1);
  return 0;
}

/*
 * Receives an id sent by a task
 */
int receiveId()
{
  int cc;
  int recvId[1];
  cc = pvm_recv(-1,-1);
  pvm_upkint(recvId, 1, 1);
  return recvId[0];
}

/*
 * Algorithm of election
 */
int election(int in, int out, int* children, int* max_id)
{
  int i;
  for(i=0; i<out; i++){
    sendId(children[i], *max_id);
  }
  for(i=0; i<in; i++){
    int id = receiveId();
    if(id > *max_id){
      *max_id = id;
    }
  }
  return 0;
}

int main()
{
  int ptid;
  int max_id; //TODO: needs to be set upon creation
  int diameter; //TODO: needs to be set upon creation
  int outgoing[1];
  int ingoing[1];

  ptid=pvm_parent(); /*get tid of task that started me*/

  /* the node receives the number of nodes it can reach */
  int cc;
  cc = pvm_recv(-1,-1);
  pvm_upkint(outgoing, 1, 1);

  /* inform the creator that we know the outgoing */
  //send(outgoing, "My outgoing: ", ptid, 1); /* if uncommented, a receive() needs to be present at the right place in node.c */

  /* the node receives the tid of the nodes it can reach */
  int children[outgoing[0]];
  cc = pvm_recv(-1,-1);
  pvm_upkint(children, outgoing[0], 1);

  /* inform the creator of the tid of the nodes it can reach */
  send(children, "My children", ptid, outgoing[0]); /* if uncommented, a receive() needs to be present at the right place in node.c */

  int i;
  for(i=0; i<diameter; i++){
    election(ingoing[0], outgoing[0], children, &max_id);
  }

  exit(0);
}