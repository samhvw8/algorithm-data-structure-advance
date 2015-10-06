typedef struct _graph {
     int **matrix;
     int size_max;
} graph;


extern graph create_graph(int size_max);
extern void add_edge(graph g, int v, int v2);
extern int adjacent(graph g, int v, int v2);
extern int get_adjacent_vertices(graph g, int vertex, int *output);
extern void drop_graph(graph *g);
