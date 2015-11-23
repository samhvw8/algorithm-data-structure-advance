#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_metro(graph data, int node) {
  printf(" %s ", get_vertex(data, node));
}

graph get_data_from_file(const char *file_name);

int main(int argc, char const *argv[]) {
     if (argc != 2) {
          printf("%s\n%s %s", "WRONG FORMAT !!\n", argv[0],
                 ".graph file"
                );
          exit(1);
     }



     graph data = get_data_from_file(argv[1]);

     int i;
     printf("%s\n", "LIST OF METRO STATIONS");
     int n = get_graph_node_num(data);
     for(i = 1; i <= n; i++)
       printf("%3d. %s\n", i, get_vertex(data, i));

     printf("\n\n");

     printf("%s", "Find shortest path between 2 metro stations :\n"
            "Select Number of start station \n\t> ");
     int s, e;
     scanf(" %d", &s);
     printf("%s", "Select Number of end station \n\t> ");
     scanf(" %d", &e);

     printf("%s\n", "shortest path :");
     BFS(data, s, e, print_metro);
     printf("\n");
     for(i = 1; i <= n; i++)
       free(get_vertex(data, i));
     drop_graph(&data);
     return 0;
}

graph get_data_from_file(const char *file_name) {
     FILE *f = fopen(file_name, "r");
     if (f == NULL) {
          fprintf(stderr, "Can't Open file %s !!\n", file_name);
          exit(1);
     }

     char temp[100];

     while (1) {
          fgets(temp, 100, f);
          if (strcmp(temp, "[STATIONS]\n") == 0)
               break;
     }

     graph ret = create_graph();

     // station name parse

     while (1) {
          fgets(temp, 100, f);
          if (strcmp(temp, "[LINES]\n") == 0)
               break;
          if (strcmp(temp, "\n") == 0)
               continue;

          int i;
          for (i = 0; temp[i] != '\0' && temp[i] != '='; i++);
          if (temp[i] == '\0')
               continue;
          int id;
          char *val = malloc(sizeof(char) * 30); // -------------------------------------------------------------
          sscanf(temp, "S%d=%[^\n]\n", &id, val);
          add_vertex(ret, id, val);
     }

     // lines parse
     while (1) {
          fgets(temp, 100, f);
          if (feof(f))
               break;
          if (strcmp(temp, "\n") == 0)
               continue;

          int i;
          for (i = 0; temp[i] != '\0' && temp[i] != '='; i++);
          if (temp[i] == '\0')
               continue;
          char temp2[100];

          sscanf(temp, "%*[^=]=%[^\n]", temp2);

          char *p = strtok(temp2, " ");
          int v;
          v = p[1] - '0';
          p = strtok(NULL, " ");
          while ( p != NULL )
          {
               int v2 = p[1] - '0';
               add_edge(ret, v, v2);
               v = v2;
               p = strtok(NULL, " ");
          }
     }
     fclose(f);
     return ret;
}
