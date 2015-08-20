#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX 100
#define MIN 20


void swap_block_mem(void* base, size_t size_block, size_t a, size_t b){
     char hold[size_block];
     memcpy(hold, (char *)base + a, size_block);
     memcpy((char *)base + a, (char *)base + b, size_block); 
     memcpy((char *)base + b, hold, size_block); 
}

void qsort_3w_gen(void* base, long int nmemb, long int size, int (*compar)(const void*,const void*)){
     if (nmemb <= 1) return ;
     char* base2 = (char*)base;
     long int i, p; // left
     long int j, q; // right
     long int k;
     long int pivot_index;
     
     i = p = 0;
     j = q = nmemb - 1;
     pivot_index = nmemb - 1;
     
     for(;;){
          while(compar(&base2[size*pivot_index], &base2[size*i]) > 0) i++;
          while(compar(&base2[size*pivot_index], &base2[size*j]) < 0) {
               j--;
               if(i == j) break;
          }
          
          if(i >= j) break;
          
          swap_block_mem(base2, size, i, j);

          if (compar(&base2[size*pivot_index], &base2[size*i]) == 0) {
               swap_block_mem(base2, size, p, i);
               p++;
          }                                                               
          if (compar(&base2[size*pivot_index], &base2[size*j]) == 0){
               swap_block_mem(base2, size, q, j);
               q--;
          } 
          
     }
     j = i - 1;
     i++;

     swap_block_mem(base2, size, i, pivot_index);
     
     for(k = 0; k <= p; k++) {
          swap_block_mem(base2, size, k, j--);
     }
     for(k = nmemb - 1 ; k >= q; k--) {
          swap_block_mem(base2, size, k, i++);
     }
       
     qsort_3w_gen(&base2[size * p], i - p, size, compar);
     qsort_3w_gen(&base2[size * j], q - j, size, compar); 
     
     
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
     //printf("%d %d %d\n", *(int*)a, *(int*)b, ret);
     return *(int*)a - *(int*)b;
}

int main(){
  int size = 10;
  int * array = create_array(size);
  int * array2 = (int *)malloc(sizeof(int) * size);
  time_t s;

  memcpy(array2, array, size * sizeof(int));
  printf("array 1: ");
  print_array(array, size);
  printf("array 2: ");
  print_array(array2, size);;

  printf("!!!\n\n");
//  s = time(NULL);
//  qsort(array, size, sizeof(int), compar_int);
//  printf("TIME : %lf", (float)(time(NULL) - s)/CLOCKS_PER_SEC);
       
//  s = time(NULL);
  qsort_3w_gen(array2, size, sizeof(int), compar_int);
//  printf("TIME : %lf", (float)(time(NULL) - s)/CLOCKS_PER_SEC);

//  printf("After sort :\n");
//  printf("array 1: ");
//  print_array(array, size);
//  printf("array 2: ");
  print_array(array2, size);

}
