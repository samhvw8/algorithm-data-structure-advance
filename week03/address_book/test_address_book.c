#include <stdio.h>
#include <stdlib.h>
#include "address_book.h"
#define CSV "data.csv"


phone_book get_data_from_file(FILE *fp, int size);

void print_addrr_book(phone_book book);

int count_line(FILE *fp);

int main(){

     FILE *fin = fopen(CSV, "r");
     if(fin == NULL){
          fprintf(stderr, "can't open file %s \n", CSV);
          exit(1);
     }

     int size = count_line(fin);

     phone_book book = get_data_from_file(fin, size);

     print_addrr_book(book);

     char name_str[25];

     printf("Name want to find > ");
     scanf("%[^\n]", name_str);

     phone_entry *find = find_person_phone_book(name_str, book);
     if(find == NULL){
          printf("Not found %s !!\n", name_str);
     } else {
          printf("%s - %s \n", find->name, find->tel);
     }

     drop_phone_book(&book);

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

phone_book get_data_from_file(FILE *fp, int size){
     phone_book ret = create_phone_book();

     int i;
     for(i = 0; i < size;i++){
          char name[25],
               tel[12];
          fscanf(fp, "%[^|]|%[^|]|%*[^|]|%*[^\n]\n", name, tel);
          add_phone_number(name, tel, &ret);
     }

     return ret;
}

void print_addrr_book(phone_book book){
     phone_entry *arr = book.entries;

     if(book.entries == NULL)
          return;
     int i;

     printf("\n\n");
     printf("STT %-25s\n", "Name");
     for(i = 0; i < book.total; i++){
       printf("%-3d %-25s\n", i + 1, arr -> name);
       arr++;
     }
     printf("\n\n");
}
