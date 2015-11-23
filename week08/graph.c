#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

graph create_graph(){
       graph ret;
       ret.vertices = make_jrb();
       ret.edges = make_jrb();
       return ret;
}

void add_vertex(graph graph, int id, char *value){
  if(graph.edges == NULL || graph.vertices == NULL)
    return;

  JRB node = jrb_find_int(graph.vertices, id);                       
  if(node == NULL){                                             
    jrb_insert_int(graph.vertices, id, new_jval_s(value));               
  }                                                             
  

}

char *get_vertex(graph graph, int id){
  if(graph.edges == NULL || graph.vertices == NULL)
    return NULL;

  JRB node = jrb_find_int(graph.vertices, id);                       
  if(node != NULL){                                             
    return jval_s(node->val);
  }                                                             
  
  return NULL;

}


void add_edge(graph g, int v, int v2){
       if(has_edge(g, v, v2))
         return;

#define ADD_EDGE_MACRO(v, v2)  {                                \
         JRB node = jrb_find_int(g.edges, v);                   \
         JRB tree;                                              \
         if(node == NULL){                                      \
           tree = make_jrb();                                   \
           jrb_insert_int(g.edges, v, new_jval_v(tree));        \
         }                                                      \
         else                                                   \
           tree = (JRB) jval_v(node->val);                      \
         jrb_insert_int(tree, v2, new_jval_i(1));               \
                                                                \
       } while(0);
     
     ADD_EDGE_MACRO(v, v2);
     #ifdef UDGGIN
     ADD_EDGE_MACRO(v2, v);
     #endif
#undef ADD_EDGE_MACRO
     
}

int has_edge(graph g, int v, int v2){
  if(g.edges == NULL || g.vertices == NULL)
          return 0;
                                    
     JRB node = jrb_find_int(g.edges, v);                           
     JRB tree;                                                
     if(node == NULL)                                         
          return 0;                                               
     tree = (JRB) jval_v(node->val);                      
     JRB f = jrb_find_int(tree, v2);
     if(f != NULL)
          return 1;
     return 0;

}

int out_degree(graph g, int vertex, int *output){
     if(g.edges == NULL || g.vertices == NULL)
          return 0;
     JRB node = jrb_find_int(g.edges, vertex);                           
     JRB tree;
     if(node == NULL)                                         
          return 0;
     int total = 0;
     tree = (JRB) jval_v(node->val);
     jrb_traverse(node, tree)
          output[total++] = jval_i(node->key);
     return total;
}

int in_degree(graph g, int vertex, int *output){

     if(g.edges == NULL || g.vertices == NULL)
          return 0;

            
     JRB node = jrb_find_int(g.edges, vertex);                           
     if(node == NULL)                                         
          return 0;

     int total = 0;

     jrb_traverse(node, g.edges) {
       if(has_edge(g, jval_i(node->key), vertex)){
         output[total++] = jval_i(node->key);
       }
     }

     return total;
}

void drop_graph(graph *g){
 
     JRB node;
     jrb_traverse(node, g->edges)
          jrb_free_tree(jval_v(node->val));
     jrb_free_tree(g->vertices);
     jrb_free_tree(g->edges);

}

void BFS(graph g, int start, int stop, void (*visited_func)(graph, int)){
  if(g.edges == NULL || g.vertices == NULL)
    return;
     JRB tmp;
     int v = 0;
     jrb_traverse(tmp, g.edges){
          v++;
     }
     
     int *visited = (int*)malloc(v * sizeof(int));
     if(visited == NULL){
          fprintf(stderr, "Allocated failed in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }
     
     int i;
     for(i = 0; i < v; i++) visited[i] = 0;
     
     Dllist queue = new_dllist();
     


     // add start node to queue
     JRB node = jrb_find_int(g.edges, start);
     if(node == NULL)
          goto end;
         

     dll_append(queue, new_jval_i(start));
     //
   


     while(!dll_empty(queue)){
          Dllist node = dll_first(queue);
          int u = jval_i(node->val);
          dll_delete_node(node);
          
          if(!visited[u]){
            visited_func(g, u);
               visited[u] = 1;
          }
          
          if(u == stop) 
               goto end;
          
          JRB u_node = jrb_find_int(g.edges, u);
          if(u_node == NULL)
               continue;
          
          JRB vertex_connect_to_u_tree = (JRB)(jval_v(u_node->val));
         

          jrb_traverse(tmp, vertex_connect_to_u_tree){
               if(!visited[tmp->key.i])
                    dll_append(queue, new_jval_i(tmp->key.i));
          }
     }
end:
     free(visited);
     free_dllist(queue);
}

void DFS(graph g, int start, int stop, void (*visited_func)(graph, int)){
  if(g.edges == NULL || g.vertices == NULL)
    return;
     JRB tmp;
     int v = 0;
     jrb_traverse(tmp, g.edges){
          v++;
     }
     
     int *visited = (int*)malloc(v * sizeof(int));
     if(visited == NULL){
          fprintf(stderr, "Allocated failed in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }
     
     int i;
     for(i = 0; i < v; i++) visited[i] = 0;
     
     Dllist stack = new_dllist();
     


     // add start node to stack
     JRB node = jrb_find_int(g.edges, start);
     if(node == NULL)
          goto end;
         

     dll_append(stack, new_jval_i(start));
     //
   


     while(!dll_empty(stack)){
          Dllist node = dll_last(stack);
          int u = jval_i(node->val);
          dll_delete_node(node);
          
          if(!visited[u]){
            visited_func(g, u);
               visited[u] = 1;
          }
          
          if(u == stop) 
               goto end;
          
          JRB u_node = jrb_find_int(g.edges, u);
          if(u_node == NULL)
               continue;
          
          JRB vertex_connect_to_u_tree = (JRB)(jval_v(u_node->val));
         

          jrb_rtraverse(tmp, vertex_connect_to_u_tree){
               if(!visited[tmp->key.i])
                    dll_append(stack, new_jval_i(tmp->key.i));
          }
     }

end:
     free(visited);
     free_dllist(stack);
}

int get_graph_node_num(graph g){
    if(g.edges == NULL || g.vertices == NULL)
      return 0;
     JRB tmp;
     int v = 0;
     jrb_traverse(tmp, g.vertices){
          v++;
     }
     return v;
}

int is_cyclic_util(graph g, int start){
  if(g.edges == NULL || g.vertices == NULL)
    return 0;
     JRB tmp;
     int v = 0;
     jrb_traverse(tmp, g.edges){
          v++;
     }
     
     int *visited = (int*)malloc(v * sizeof(int));
     if(visited == NULL){
          fprintf(stderr, "Allocated failed in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }
     
     int i;
     for(i = 0; i < v; i++) visited[i] = 0;
     
     Dllist stack = new_dllist();
     


     // add start node to stack
     JRB node = jrb_find_int(g.edges, start);
     if(node == NULL)
          goto end;
         

     dll_append(stack, new_jval_i(start));
     //
   


     while(!dll_empty(stack)){
          Dllist node = dll_last(stack);
          int u = jval_i(node->val);
          dll_delete_node(node);
          
          if(!visited[u]){
            visited[u] = 1;
          }
          
          if(u == start) {
            free(visited);
            free_dllist(stack);
            return 1;
          }
          
          JRB u_node = jrb_find_int(g.edges, u);
          if(u_node == NULL)
               continue;
          
          JRB vertex_connect_to_u_tree = (JRB)(jval_v(u_node->val));
         

          jrb_rtraverse(tmp, vertex_connect_to_u_tree){
               if(!visited[tmp->key.i])
                    dll_append(stack, new_jval_i(tmp->key.i));
          }
     }

end:
     free(visited);
     free_dllist(stack);
     return 0;
}

int is_cyclic(graph g){
  if(g.edges == NULL || g.vertices == NULL)
    return 0;
     JRB tmp;
     int v = 0;
     jrb_traverse(tmp, g.edges){
          v++;
          if(is_cyclic_util(g, v))
            return 1;
     }
     return 0;
}