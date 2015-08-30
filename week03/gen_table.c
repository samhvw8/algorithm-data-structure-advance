#include <stdio.h>
#include <stdlib.h>
#include "gen_table.h"


symbol_table create_symble_table(entry (*make_node)(void *, void*),
                                      int(*compare)(void*, void*)){
     symble_table ret;
     ret.size = INITAL_SIZE;
     ret.total = 0;
     ret.make_node = make_node;
     ret.compare = compare;
     ret.entries = (entry *)malloc(sizeof(entry) * (ret,size));
     if(ret.entries == NULL){
          fprintf(stderr, "Allocate failed in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }

     return ret;
}

void drop_symbol_table(symble_table *tab){
     if(tab == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }

     if(tab -> entries != NULL){
          entry * to_free_entry = tab->entries;
          int idx = 0;
          while(idx < tab -> total){
               free(to_free_entry -> key);
               free(to_free_entry -> value);
               to_free_entry++;
          }

          free(entries);
     }

}

void add_entry(void *key, void *value, symble_table *book){
     if(key == NULL || book == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }

     if(book -> entries == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }

     if(book -> total ==  book -> size){
          (book -> size) += INCREMENTAL_SIZE;
          (book->entries) = (entry *)realloc(book->entries, sizeof(entry) * (book -> size));
          if(book->entries == NULL){
               fprintf(stderr, "Realloc failed in %s:%d \n", __FILE__, __LINE__);
               exit(1);
          }
     }

     entry * add = book -> entries;
     add[book->total] = book->make_node(key, value);
     (book->total)++;

}

entry * get_entry(void *key, symble_table book){
     if(key == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }
     if(book.entries == NULL){
          fprintf(stderr, "Error NULL in %s:%d \n", __FILE__, __LINE__);
          exit(1);
     }

     entry *ret = book.entries;
     int idx = 0;
     while(idx <  book.total){
          if(book->compare(ret->key, key) == 0)
               return ret;
          ret++;
          idx++;
     }

     return NULL;

}


