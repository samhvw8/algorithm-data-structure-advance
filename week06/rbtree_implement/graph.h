#include <libfdr/jrb.h>
typedef JRB graph;

extern graph create_graph();
extern void add_edge(graph g, int v, int v2);
extern int adjacent(graph g, int v, int v2);
extern int get_adjacent_vertices(graph g, int vertex, int *output);
extern void drop_graph(graph *g);
