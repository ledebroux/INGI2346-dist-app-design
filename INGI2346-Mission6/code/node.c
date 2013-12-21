/* node.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "pvm3.h"

/*
 * Sends a array of int with message label to a task
 */
int send_array(int* array, char* label, int tid, int size)
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
 * Sends a integer with a message label to a task
 */
int send_int(int i, char* label, int tid)
{
  char msg[100];
  strcpy(msg,label);
  char str[2];
  sprintf(str, " %d", i);
  strcat(msg,str);
  pvm_initsend(PvmDataDefault);
  pvm_pkstr(msg);
  pvm_send(tid,1);
  return 0;
}

/*
 * Sends an integer to a task
 */
int sendId(int tid, int max_id)
{
  pvm_initsend(PvmDataDefault);
  pvm_pkint(&max_id, 1, 1);
  pvm_send(tid,1);
  return 0;
}

/*
 * Sends an id to a specific task, with point-to-point messages
 */
int unicastSendId(int* tid, int ntasks, int max_id)
{
  int i;
  for(i=0; i<ntasks; i++){
    sendId(tid[i], max_id);
  }
  return 0;
}

/*
 * Sends an id to a group of tasks, with multicast messages.
 */
int multicastSendId(int* tid, int ntasks, int max_id)
{
  pvm_initsend(PvmDataDefault);
  pvm_pkint(&max_id, 1, 1);
  pvm_mcast(tid, ntasks, 1);
  return 0;
}

/*
 * Sends the max_id in a broadcast mode.
 */
int broadcastSendId(int max_id, int ptid){
  int mtid = pvm_mytid();
  char grp[100];
  strcpy(grp,"grp");
  char nb[10];
  sprintf(nb, " %d", mtid);
  strcat(grp,nb);
  pvm_initsend(PvmDataDefault);
  pvm_pkint(&max_id, 1, 1);
  pvm_bcast(grp,1);
  return 0;
}

/*
 * Receives an id sent by a task
 */
int receiveId(int tid)
{
  int cc;
  int recvId[1];
  cc = pvm_recv(tid, -1);
  pvm_upkint(recvId, 1, 1);
  return recvId[0];
}

/*
 * Algorithm of election : we first send our max_id to all our children
 * and then we wait for messages from our parents.
 * The number of messages we have to receive corresponds to the number of
 * parents, but we have to specify from which tid we want to receive a message.
 * It avoids receiving 2 messages from the same tid in the same "round".
 * Note that you can choose the mode of sending between unicast, multicast or
 * broadcast.
 */
int election(int in, int* parents, int out, int* children, int* max_id, int sendingType, int ptid)
{
  /* One of the two lines below has to be commented, 
   * and the other, uncommented to use multicast or unicast */
  if(sendingType == 1){
    unicastSendId(children, out, *max_id);
  }
  else if(sendingType == 2){
    multicastSendId(children, out, *max_id);
  }
  else if(sendingType == 3){
    broadcastSendId(*max_id, ptid);
  }
  int i;
  for(i=0; i<in; i++){
    int id;
    id = receiveId(parents[i]);
    if(id > *max_id){
      *max_id = id;
    }
  }
  return 0;
}


int main()
{
  int ptid;
  int max_id;
  int ingoing;
  int outgoing;
  int diameter;
  int sendingType;
  int n;
  int initData[3];

  ptid = pvm_parent(); /*get tid of task that started me*/

  /* 
   * The node received the initiliziation information from the main taks
   * with its id, the number of ingoing edges (parents), the number of
   * outgoing edges (childrens) and the diameter of the graph.
   */
  int cc;
  cc = pvm_recv(-1,-1);
  pvm_upkint(initData, 6, 1);

  max_id = initData[0];
  ingoing = initData[1];
  outgoing = initData[2];
  diameter = initData[3];
  sendingType = initData[4];
  n = initData[5];

  /* The node receives 2 arrays of integer with the list of tids 
   * of its parents (ingoing edges) and its childrens (outgoing edges)
   */
  int children[outgoing];
  int parents[ingoing];
  cc = pvm_recv(-1,-1);
  pvm_upkint(children, outgoing, 1);
  pvm_upkint(parents, ingoing, 1);

  int i;

  if(sendingType == 3){
  /* 
   * When a node wants to send, with broadcast, it is to all of its children.
   * the broadcast group must be joined by all of its children.
   * Thus, one node have to go to the groups of its parents
   */
    for(i=0; i<ingoing; i++){
      char grp[100];
      strcpy(grp,"grp");
      char nb[10];
      sprintf(nb, " %d", parents[i]);
      strcat(grp,nb);
      pvm_joingroup(grp);
    }
  /* This group is useful to synchronize the election rounds when using bcast
   * It could be used for the other modes too although not needed.
   * Due to our receive function, it is not strictly needed, even in broadcast
   */
    pvm_joingroup("NodesGroup");
  }

  /*
   * The following lines could be done using a barrier on a group
   * containing all the nodes, instead of telling the server that
   * the creation is done and wait for a start message.
   */
  send_int(max_id, "Created", ptid);

  /*
   * Waits for the message "start" sent by the main task, and which allows the node
   * to start the election protocol. The message start sent to each node allows
   * a sort of synchronization between the different nodes.
   */
  char start[10];
  cc = pvm_recv(-1,-1);
  pvm_upkstr(start);

  /* Iteration of the algorithm of "diamater" times
   * Synchronise between the different nodes at every round.
   */
  for(i=0; i<diameter; i++){
    election(ingoing, parents, outgoing, children, &max_id, sendingType, ptid);
    if(sendingType == 3){
      /* Not really needed due to the implem of our receive function */
      pvm_barrier("NodesGroup", n);
    }
  }

  /* The node send its max_id to the main task when the election protocol is over */
  send_int(max_id, "Max Id", ptid);
  
  pvm_exit();
  exit(0);
}