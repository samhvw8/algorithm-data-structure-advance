#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bt/btree.h>
#include <utility.h>
#define CSV "data.csv"

typedef struct _person{
     char tel[20];
     char email[60];
     char birth_day[20];
} person;



void get_data_from_file(FILE *fp, BTA *btfile, int size);

int count_line(FILE *fp);

int main(){

     FILE *fin = fopen(CSV, "r");
     if(fin == NULL){
          fprintf(stderr, "can't open file %s \n", CSV);
          exit(1);
     }

     int size = count_line(fin);

     BTA *btfile;
     btinit();

     if(get_menu("Has not Database file phone_db;Has Database file phone_db", 2, 0) == 2){
          btfile = btopn("phone_db", 0, FALSE);
     } else {
          btfile = btcrt("phone_db", 0, FALSE);
          get_data_from_file(fin, btfile, size);
     }

     fclose(fin);

     for(;;){
          switch(get_menu("Show All;Find;Modify", 3, 1)) {

          case 1:
          {
               btpos(btfile, ZSTART);
               printf("%-25s %-18s %-20s %-20s\n", "Name", "Date Birth", "Tel", "Email");
               while(1){
                    char name[100];
                    person info;
                    int size_receive;
                    if(btseln(btfile, name, (char *)&info, sizeof(person), &size_receive) == QNOKEY)
                         break;
                    printf("%-25s %-18s %-20s %-20s\n", name, info.birth_day, info.tel, info.email);
               }
          }
          break;
          case 2:
          {
               char name[40];
               printf("\n\t\t> Person name : ");
               scanf("%[^\n]", name);
               mfflush();
               person info_found;
               int actual_size;
               if(btsel(btfile, name, (char*)&info_found, sizeof(person), &actual_size) != 0){
                    printf("\t\t\tCan't found %s !!\n", name);
               } else {
                    printf("\t\t%-15s %-15s %-15s %-15s\n", name, info_found.birth_day, info_found.tel, info_found.email);
               }

          }
          break;
          case 3:
          {
               char name[40];
               printf("\n\t\t> Person name : ");
               scanf("%[^\n]", name);
               mfflush();
               person info_found;
               int actual_size;
               if(btsel(btfile, name, (char*)&info_found, sizeof(person), &actual_size) != 0){
                    printf("\t\t\tCan't found %s !!\n", name);
               } else {
                    printf("\t\t%-15s %-15s %-15s %-15s\n", name, info_found.birth_day, info_found.tel, info_found.email);
                    int c;
               continue_change:
                    if((c = get_menu("Edit Tel;Edit Email;Edit birthday", 3, 0)) == 1){
                         printf("\tChange %s --> ", info_found.tel);
                         scanf("%[^\n]", info_found.tel);
                         mfflush();
                    }
                    if (c == 2) {
                         printf("\tChange %s --> ", info_found.email);
                         scanf("%[^\n]", info_found.email);
                         mfflush();
                    }
                    if(c == 3) {
                         printf("\tChange %s --> ", info_found.birth_day);
                         scanf("%[^\n]", info_found.birth_day);
                         mfflush();
                    }

                    if(get_menu("Continue change another field;Don want", 2, 0) == 1)
                         goto continue_change;

                    btupd(btfile, name, (char *)&info_found, sizeof(person));
               }
          }
          break;
          case 0:
               goto end;
          }
     }

end:


     btcls(btfile);

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

void get_data_from_file(FILE *fp, BTA *btfile, int size){
     int i;
     for(i = 0; i < size;i++){
          person input;
          char name[100];
          fscanf(fp, "%[^|]|%[^|]|%[^|]|%[^\n]\n", name, input.tel, input.email, input.birth_day);
          btins(btfile, name, (char *)&input, sizeof(person));
     }
}
