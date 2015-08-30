typedef struct _phone_entry {
     char name[25],
          tel[12];
} phone_entry;

typedef struct _phone_book {
     phone_entry * entries;
     int total;
     int size;
} phone_book;

#define INITAL_SIZE 10
#define INCREMENTAL_SIZE 5

// create new phone book
phone_book create_phone_book();

// drop entries of a phone book
void drop_phone_book(phone_book *book);

/* add an entry in the phone book
   NB: if the entry exists, the value should be overwritten */
void add_phone_number(char *name, char *tel, phone_book *book);

// find an entry in the phone book
// return NULL if the entry does not exist
phone_entry *find_person_phone_book(char *name, phone_book book);

