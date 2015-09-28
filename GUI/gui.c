#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bt/btree.h>
#include <gtk/gtk.h>
#define CSV "data.csv"

typedef struct _person{
     char tel[20];
     char email[60];
     char birth_day[20];
} person;

typedef struct _widgets
{
    GtkWidget *entry;
    GtkWidget *defnview;
    GtkListStore *list;
    GtkWidget *combo;
} widgets;


int main(int argc, char **argv){
     
     GtkBuilder *builder;
     GtkWidget  *application;
     GtkWidget  *add_window, *about_dialog;
     GtkWidget  *search, *restoreok;
     GtkWidget  *addok, *removeok, *editok;
     GError     *error = NULL;
     
     widgets *add = g_slice_new(widgets)
          , *remove = g_slice_new(widgets)
          , *edit = g_slice_new(widgets);


     BTA *btfile;
     btinit();


     btfile = btopn("phone_db", 0, FALSE);

     
     // gtk init
     gtk_init( &argc, &argv );
     builder = gtk_builder_new();
     if(!gtk_builder_add_from_file(builder, "guide.glade", &error))
     {
          g_warning("%s", error->message);
          g_free(error);
          return(1);
     }
     


     application = GTK_WIDGET(gtk_builder_get_object(builder, "application"));
     add_window = GTK_WIDGET(gtk_builder_get_object(builder, "add_window"));
     about_dialog = GTK_WIDGET(gtk_builder_get_object(builder, "about_dialog"));

     
     // connect signal
     gtk_builder_connect_signals( builder, NULL );



     //Xoa moi truong GtkBuilder
     g_object_unref( G_OBJECT( builder ) );

     //Hien thi cua so chinh cung cac widget khac
     gtk_widget_show(application);


     //Thuc hien vong lap main
     gtk_main();

     //Free du lieu
     g_slice_free (widgets, add);
     g_slice_free (widgets, remove);
     g_slice_free (widgets, edit);
     



     /*



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
     */

     btcls(btfile);

     return 0;
}
