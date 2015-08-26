#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX 10
#define MIN 5
/* Byte-wise swap two items of size SIZE. */
#define SWAP(a, b, size)                        \
     do                                         \
     {                                          \
          register size_t __size = (size);      \
          register char *__a = (a), *__b = (b); \
          do                                    \
          {                                     \
               char __tmp = *__a;               \
               *__a++ = *__b;                   \
               *__b++ = __tmp;                  \
          } while (--__size > 0);               \
     } while (0)



void qsort_3w_gen(void* base, long int last_index_b, long int size, int (*compar)(const void*,const void*)){
     if (last_index_b <= 0) return ;
     char* base2 = (char*)base;
     long int i, p; // left
     long int j, q; // right
     long int k;
     long int pivot_index;
     
     i = p = - 1;
     j = q = last_index_b;
     pivot_index = last_index_b;
     
     for(;;){
          
          while(compar(&base2[size * pivot_index], &base2[size * (++i)]) > 0);  
          while(compar(&base2[size * pivot_index], &base2[size *(--j)]) < 0) 
               if(i == j) break;
          
          if(i >= j) break;
          SWAP(&base2[size * i], &base2[size * j], size);
          
          if (compar(&base2[size*pivot_index], &base2[size*i]) == 0) {
               SWAP(&base2[size * i], &base2[size * ++p], size);
          }                                                               
          if (compar(&base2[size*pivot_index], &base2[size*j]) == 0){
               SWAP(&base2[size * --q], &base2[size * j], size);
          } 
          
     }
     SWAP(&base2[size * i], &base2[size * pivot_index], size);
     j = i - 1;
     i = i + 1;
     
     
     for(k = 0; k <= p; k++, j--) {
          SWAP(&base2[size * k], &base2[size * j], size);
     }
     for(k = last_index_b - 1 ; k >= q; k--, i++) {
          SWAP(&base2[size * k], &base2[size * i], size);
     }
       
     qsort_3w_gen(base2, j, size, compar);
     qsort_3w_gen(&base2[size * i], last_index_b - i, size, compar); 
}

int *create_array(int size){
  int* array;
  array = (int *)malloc(sizeof(int) * size);
  if(array == NULL){
    fprintf(stderr, "Allocate failed in %s:%d", __FILE__, __LINE__);
    exit(1);
  }
  int i;
  srand(time(NULL));
  for(i = 0; i < size; i++){
    array[i] = MIN + rand() % (MAX - MIN);
  }
  return array;
}

void print_array(int* arr, int size){
  int i;
  for(i = 0; i < size; i++){
    printf("%d\t", arr[i]);
  }
  printf("\n");
}

int compar_int(const void * a, const void * b){
     int ret = *(int*)a - *(int*)b;
     //printf("cmp :%d %d %d\n", *(int*)a, *(int*)b, ret);
  
     return *(int*)a - *(int*)b;
}

int main(){
  int size = 1000;
  int * array = create_array(size);
  int array2[size];
  clock_t s, e;

  memcpy(array2, array, size * sizeof(int));
  print_array(array, size);

  printf("\n\n\n----------------\n\n\n");
  s = clock();
  qsort(array, size, sizeof(int), compar_int);

  printf("Time run qsort 2 way built-in : %lf \n", (float)(clock() - s)/CLOCKS_PER_SEC);
  
  s = clock();
  qsort_3w_gen(array2, size-1, sizeof(int), compar_int);

  printf("TIME run qsort 3 way generic: %lf", (float)(clock() - s)/CLOCKS_PER_SEC);
  
  printf("\nAfter sort :\n");
  printf("array 1: ");
  print_array(array, size);
  printf("array 2: ");
  print_array(array2, size);
  free(array);
}
