/* election.c */
#include <stdio.h>
#include "pvm3.h"

/*
 * Receives a string sent by a task
 */
int receive(){
  char msg[100];
  int cc, tid;
  cc = pvm_recv(-1,-1);
  pvm_bufinfo(cc,(int *)0,(int *)0,&tid);
  pvm_upkstr(msg);
  printf("from t%i: %s\n",tid,msg);
  return 0;
}

// int index(int x, int y, int col){
//   int index = x*col+y;
//   return index;
// }

/* 
 * Prints an array of int
 */
void print_array(int* array, int size)
{
  int i;
  for(i=0; i<size; i++){
    printf("%i  ", array[i]);
  }
  printf("\n");
}

/* 
 * Prints a matrix of int
 */
void print_matrix(int row, int col, int matrix[row][col])
{
  int i, j;
  for (i=0;i<row;i++){
    for(j=0;j<col;j++){
      printf("%d ",matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

/*
 * Computes the diameter of the graph
 * Using the Floyd-Warshall algorithm to get
 *    the shortest path between all nodes
 */
int compute_diameter(int vertices, int row, int edges[row][2]){
  /* Begining of Floyd-Warshall algorithm */
  int dist[vertices][vertices];
  int i, j;
  for (i=0; i<vertices; i++){
    for(j=0; j<vertices; j++){
      dist[i][j]=vertices;
    }
  }
  for(i=0; i<vertices; i++){
    dist[i][i]=0;
  }
  for(j=0; j<row; j++){
    int u = edges[j][0];
    int v = edges[j][1];
    dist[u-1][v-1] = 1;
    printf("u: %i and v: %i\n", u, v);
  }

  int k;
  for(k=0; k<vertices; k++){
    for (i=0; i<vertices; i++){
      for (j=0; j<vertices; j++){
        if(dist[i][j] > dist[i][k] + dist[k][j]){
          dist[i][j] = dist[i][k] + dist[k][j];
        }
      }
    }
  }
  /* End of Floyd-Warshall algorithm */

  print_matrix(vertices, vertices, dist);
 
  /* Computation of the diameter */
  int diameter = 0;
  for (i=0;i<vertices;i++){
    for(j=0;j<vertices;j++){
      if(dist[i][j]>diameter){
        diameter = dist[i][j];
      }
    }
  }
  // printf("Diameter: %i\n", diameter);
  return diameter;
}

int main()
{
  /*
   * Graph initialization : TODO (in a .txt file, and pass it in argument)
   */
  unsigned int n = 5;
  unsigned int nbEdge = 6;
  int e[][2] = {{1,2}, {1,4}, {2,3}, {3,1}, {4,5}, {5,1}};

  /*
  unsigned int n = 6;
  unsigned int nbEdge = 8;
  int e[][2] = {{1,2}, {1,4}, {2,3}, {3,1}, 
                {4,5}, {5,1}, {3,6}, {6,4}};
  */

  /*
   * VAR INIT
   */

  unsigned int i, j;

  int diameter = compute_diameter(n, nbEdge, e);

  int adjMatrix[n][n]; // Adjacency matrix for the graph
  for (i=0;i<n;i++){ // Initialization of all elem to 0
    for(j=0;j<n;j++){
      adjMatrix[i][j]=0;
    }
  }

  int cc;
  int tid[n-1]; /* Array containing the tid of each node */

  int outgoing[n-1]; /* Array containing the number of outgoing edges for each node */
  int ingoing[n-1]; /* Array containing the number of ingoing edges for each node */
  for(i = 1; i <= n; i++){ 
    outgoing[i-1]=0;
    ingoing[i-1]=0;
  }

  /*
   * Computation of AdjMatrix, outgoing and ingoing
   * outgoing: array where outgoing[i] is 
   *    the number of outgoing edges from i+1
   * ingoing: array where ingoing[i] is 
   *    the number of ingoing edges to i+1
   */
  for(i = 1; i <= n; i++){
    for(j=0; j<nbEdge; j++){
      if(e[j][0] == i){
        adjMatrix[i-1][e[j][1]-1] = 1;
        outgoing[i-1]++;
        ingoing[e[j][1]-1]++;
      }
    }
  }

  /*
   * Transpose the AdjMatrix (to be used to advertise the ingoing edges to a task)
   */
  int TadjMatrix[n][n];
  for(i=0; i<n; i++){
    for(j=0; j<n; j++){
      TadjMatrix[j][i]=adjMatrix[i][j];
    }
  }

  /*
   * Creates one pvm_task for each node in the graph
   * Send a array of int containg several initiliziation information to the task.
   * This information are : the Id of the node, the number of ingoing edges,
   * the number of outgoing edges, and the diameter.
   */
  for(i = 1; i <= n; i++){
    cc = pvm_spawn("node",(char **)0,0,"",1,&tid[i-1]);
    printf("Node %i spawned\n", i);
    if(cc == 1){
      int initData[4] = {i, ingoing[i-1], outgoing[i-1], diameter};
      //print_array(initData, 4);
      pvm_initsend(PvmDataDefault);
      pvm_pkint(initData, 4, 1);
      pvm_send(tid[i-1],1);
      receive(); /* if uncommented, a send() needs to be present at the right place in node.c */
    }

  }

  /* At this point of the execution, all tids are known */

  // print_matrix(n, n, adjMatrix);
  // print_matrix(n, n, TadjMatrix);

  print_array(tid, n);
  // print_array(outgoing, n);
  // print_array(ingoing, n);

  /*
   * For each node, we build 2 array of int containing the tids of the parents
   * (via ingoing edges) and the tids of the children (via outgoing edges).
   * We advertise those arrays to each node.
   */

  for(i=1; i<=n; i++){
    int children[outgoing[i-1]];
    int parents[ingoing[i-1]];
    int offset_children = 0;
    int offset_parents = 0;
    for(j=0; j<n; j++){
      if(adjMatrix[i-1][j]==1){
        //printf("%i has child %i\n", i, tid[j]);
        children[offset_children] = tid[j];
        offset_children++;
      }
      if(TadjMatrix[i-1][j]==1){
        //printf("%i is reachable by %i\n", i, tid[j]);
        parents[offset_parents] = tid[j];
        offset_parents++;
      }
      // TODO set the value for parent wrt. TadjMatrix
    }
    pvm_initsend(PvmDataDefault);
    //print_array(children, outgoing[i-1]);
    //print_array(parents, ingoing[i-1]);
    pvm_pkint(children, outgoing[i-1], 1);
    pvm_pkint(parents, ingoing[i-1], 1);
    pvm_send(tid[i-1],1);

    receive(); /* if uncommented, a send() needs to be present at the right place in node.c */
    receive();
    //receive();
  }

  /*
   * We send a message "start" to all nodes to advertise the tasks they can begin
   * the election protocol. Without this message, all nodes begin the election
   * protocol while the others have not yet received the information about 
   * children and parents tids.
   */

  for(i=1; i<=n; i++){
    pvm_initsend(PvmDataDefault);
    pvm_pkstr("Start");
    pvm_send(tid[i-1],1);
  }
  // printf("size of e %lu\n", sizeof(e)/sizeof(e[0]));

  /*
   * Finally, we wait for the max_id of each node, to verify the result of the election protocol.
   */ 
  for(i=1; i<=n; i++){
    receive();
  }

  printf("End of main()\n");
  return 0;
}

