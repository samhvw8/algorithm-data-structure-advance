#include <libfdr/jrb.h>
#include <libfdr/dllist.h>
typedef JRB graph;

extern graph create_graph();
extern void add_edge(graph g, int v, int v2);
extern int adjacent(graph g, int v, int v2);
extern int get_adjacent_vertices(graph g, int vertex, int *output);
extern void drop_graph(graph *g);
extern void BFS(graph g, int start, int stop, void (*visited_func)(int));
extern void DFS(graph g, int start, int stop, void (*visited_func)(int));

