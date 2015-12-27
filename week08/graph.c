#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

graph create_graph() {
  graph ret;
  ret.vertices = make_jrb();
  ret.edges = make_jrb();
  return ret;
}

void add_vertex(graph graph, int id, char *value) {
  if (graph.edges == NULL || graph.vertices == NULL)
    return;

  JRB node = jrb_find_int(graph.vertices, id);
  if (node == NULL) {
    jrb_insert_int(graph.vertices, id, new_jval_s(value));
  }


}

char *get_vertex(graph graph, int id) {
  if (graph.edges == NULL || graph.vertices == NULL)
    return NULL;

  JRB node = jrb_find_int(graph.vertices, id);
  if (node != NULL) {
    return jval_s(node->val);
  }

  return NULL;

}


void add_edge(graph g, int v, int v2) {
  if (has_edge(g, v, v2))
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
#ifndef DIRECTED_GRAPH
  ADD_EDGE_MACRO(v2, v);
#endif

#undef ADD_EDGE_MACRO

}

int has_edge(graph g, int v, int v2) {
  if (g.edges == NULL || g.vertices == NULL)
    return 0;

  JRB node = jrb_find_int(g.edges, v);
  JRB tree;
  if (node == NULL)
    return 0;
  tree = (JRB) jval_v(node->val);
  JRB f = jrb_find_int(tree, v2);
  if (f != NULL)
    return 1;
  return 0;

}

int out_degree(graph g, int vertex, int *output) {
  if (g.edges == NULL || g.vertices == NULL)
    return 0;
  JRB node = jrb_find_int(g.edges, vertex);
  if (node == NULL)
    return 0;
  int total = 0;
  JRB tree;
  tree = (JRB) jval_v(node->val);
  jrb_traverse(node, tree)
  output[total++] = jval_i(node->key);
  return total;
}

int in_degree(graph g, int vertex, int *output) {

  if (g.edges == NULL || g.vertices == NULL)
    return 0;


  JRB node = jrb_find_int(g.vertices, vertex);
  if (node == NULL)
    return 0;

  int total = 0;

  jrb_traverse(node, g.vertices) {
    if (has_edge(g, jval_i(node->key), vertex)) {
      output[total++] = jval_i(node->key);
    }
  }

  return total;
}

void drop_graph(graph *g) {

  JRB node;
  jrb_traverse(node, g->edges)
  jrb_free_tree(jval_v(node->val));
  jrb_free_tree(g->vertices);
  jrb_free_tree(g->edges);

}

void BFS(graph g, int start, int stop, void (*visited_func)(graph, int)) {
  if (g.edges == NULL || g.vertices == NULL)
    return;
  JRB tmp;
  int max_id = get_graph_max_id(g);

  int *visited = (int*)malloc((max_id + 1) * sizeof(int));
  if (visited == NULL) {
    fprintf(stderr, "Allocated failed in %s:%d \n", __FILE__, __LINE__);
    exit(1);
  }


  JRB node;
  jrb_traverse(node, g.vertices) {
    visited[jval_i(node->key)] = 0;
  }

  Dllist queue = new_dllist();



  // add start node to queue
  node = jrb_find_int(g.vertices, start);
  if (node == NULL)
    goto end;


  dll_append(queue, new_jval_i(start));
  //



  while (!dll_empty(queue)) {
    Dllist node = dll_first(queue);
    int u = jval_i(node->val);
    dll_delete_node(node);

    if (!visited[u]) {
      visited_func(g, u);
      visited[u] = 1;
    }

    if (u == stop)
      goto end;

    JRB u_node = jrb_find_int(g.edges, u);
    if (u_node == NULL)
      continue;

    JRB vertex_connect_to_u_tree = (JRB)(jval_v(u_node->val));


    jrb_traverse(tmp, vertex_connect_to_u_tree) {
      if (!visited[tmp->key.i])
        dll_append(queue, new_jval_i(tmp->key.i));
    }
  }
end:
  free(visited);
  free_dllist(queue);
}

void DFS(graph g, int start, int stop, void (*visited_func)(graph, int)) {
  if (g.edges == NULL || g.vertices == NULL)
    return;
  JRB tmp;
  int max_id = get_graph_max_id(g);

  int *visited = (int*)malloc((max_id + 1) * sizeof(int));
  if (visited == NULL) {
    fprintf(stderr, "Allocated failed in %s:%d \n", __FILE__, __LINE__);
    exit(1);
  }

  JRB node;
  jrb_traverse(node, g.vertices) {
    visited[jval_i(node->key)] = 0;
  }

  Dllist stack = new_dllist();



  // add start node to stack
  node = jrb_find_int(g.edges, start);
  if (node == NULL)
    goto end;


  dll_append(stack, new_jval_i(start));
  //



  while (!dll_empty(stack)) {
    Dllist node = dll_last(stack);
    int u = jval_i(node->val);
    dll_delete_node(node);

    if (!visited[u]) {
      visited_func(g, u);
      visited[u] = 1;
    }

    if (u == stop)
      goto end;

    JRB u_node = jrb_find_int(g.edges, u);
    if (u_node == NULL)
      continue;

    JRB vertex_connect_to_u_tree = (JRB)(jval_v(u_node->val));


    jrb_rtraverse(tmp, vertex_connect_to_u_tree) {
      if (!visited[tmp->key.i])
        dll_append(stack, new_jval_i(tmp->key.i));
    }
  }

end:
  free(visited);
  free_dllist(stack);
}

int get_graph_max_id(graph g) {
  if (g.edges == NULL || g.vertices == NULL)
    return 0;
  int max_id = 0;
  JRB tmp;
  jrb_traverse(tmp, g.vertices) {
    int key = jval_i(tmp->key);
    if (key > max_id)
      max_id = key;
  }
  return max_id;
}

int get_graph_min_id(graph g) {
  if (g.edges == NULL || g.vertices == NULL)
    return 0;
  int min_id = 10000000;
  JRB tmp;
  jrb_traverse(tmp, g.vertices) {
    int key = jval_i(tmp->key);
    if (key < min_id)
      min_id = key;
  }
  return min_id;
}

int is_cyclic_util(graph g, int start) {
  if (g.edges == NULL || g.vertices == NULL)
    return 0;

  int max_id = get_graph_max_id(g);

  int *visited = (int*)malloc((max_id + 1) * sizeof(int));
  if (visited == NULL) {
    fprintf(stderr, "Allocated failed in %s:%d \n", __FILE__, __LINE__);
    exit(1);
  }

  JRB node;
  jrb_traverse(node, g.vertices) {
    visited[jval_i(node->key)] = 0;
  }

  Dllist stack = new_dllist();



  // add start node to stack
  node = jrb_find_int(g.vertices, start);
  if (node == NULL)
    goto end;


  dll_append(stack, new_jval_i(start));
  //

  int flag = 0;

  while (!dll_empty(stack)) {
    Dllist node = dll_last(stack);
    int u = jval_i(node->val);
    dll_delete_node(node);

    if (!visited[u]) {
      visited[u] = 1;
      if (!flag && (u == start))
        visited[u] = 0;
    }

    if ((u == start) && (visited[start] == 1)) {
      free(visited);
      free_dllist(stack);
      return 1;
    }

    flag++;

    JRB u_node = jrb_find_int(g.vertices, u);
    if (u_node == NULL)
      continue;

    int *out_degree_u_list = malloc((max_id + 1) * sizeof(int));
    int out_degree_u;
    if(out_degree_u_list == NULL){
      fprintf(stderr, "%s %s:%d\n", "malloc failed in", __FILE__, __LINE__);
      exit(1);
    }    

    if ((out_degree_u = out_degree(g, u, out_degree_u_list)) == 0)
      continue;

    int i;
    for(i = 0; i < out_degree_u; i++){
      int _v = out_degree_u_list[i];
      if (!visited[_v])
        dll_append(stack, new_jval_i(_v));
    }

    free(out_degree_u_list);
  }

end:
  free(visited);
  free_dllist(stack);
  return 0;
}

int is_cyclic(graph g) {
  if (g.edges == NULL || g.vertices == NULL)
    return 0;
  JRB tmp;
  jrb_traverse(tmp, g.vertices) {
    if (is_cyclic_util(g, jval_i(tmp->key)))
      return 1;
  }
  return 0;
}

void TSort(graph g, void (*visited_func)(graph, int)) {
  if (g.edges == NULL || g.vertices == NULL)
    return;

  Dllist queue = new_dllist();
  JRB tmp;

  JRB node = jrb_find_int(g.vertices, get_graph_min_id(g));
  if (node == NULL)
    goto end;

  int max_id = get_graph_max_id(g);
  int *in_degree_node = malloc(sizeof(int) * (max_id + 1));
  if (in_degree_node == NULL) {
    fprintf(stderr, "%s in %s:%d !!\n", "malloc failed", __FILE__, __LINE__);
    exit(1);
  }

  int *in_degree_arr = malloc(sizeof(int) * (max_id + 1));
  if (in_degree_arr == NULL) {
    fprintf(stderr, "%s in %s:%d !!\n", "malloc failed", __FILE__, __LINE__);
    exit(1);
  }



  jrb_traverse(tmp, g.vertices) {
    int v = jval_i(tmp->key);
    if ((in_degree_arr[v] = in_degree(g, v, in_degree_node)) == 0) {
      dll_append(queue, new_jval_i(v));
    }
  }

  free(in_degree_node);
  while (!dll_empty(queue)) {
    Dllist node = dll_first(queue);
    int u = jval_i(node->val);
    dll_delete_node(node);

    visited_func(g, u);

    int *out_degree_node_u = malloc(sizeof(int) * (max_id + 1));
    if (out_degree_node_u == NULL) {
      fprintf(stderr, "%s in %s:%d !!\n", "malloc failed", __FILE__, __LINE__);
      exit(1);
    }

    int out_degree_u;
    if ((out_degree_u = out_degree(g, u, out_degree_node_u)) != 0) {
      int w, i;
      int *in_degree_node_w = malloc(sizeof(int) * (max_id + 1));
      if (in_degree_node_w == NULL) {
        fprintf(stderr, "%s in %s:%d !!\n", "malloc failed", __FILE__, __LINE__);
        exit(1);
      }
      for (i = 0; i < out_degree_u; ++i)
      {
        w = out_degree_node_u[i];

        in_degree_arr[w] -= 1;

        if (in_degree_arr[w] == 0) {
          dll_append(queue, new_jval_i(w));
        }
      }
      free(in_degree_node_w);
    }
    free(out_degree_node_u);
  }

end:
  free(in_degree_arr);
  free_dllist(queue);
}