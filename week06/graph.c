#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"


graph create_graph(int size_max){
     graph ret;
     
     ret.size_max = size_max;
     ret.matrix = malloc(sizeof(int *) * (size_max));
     if(ret.matrix == NULL) {
          fprintf(stderr, "ERROR: NULL value in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }
     
     int index;
     for (index = 0; index < size_max; index++){
          (ret.matrix)[index] = malloc(sizeof(int) * size_max);
          if((ret.matrix)[index] == NULL) {
               fprintf(stderr, "ERROR: NULL value in %s:%d \n", __FILE__, __LINE__);
               exit(1);
          }
          
          memset(((ret.matrix)[index]), 0, size_max + 1);
     }
     
     return ret;
}

void add_edge(graph g, int v, int v2){
     if(v > g.size_max || v2 > g.size_max) {
          fprintf(stderr, "ERROR: value v or v2 over size_max");
          return;
     }
     (g.matrix)[v][v2] = 1;
     (g.matrix)[v2][v] = 1;
}

int adjacent(graph g, int v, int v2){
     if (g.matrix == NULL) 
          return 0;
     if(((g.matrix)[v][v2] == 1) && ((g.matrix)[v2][v] == 1))
          return 1;
     return 0;
}

int get_adjacent_vertices(graph g, int vertex, int *output){
     int index, j;
     j = 0;
     for(index = 0; index < (g.size_max); index++){
          if(index == vertex)
               continue;
          if(adjacent(g, vertex, index) == 1) {
               output[j] = index;
               j++;
          }
     }
     return j;
}

void drop_graph(graph *g){
     int i;
     for( i = 0; i < g->size_max; i++){
          free((g->matrix)[i]);
          (g->matrix)[i] = NULL;
     }

     free(g->matrix);
     (g->matrix) = NULL;
}
