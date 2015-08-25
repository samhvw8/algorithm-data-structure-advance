#include <jval.h>
#include <stdlib.h>
#include <time.h>

jval *create_array_jval_int(int size, int min, int max);

void print_array_jval(jval* arr, int size);

int main(){
     int size = 10;
     jval * arr = create_array_jval_int(size, 5, 15);

     print_array_jval(arr, size);
     sort_gen_int(arr, 0, size - 1);
     print_array_jval(arr, size);

     jval find_j;
     int find;
     printf("What's number you want to search !!\n> ");
     scanf("%d", &find);
     find_j = new_jval_int(find);
     int found_index = search_gen_int(arr, 0, size - 1, find_j);
     if(found_index == -1){
          printf("number %d not found !\n", find);
     } else
          printf("Found number %d in index %d\n", find, found_index);

     return 0;
}

jval *create_array_jval_int(int size, int min, int max){
     jval* array;
     array = (jval *)malloc(sizeof(jval) * size);
     if(array == NULL){
          fprintf(stderr, "Allocate failed in %s:%d", __FILE__, __LINE__);
          exit(1);
     }
     int i;
     srand(time(NULL));
     for(i = 0; i < size; i++){
          array[i].i = min + rand() % (max - min);
     }
     return array;
}

void print_array_jval(jval* arr, int size){
     int i;
     for(i = 0; i < size; i++){
          printf("%d\t", JVAL_I(arr[i]));
     }
     printf("\n");
}
