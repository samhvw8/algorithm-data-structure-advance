#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX 100
#define MIN 20

void exch(int* a, int i, int j){
  int temp;
  temp = a[i];
  a[i] = a[j];
  a[j] = temp;
  
}
void sort2way(int a[], int l, int r){
  if (r <= l) return;
  
  int i = l - 1;
  int j = r;
  int k;

  while(1){

    while(a[++i] < a[r]);
    while(a[r] < a[--j]) if (j == l) break;
    if(i >= j) break;
    exch(a, i, j);
  }
  
  exch(a, i, r);
  j = i - 1;
  i = i + 1;
  sort2way(a, l, j);
  sort2way(a, i, r);
}

void sort3way(int a[], int l, int r){
  if (r <= l) return;
  
  int i = l - 1, p = l - 1;
  int j = r, q = r;
  int k;

  while(1){

    while(a[++i] < a[r]);
    while(a[r] < a[--j]) if (j == l) break;
  
    if(i >= j) break;
    
    exch(a, i, j);
    
    if (a[i] == a[r]) exch(a, ++p, i);
    if (a[j] == a[r]) exch(a, --q, j);
  }
  
  exch(a, i, r);
  j = i - 1;
  i = i + 1;
  
  for(k = l; k <= p; k++) exch(a, k, j--);
  for(k = r - 1; k >= q; k--) exch(a, k, i++);
  sort3way(a, l, j);
  sort3way(a, i, r);
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



int main(){
  int size = 100;
  int * array = create_array(size);
  int * array2 = (int *)malloc(sizeof(int) * size);
  time_t s, e;

  memcpy(array2, array, size * sizeof(int));
  printf("array 1: ");
  print_array(array, size);
  printf("array 2: ");
  print_array(array2, size);;
  
  s = time(NULL);
  sort2way(array, 0, size);
  printf("TIME : %lf\n",(float)(time(NULL) - s)/CLOCKS_PER_SEC);
  
  s = time(NULL);
  sort3way(array2, 0, size);
  printf("TIME : %lf\n",(float)(time(NULL) - s)/CLOCKS_PER_SEC);

  printf("After sort :\n");
  printf("array 1: ");
  print_array(array, size);
  printf("array 2: ");
  print_array(array2, size);

}
