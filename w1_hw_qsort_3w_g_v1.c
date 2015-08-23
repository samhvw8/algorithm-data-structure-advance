#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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
typedef struct _address_book {
     char name[25],
          email[60],
          tel[12],
          birth_day[12];
} address_book;



int count_line(FILE *fp){
     char s[1000];
     int count = 0;
     while(fgets(s, 1000, fp) != NULL){
          count++;
     }
     rewind(fp);
     return count;
}
address_book* get_data_from_file(FILE *fp, int size){
     address_book *abook =(address_book *)malloc(sizeof(address_book) * size);
     if(abook == NULL){
          fprintf(stderr, "Allocate Failed in %s:%d", __FILE__, __LINE__);
          exit(1);
     }
     int i;
     for(i = 0; i < size;i++){
          fscanf(fp, "%[^|]|%[^|]|%[^|]|%[^\n]\n", abook -> name, abook -> tel, abook -> email,  abook -> birth_day);
          abook++;
     }
     return abook - size;
}

void print_addrr_book(address_book *arr, int size){
     int i;
     printf("\n\n");
     printf("%-25s %-13s %-62s %-13s\n", "Name", "Tel", "Email", "Birth day");
     for(i = 0; i < size; i++){
          printf("%-25s %-13s %-62s %-13s\n", arr -> name, 
                 arr -> tel, arr -> email, arr -> birth_day);
          arr++;
     }
     printf("\n\n");
}


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



void print_array(int* arr, int size){
  int i;
  for(i = 0; i < size; i++){
    printf("%d\t", arr[i]);
  }
  printf("\n");
}

int compar_address_name(const void * a, const void * b){
     address_book _a = *(address_book*)a;
     address_book _b = *(address_book*)b;
     return strcmp(_a.name, _b.name);
}


int main(){
     FILE *fp = fopen("Address_book.csv", "r");
     if(fp == NULL){
          fprintf(stderr, "Can't open file");
          exit(1);
     }

     clock_t s;
     float t_qsort, t_qsort_3way;
     int size_addrr = count_line(fp);

     address_book * addrr_book = get_data_from_file(fp, size_addrr);
     address_book addrr_book_2[size_addrr];
     memcpy(addrr_book_2, addrr_book, sizeof(address_book) * size_addrr);

     s = clock();
     qsort_3w_gen(addrr_book_2, size_addrr - 1, sizeof(address_book), compar_address_name);
     t_qsort_3way = (float)(clock() - s)/CLOCKS_PER_SEC;

     s = clock();
     qsort(addrr_book, size_addrr, sizeof(address_book), compar_address_name);
     t_qsort = (float)(clock() - s)/CLOCKS_PER_SEC;
     
     
     printf("after sort :\n");
     printf("Address book 1\n");
     print_addrr_book(addrr_book, size_addrr);
     printf("Address book 2\n");
     print_addrr_book(addrr_book_2, size_addrr);

     printf("Time run qsort built-in :%f\n", t_qsort);
     printf("Time run qsort 3 way generic :%f\n", t_qsort_3way);
}
