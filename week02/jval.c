/*
  Generic Data type
  Author: Ginz
 */
#include <jval.h>

jval new_jval_int(int val){
  jval ret;
  ret.i = val;
  return ret;
}

jval new_jval_float(float val){
  jval ret;
  ret.f = val;
  return ret;
}

jval new_jval_double(double val){
  jval ret;
  ret.d = val;
  return ret;
}

jval new_jval_string(char* val){
  jval ret;
  ret.s = (char *)malloc(sizeof(char) + (strlen(val) + 1));
  if(ret.s == NULL){
       fprintf(stderr, "Allocate failed in %s:%d", __FILE__, __LINE__);
       exit(1);
  }
  sprintf(ret.s, "%s", val);
  return ret;
}

jval new_jval_void(void* val, size_t size){
  jval ret;
  ret.v = malloc(size);
  if(ret.v == NULL){
    fprintf(stderr, "Allocate failed !!\n");
    exit(0);
  }
  memcpy(ret.v, val, size);
  return ret;
}

jval new_jval_long(long val){
  jval ret;
  ret.l = val;
  return ret;
}

jval new_jval_char(char val){
  jval ret;
  ret.c = val;
  return ret;
}

void exch(jval *a, jval *b){
  jval temp;
  temp = *a;
  *a = *b;
  *b = temp;
}

void sort_gen(jval *arr, int l, int r, int (*compare)(jval *, jval *)){
  if(l >= r) return;
  int i, p,
    j, q,
    k;
  i = p = l - 1;
  j = q = r;
  while(1){
    while(compare(&arr[r], &arr[++i]) > 0);
    while(compare(&arr[r], &arr[--j]) < 0)
      if(j == l)
        break;
    
    if(i >= j)
      break;
    
    exch(&arr[i], &arr[j]);

    if(compare(&arr[r], &arr[i]) == 0) 
         exch(&arr[i], &arr[++p]);
    if(compare(&arr[r], &arr[j]) == 0) 
         exch(&arr[j], &arr[--q]);
  }
  exch(&arr[i], &arr[r]);
  j = i - 1;
  i = i + 1;
  
  for(k = l; k <= p; k++) exch(&arr[k], &arr[j--]);
  for(k = r - 1; k >= q; k--) exch(&arr[k], &arr[i++]);
  
  sort_gen(arr, l, j, compare);
  sort_gen(arr, i, r, compare);  
}

int search_gen(jval *arr, int l, int r, jval val,int (*compare)(jval *, jval *)){
     if(l > r) return -1;
     int m = (l + r)/2;
     if(compare(&arr[m], &val) == 0) return m;
     else if (compare(&arr[m], &val) < 0) return search_gen(arr, m + 1, r, val, compare);
     else return search_gen(arr, l, m - 1, val, compare);
}

void sort_gen_int(jval *arr, int l, int r){
     sort_gen(arr, l, r, compare_int);
}

int search_gen_int(jval *arr, int l, int r, jval val){
     return search_gen(arr, l, r, val, compare_int);
}

int compare_int(jval *a, jval *b){
  return (*a).i -(*b).i;
}
