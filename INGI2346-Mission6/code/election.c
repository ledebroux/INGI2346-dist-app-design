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

/*
 * Computes the diameter of the graph
 * Using the Floyd-Warshall algorithm to get
 *    the shortest path between all nodes
 */
int compute_diameter(int vertices, unsigned int edges[][2]){
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
  printf("size %lu\n", sizeof(edges)/sizeof(edges[0]));
  for(j=0; j<(int)(sizeof(edges)/sizeof(edges[0])); j++){
    int u = edges[j][0];
    int v = edges[j][1];
    dist[u][v] = 1;
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

  for (i=0;i<vertices;i++){
    for(j=0;j<vertices;j++){
      printf("%d ",dist[i][j]);
    }
    printf("\n");
  }
 
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

// void print_matrix(int** matrix, int row, int col)
// {
//   int i, j;
//   for (i=0;i<row;i++){
//     for(j=0;j<col;j++){
//       printf("%d ",matrix[i][j]);
//     }
//     printf("\n");
//   }
// }

int main()
{
  unsigned int n = 5;
  unsigned int e[][2] = {{1,2}, {1,4}, {2,3}, {3,1}, {4,5}, {5,1}};

  /*
   * VAR INIT
   */

  unsigned int i, j;

  int diameter = compute_diameter(n, e);

  int adjMatrix[n-1][n-1];
  for (i=0;i<n;i++){
    for(j=0;j<n;j++){
      adjMatrix[i][j]=0;
    }
  }

  int cc;
  int tid[n-1]; /* Array containing the tid of each node */

  int outgoing[n-1];
  int ingoing[n-1];
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
    for(j=0; j<sizeof(e)/sizeof(e[0]); j++){
      if(e[j][0] == i){
        adjMatrix[i-1][e[j][1]-1] = 1;
        outgoing[i-1]++;
        ingoing[e[j][1]-1]++;
      }
    }
  }

  /*
   * Creates one pvm_task for each node in the graph
   * and gives them as arg:
   *    - outgoing
   *    - ingoing
   *    - id
   */
  for(i = 1; i <= n; i++){
    cc = pvm_spawn("node",(char **)0,0,"",1,&tid[i-1]);
    if(cc == 1){
      pvm_initsend(PvmDataDefault);
      pvm_pkint(&outgoing[i-1], 1, 1);
      pvm_send(tid[i-1],1);
      //receive(); /* if uncommented, a send() needs to be present at the right place in node.c */
    }

  }

  /* At this point of the execution, all tids are known */

  //print_matrix(adjMatrix, n, n);

  for (i=0;i<n;i++){
    for(j=0;j<n;j++){
      printf("%d ",adjMatrix[i][j]);
    }
    printf("\n");
  }

  print_array(tid, n);
  print_array(outgoing, n);
  print_array(ingoing, n);

  for(i=1; i<=n; i++){
    int children[outgoing[i-1]];
    int offset = 0;
    for(j=0; j<n; j++){
      if(adjMatrix[i-1][j]==1){
        //printf("%i has child %i\n", i, tid[j]);
        children[offset] = tid[j];
        offset++;
      }
    }
    pvm_initsend(PvmDataDefault);
    pvm_pkint(children, outgoing[i-1], 1);
    pvm_send(tid[i-1],1);

    receive(); /* if uncommented, a send() needs to be present at the right place in node.c */
  }
  printf("size of e %lu\n", sizeof(e)/sizeof(e[0]));

  printf("End of main()\n");
  return 0;
}

