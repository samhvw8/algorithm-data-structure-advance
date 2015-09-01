#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "address_book.h"


// create new phone book
phone_book create_phone_book(){
     phone_book ret;
     ret.size = INITAL_SIZE;
     ret.total = 0;
     ret.entries = (phone_entry *)malloc(sizeof(phone_entry) * (ret.size));
     if(ret.entries == NULL){
          fprintf(stderr, "Allocate failed in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }
     return ret;
}

// drop entries of a phone book
void drop_phone_book(phone_book *book){
     if(book == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }

     if(book -> entries != NULL){
          free(book->entries);
          book->entries = NULL;
     }
     book->total = 0;
     book->size = INITAL_SIZE;
}

/* add an entry in the phone book
   NB: if the entry exists, the value should be overwritten */
void add_phone_number(char *name, char *tel, phone_book *book){
  
     if(name == NULL || book == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }
     if(find_person_phone_book(name, *book) != NULL) 
       return;

     if(book -> entries == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }

     if(book -> total ==  book -> size){
          (book -> size) += INCREMENTAL_SIZE;
          (book->entries) = (phone_entry *)realloc(book->entries, sizeof(phone_entry) * (book -> size));
          if(book->entries == NULL){
               fprintf(stderr, "Realloc failed in %s:%d \n", __FILE__, __LINE__);
               exit(1);
          }
     }

     phone_entry * add = book -> entries;
     // Add entry
     strcpy((add[book->total]).name, name);
     strcpy((add[book->total]).tel, tel);

     (book->total)++;

}

// find an entry in the phone book
// return NULL if the entry does not exist
phone_entry *find_person_phone_book(char *name, phone_book book){
     if(name == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }
     if(book.entries == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }

     phone_entry *ret = book.entries;
     int idx = 0;
     while(idx <  book.total){
          if(strcmp(ret->name, name) == 0)
               return ret;
          ret++;
          idx++;
     }

     return NULL;

}

