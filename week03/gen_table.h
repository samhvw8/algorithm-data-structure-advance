typedef struct _entry {
     void * key;
     void * value;
} entry;

typedef struct _symbol_table {
     entry * entries;
     int size,
          total;
     entry  (*make_node)(void *, void *); // refer function create entry
     int (*compare)(void *, void*); // refer to compare function
} symbol_table;

#define INITAL_SIZE 100
#define INCREMENTAL_SIZE 10

symbol_table create_symble_table(entry (*make_node)(void *, void*),
                                 int(*compare)(void*, void*));

void drop_symbol_table(symble_table *tab);

void add_entry(void *key, void *value, symble_table *book);

entry * get_entry(void *key, symble_table book);


