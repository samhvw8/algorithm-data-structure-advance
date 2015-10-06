#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

graph create_graph(){
     return make_jrb();
}
void add_edge(graph g, int v, int v2){
     if(adjacent(g, v, v2))
        return;

#define ADD_EDGE_MACRO(v, v2)  {                        \
          JRB node = jrb_find_int(g, v);                \
          JRB tree;                                     \
          if(node == NULL){                             \
               tree = make_jrb();                       \
               jrb_insert_int(g, v, new_jval_v(tree));  \
          }                                             \
          else                                          \
               tree = (JRB) jval_v(node->val);          \
               jrb_insert_int(tree, v2, new_jval_i(1)); \
                                                        \
     } while(0);
     
     ADD_EDGE_MACRO(v, v2);
     ADD_EDGE_MACRO(v2, v);
     
#undef ADD_EDGE_MACRO
     
}

int adjacent(graph g, int v, int v2){
     if(g == NULL)
          return 0;
                                    
     JRB node = jrb_find_int(g, v);                           
     JRB tree;                                                
     if(node == NULL)                                         
          return 0;                                               
     tree = (JRB) jval_v(node->val);                      
     JRB f = jrb_find_int(tree, v2);
     if(f != NULL)
          return 1;
     return 0;

}

int get_adjacent_vertices(graph g, int vertex, int *output){
     if(g == NULL)
          return 0;
     JRB node = jrb_find_int(g, vertex);                           
     JRB tree;
     if(node == NULL)                                         
          return 0;
     int total = 0;
     tree = (JRB) jval_v(node->val);
     jrb_traverse(node, tree)
          output[total++] = jval_i(node->key);
     return total;
}
void drop_graph(graph *g){
     JRB graph = (JRB) *g;
     JRB node;
     jrb_traverse(node, graph)
          jrb_free_tree(jval_v(node->val));
     jrb_free_tree(*g);
     *g = NULL;
}
