#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include <string.h>

void print_node(graph, int);

int main() {
     int i, n , output[100];
     graph g = create_graph();

     add_vertex(g, 0, "0");
     add_vertex(g, 1, "1");
     add_vertex(g, 2, "2");
     add_vertex(g, 3, "3");
     add_vertex(g, 4, "4");
     add_vertex(g, 5, "5");
     add_vertex(g, 6, "6");
     add_vertex(g, 7, "7");
     add_vertex(g, 8, "8");


     add_edge(g, 0, 1, 4);
     add_edge(g, 0, 7, 8);
     add_edge(g, 1, 2, 8);
     add_edge(g, 1, 7, 11);
     add_edge(g, 2, 3, 7);
     add_edge(g, 2, 8, 2);
     add_edge(g, 2, 5, 4);
     add_edge(g, 3, 4, 9);
     add_edge(g, 3, 5, 14);
     add_edge(g, 4, 5, 10);
     add_edge(g, 5, 6, 2);
     add_edge(g, 6, 7, 1);
     add_edge(g, 6, 8, 6);
     add_edge(g, 7, 8, 7);

#define PRINT_ADJACENT_VERTIES(v) {                                 \
          n = out_degree(g, v, output);                              \
          if(n == 0)                                                \
               printf("No adjacent vertices of node" #v " \n");     \
          else{                                                     \
               printf("Adjacent vertices of node "#v": ");          \
               for(i = 0; i < n; i++)                               \
                    printf("%5d", output[i]);                       \
               printf("\n");                                        \
          }                                                         \
     }

     // PRINT_ADJACENT_VERTIES(0);
     // PRINT_ADJACENT_VERTIES(1);
     // PRINT_ADJACENT_VERTIES(2);
     // PRINT_ADJACENT_VERTIES(3);
     // PRINT_ADJACENT_VERTIES(4);
     // PRINT_ADJACENT_VERTIES(5);
#undef PRINT_ADJACENT_VERTIES

     int path[100];
     double len[100];
     double out_val = shortest_path(g, 0, 4, path, len);
     if(out_val == INFINITY) {
          printf("Not found path !!\n");
     } else {
          printf("%.2f !!!\n", out_val);
          for (i = 0; i < 9; ++i)
          {
               printf("%d  %.2f    %d\n", i, len[i], path[i]);
          }

     }


     // printf("\n\tTest traversal BFS: ");

     // BFS(g, 2, -1, print_node);

     // printf("\n\tTest traversal DFS: ");

     // DFS(g, 2, -1, print_node);
     // printf("%s\n", "Check TSort");
     // TSort(g, print_node);
     // printf("\n");
     // printf("%s\n", "Check cycle");
     // if(is_cyclic(g)){
     //      printf("%s\n", "graph has cycle");
     // } else {
     //      printf("%s\n", "graph has no cycle");
     // }

     // printf("\n\nTest drop !\n");

     drop_graph(&g);


     return 0;
}


void print_node(graph g, int i) {
     printf("\t\t%d", i);
}
