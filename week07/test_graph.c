#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include <string.h>

void print_node(int i);

int main(){
     int i, n ,output[100];
     graph g = create_graph();
 
     add_edge(g, 0, 1);
     add_edge(g, 0, 2);
     add_edge(g, 1, 2);
     add_edge(g, 2, 0);
     add_edge(g, 2, 3);
     add_edge(g, 3, 3);
 

#define PRINT_ADJACENT_VERTIES(v) {                                 \
          n = get_adjacent_vertices(g, v, output);                  \
          if(n == 0)                                                \
               printf("No adjacent vertices of node" #v " \n");     \
          else{                                                     \
               printf("Adjacent vertices of node "#v": ");          \
               for(i = 0; i < n; i++)                               \
                    printf("%5d", output[i]);                       \
               printf("\n");                                        \
          }                                                         \
     }                                                              

     PRINT_ADJACENT_VERTIES(0);
     PRINT_ADJACENT_VERTIES(1);
     PRINT_ADJACENT_VERTIES(2);
     PRINT_ADJACENT_VERTIES(3);
#undef PRINT_ADJACENT_VERTIES

     printf("\n\tTest traversal BFS: ");

     BFS(g, 2, -1, print_node);

     printf("\n\tTest traversal DFS: ");

     DFS(g, 2, -1, print_node);
     
     
     printf("\n\nTest drop !\n");

     drop_graph(&g);

     n = get_adjacent_vertices(g, 1, output);
 
     if(n == 0)
          printf("No adjacent vertices of node 1 \n");
     else{
          printf("Adjacent vertices of node 1: ");
          for(i = 0; i < n; i++)
               printf("%5d", output[i]);
     }
     printf("\n");
     
     
     return 0;
}


void print_node(int i){
     printf("\t\t%d", i);
}
