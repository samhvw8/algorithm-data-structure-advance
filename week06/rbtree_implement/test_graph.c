#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include <string.h>

int main(){
     int i, n ,output[100];
     graph g = create_graph();
     add_edge(g, 0, 1);
     add_edge(g, 0, 2);
     add_edge(g, 1, 2);
     add_edge(g, 1, 3);
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


     printf("\n");
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
