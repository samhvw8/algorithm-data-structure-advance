#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#define CSV "data.csv"

int compar(void * a, void *b);

entry create_phone_entry(void *name, void *tel);

symbol_table get_data_from_file(FILE *fp, int size);

void print_addrr_book(symbol_table book);

int count_line(FILE *fp);

int main(){

     FILE *fin = fopen(CSV, "r");
     if(fin == NULL){
          fprintf(stderr, "can't open file %s \n", CSV);
          exit(1);
     }

     int size = count_line(fin);

     symbol_table book = get_data_from_file(fin, size);

     print_addrr_book(book);

     char name_str[25];

     printf("Name want to find > ");
     scanf("%[^\n]", name_str);

     entry *find = get_entry(name_str, book);
     if(find == NULL){
          printf("Not found %s !!", name_str);
     } else {
          printf("%s - %s \n", (char *)(find->key), (char *)(find->value));
     }

     drop_symbol_table(&book);

     print_addrr_book(book);

     fclose(fin);
     return 0;
}

int count_line(FILE *fp){
     char s[1000];
     int count = 0;
     while(fgets(s, 1000, fp) != NULL){
          count++;
     }
     rewind(fp);
     return count;
}

symbol_table get_data_from_file(FILE *fp, int size){
     symbol_table ret = create_symbol_table(create_phone_entry, compar);

     int i;
     for(i = 0; i < size;i++){
          char name[25],
               tel[12];
          fscanf(fp, "%[^|]|%[^|]|%*[^|]|%*[^\n]\n", name, tel);
          add_entry(name, tel, &ret);
     }

     return ret;
}

void print_addrr_book(symbol_table book){
     entry *arr = book.entries;

     if(book.entries == NULL)
          return;
     int i;

     printf("\n\n");
     printf("%-25s %-13s\n", "Name", "Tel");
     for(i = 0; i < book.total; i++){
          char *name = (char *)arr->key;
          char *tel = (char *)arr->value;
          printf("%-25s %-13s\n", name, tel);
          arr++;
     }
     printf("\n\n");
}

entry create_phone_entry(void *name, void *tel){
     entry ret;
     ret.key = malloc(strlen(name) + 1);
     ret.value = malloc(strlen(tel) + 1);
     if(ret.key == NULL || ret.value == NULL){
          fprintf(stderr, "Allocate failed in %s:%d !!\n", __FILE__, __LINE__);
          exit(1);
     }
     strcpy(ret.key, name);
     strcpy(ret.value, tel);

     return ret;
}

int compar(void * a, void *b){
     return strcmp((char *)a, (char *)b);
}
