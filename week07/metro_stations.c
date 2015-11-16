#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct station_s {
     char name[100];
} station;

typedef struct map_s {
     station *stations_list;
     int station_num;
     graph graph;
} map;

map get_data_from_file(char *file_name);

int main() {

     map data = get_data_from_file("input.graph");

     return 0;
}

map get_data_from_file(char *file_name) {
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

     int MAX = 10;
     map ret;

     ret.stations_list = (station *)malloc(sizeof(station) * MAX);
     ret.station_num = 0;
     ret.graph = create_graph();

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

          if (ret.station_num == MAX - 1) {
               MAX += 10;
               ret.stations_list = (station *)realloc(ret.stations_list, sizeof(station) * MAX);
               if (ret.stations_list == NULL) {
                    fprintf(stderr, "Reallocate failed in %s:%d !!\n", __FILE__, __LINE__);
                    exit(1);
               }
          }

          sscanf(temp, "S%*d=%[^\n]\n", ret.stations_list[(ret.station_num)++].name);
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
               add_edge(ret.graph, v, v2);
               v = v2;
               p = strtok(NULL, " ");
          }
     }

     return ret;
}
