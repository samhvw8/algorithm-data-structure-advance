#include <libfdr/jrb.h>
#include <libfdr/dllist.h>
#define INFINITY 100000000000
#define UNDEFINED -1
typedef struct graph_s {
  JRB vertices,
    edges;  
} graph;

extern graph create_graph();
extern void add_vertex(graph graph, int id, char *value);
extern char *get_vertex(graph graph, int id);
extern void add_edge(graph g, int v, int v2, double weight);
extern int has_edge(graph g, int v, int v2);
extern int in_degree(graph g, int vertex, int *output);
extern int out_degree(graph g, int vertex, int *output);
extern void drop_graph(graph *g);
extern void BFS(graph g, int start, int stop, void (*visited_func)(graph, int));
extern void DFS(graph g, int start, int stop, void (*visited_func)(graph, int));
extern int get_graph_max_id(graph g);
extern int get_graph_min_id(graph g);
extern int is_cyclic_util(graph g, int start);
extern int is_cyclic(graph g);
extern void TSort(graph g, void (*visited_func)(graph, int));
extern double get_edge_val(graph, int, int);
extern double shortest_path(graph, int s, int t, int* path, double* length);