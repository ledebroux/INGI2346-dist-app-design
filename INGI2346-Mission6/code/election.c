/* election.c */
#include <stdio.h>
#include <string.h>
#include "pvm3.h"


/*
 * Replace a character by another in a string
 *
 * The code comes from 
 * http://stackoverflow.com/questions/16573206/c-replace-char-in-char-array
 */
 
char *replace_char (char *str, char find, char *replace) {
    char *ret=str;
    char *wk, *s;

    wk = s = strdup(str);

    while (*s != 0) {
        if (*s == find){
            while(*replace)
                *str++ = *replace++;
            ++s;
        } else
            *str++ = *s++;
    }
    *str = '\0';
    free(wk);
    return ret;
}


/*
 * Parse the file to initialise the variables n, nbEdges, edge
 */
int parseFile(char* fileName, int* n, int* nbEdges, int edge[200][2]){
  FILE* file = NULL;
  file = fopen(fileName,"r");
  char descr[4096] = "";

  if (file != NULL){
    fscanf(file, "%d", n);
    fseek(file, +1, SEEK_CUR);
    fscanf(file, "%d", nbEdges);
    fseek(file, +1, SEEK_CUR);
    fscanf(file, "%s", descr);
    fclose(file);

    char *temp1 = replace_char(descr,'{',"");
    char *temp2 = replace_char(temp1,'}',"");
    char *graph = replace_char(temp2,',',"");

    int i;  
    for(i = 0; i<strlen(graph); i++){
      if(i%2 == 0){
        edge[i/2][0] = descr[i] - '0';
      } else {
        edge[i/2][1] = descr[i] - '0';
      } 
    }
  } else {
    printf("The input file was not found\n");
    exit(0);
  }
  return 0;
 }


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
    // printf("u: %i and v: %i\n", u, v);
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

  // print_matrix(vertices, vertices, dist);
 
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

int main(int argc, char *argv[])
{
  /*
   * Graph initialization : Parse the file given in argument.
   * The file must contains on the first line the number of nodes,
   * the number of edges on the second line,
   * and a description of the edges on the third line
   */
  unsigned int n = 0;
  unsigned int nbEdge = 0;
  int e[200][2];
  if(argc == 2){
    int result = parseFile(argv[1], &n, &nbEdge, e);
  } else {
    printf("No graph file provided");
    return -1;
  }
  
  /*
  Choose the type of communication you want to use:
  1 : unicast
  2 : multicast
  3 : broadcast
  */
  int SENDINGTYPE = 1;

  /*
  unsigned int n = 4;
  unsigned int nbEdge = 4;
  int e[][2] = {{1,2}, {2,3}, {3,4}, {4,1}};
  */

  /*
  unsigned int n = 5;
  unsigned int nbEdge = 5;
  int e[][2] = {{1,2}, {2,3}, {3,4}, {4,5}, {5,1}};
  */

  /*
  unsigned int n = 5;
  unsigned int nbEdge = 6;
  int e[][2] = {{1,2}, {1,4}, {2,3}, {3,1}, {4,5}, {5,1}};
  */

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
   * the number of outgoing edges, the diameter, the type for sending (point-to-point (1), multicast (2) or broadcast (3))
   */
  for(i = 1; i <= n; i++){
    cc = pvm_spawn("node",(char **)0,0,"",1,&tid[i-1]);
    printf("Node %i spawned\n", i);
    if(cc == 1){
      int initSize = 6;
      int initData[6] = {i, ingoing[i-1], outgoing[i-1], 
          diameter, SENDINGTYPE, n};
      //print_array(initData, initSize);
      pvm_initsend(PvmDataDefault);
      pvm_pkint(initData, initSize, 1);
      pvm_send(tid[i-1],1);
      //receive(); /* if uncommented, a send() needs to be present at the right place in node.c */
    }

  }

  /* At this point of the execution, all tids are known */

  // print_matrix(n, n, adjMatrix);
  // print_matrix(n, n, TadjMatrix);
  // print_array(tid, n);
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
        children[offset_children] = tid[j];
        offset_children++;
      }
      if(TadjMatrix[i-1][j]==1){
        parents[offset_parents] = tid[j];
        offset_parents++;
      }
    }
    pvm_initsend(PvmDataDefault);
    //print_array(children, outgoing[i-1]);
    //print_array(parents, ingoing[i-1]);
    pvm_pkint(children, outgoing[i-1], 1);
    pvm_pkint(parents, ingoing[i-1], 1);
    pvm_send(tid[i-1],1);

    //receive(); /* if uncommented, a send() needs to be present at the right place in node.c */
    //receive();

    receive();
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

  /*
   * Finally, we wait for the max_id of each node, to verify the result of the election protocol.
   */ 
  for(i=1; i<=n; i++){
    receive();
  }

  printf("End of election process\n");

  pvm_exit();
  return 0;
}

