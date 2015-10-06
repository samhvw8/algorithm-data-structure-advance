#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include <gtk/gtk.h>
#define NUMBERSUGGEST 20

//LENH COMPILE
//gcc `pkg-config --cflags gtk+-3.0` -o Dplus main.c `pkg-config --libs gtk+-3.0` -w libbt.a


//KHAI BAO
typedef struct
{
    GtkWidget *entry;
    GtkWidget *defnview;
    GtkListStore *list;
    GtkWidget *combo;
} Widgets;


//HAM THEM DU LIEU

int AddEntryClicked(BTA*b, char* wordFind, char* defnTemp)
{
    return (!btins(b,wordFind,defnTemp,sizeof(char)*6500));
}

static void on_addok_clicked(GtkWidget *button, Widgets *A, BTA *b)
{
    GtkTextIter start, end;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer (A->defnview);
    gchar *texte;
    gchar *textd;

    gtk_text_buffer_get_bounds (buffer, &start, &end);

    btinit();
    b = btopn("BTdata", 0, 0);

    texte = gtk_entry_get_text(GTK_ENTRY(A->entry));
    textd = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

    AddEntryClicked(b,texte,textd);
    gtk_entry_set_text(A->entry,"");
    gtk_text_buffer_set_text(buffer,"", -1);

    btcls(b);
}


//HAM XOA DU LIEU

int DeleteEntry(BTA*b, char* wordFind)
{
    char defnFind[6500];
    int rsize;
    int exist = btsel(b, wordFind, defnFind, sizeof(defnFind), &rsize);
    if (exist == 0)
    {
        return btdel(b, wordFind);
    }
    else
    {
        return 0;
    }
}

static void on_removeok_clicked(GtkWidget *button, Widgets *rem, BTA *b)
{
    gchar *text;
    b = btopn("BTdata", 0, 0);

    text = gtk_entry_get_text(GTK_ENTRY(rem->entry));

    DeleteEntry(b,text);
    gtk_entry_set_text(rem->entry,"");
    btcls(b);

}


//HAM SUA DU LIEU

int EditEntryClicked(BTA*b, char* wordFind, char* defnTemp)
{
    return btupd(b, wordFind, defnTemp, sizeof(char)*6500);
}

static void on_editok_clicked(GtkWidget *button, Widgets *edit, BTA *b)
{
    GtkTextIter start, end;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer (edit->defnview);
    gchar *texte;
    gchar *textd;

    gtk_text_buffer_get_bounds (buffer, &start, &end);

    btinit();
    b = btopn("BTdata", 0, 0);

    texte = gtk_entry_get_text(GTK_ENTRY(edit->entry));
    textd = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

    EditEntryClicked(b,texte,textd);
    gtk_entry_set_text(edit->entry,"");
    gtk_text_buffer_set_text(buffer,"", -1);

    btcls(b);
}


//HAM KHOI PHUC DU LIEU GOC

#define BlockSize 64
void CopybyBlock(FILE *fin, FILE *fout)
{
    int num;
    char buff[BlockSize + 1];
    while (!feof(fin))
    {
        num = fread(buff, sizeof(char),BlockSize, fin);
        buff[num * sizeof(char)] = '\0';
        fwrite(buff, sizeof(char), num, fout);
    }
}

void RestoreDataFile(BTA*b)
{
    btcls(b);
    remove("BTdata");
    FILE* ori = fopen("BTdata-o","rb");
    FILE* cur = fopen("BTdata", "wb");
    CopybyBlock(ori, cur);
    b = btopn("BTdata", 0, 0);
    fclose(ori);
    fclose(cur);
}

static void on_restore_clicked(GtkWidget *widget, gpointer window, BTA *b)
{
    GtkWidget *dialog;
    b = btopn("BTdata", 0, 0);
    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Restored default FOLDOC data!",
                                    "title");
    gtk_window_set_title(GTK_WINDOW(dialog), "Restore");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    RestoreDataFile(b);

    btcls(b);

}

//CAC HAM THUOC SOUNDEX
char **NextWordSoundex(BTA *sdx, char *sdxFind)  // Tham so la ma soundex cua tu vua tra nghia
{
    char similarWord[10000], *p;
    char **result = malloc(1000 * sizeof(char *));

    int rsize, n;
    n = 0;
    btsel(sdx, sdxFind, similarWord, sizeof(similarWord), &rsize);
    p = strtok(similarWord, "-");
    while (p != NULL)
    {
        result[n] = malloc(100);
        result[n++] = strdup(p);
        p = strtok(NULL, "-");
    }
    result[n] = NULL;
    return result;
}

char *soundex(char *chAlphaName)
{
    int i;
    int j = 0;
    char SCode = '0';
    char PrevCode = '0';
    char CharTemp = '0';
    char *strResult = malloc(sizeof(strlen(chAlphaName)));

    for (i = 0; i < strlen(chAlphaName); i++)
    {
        chAlphaName[i]=tolower(chAlphaName[i]);
    }

    for (i = 0; (i < strlen(chAlphaName) && j < 4); i++)
    {
        CharTemp = chAlphaName[i];

        switch(CharTemp)
        {
        case 'r':
            SCode = '6';
            break;
        case 'm':
        case 'n':
            SCode='5';
            break;
        case 'l':
            SCode='4';
            break;
        case 'd':
        case 't':
            SCode='3';
            break;
        case 'c':
        case 'g':
        case 'j':
        case 'k':
        case 'q':
        case 's':
        case 'x':
        case 'z':
            SCode = '2';
            break;
        case 'b':
        case 'f':
        case 'p':
        case 'v':
            SCode = '1';
            break;
        default:
            SCode = '0';
            break;
        }

        if (SCode > '0' || j==0)
        {
            //SCode la chu cai dau tien
            if (j == 0)
            {
                strResult[j] = chAlphaName[j];

                j++;
            }
            else if (SCode != PrevCode)
            {
                strResult[j] = SCode;
                j++;
            }
        }


        if (CharTemp == 'h' || CharTemp == 'w')
        {
            SCode = PrevCode;
        }

        PrevCode = SCode;
        SCode = '0';

    }

    for (i = j; i < 4; i++)
    {
        strResult[i] = '0' ;
    }
    strResult[i]='\0';
    return strResult;
}


//HAM TIM KIEM DU LIEU

gboolean list_store_remove_nth_row (GtkListStore *store, gint n)
{
    GtkTreeIter  iter;
    g_return_val_if_fail (GTK_IS_LIST_STORE(store), FALSE);
    if (gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(store), &iter, NULL, n))
    {
        gtk_list_store_remove(store, &iter);
        return TRUE;
    }
    return FALSE;
}

void SearchSuggest(BTA*b, int n, char* wordFind, GtkListStore *list)
{
    GtkTreeIter  iter;
    int i, rsize;
    char tmp[85];
    strcpy(tmp, wordFind);
    char *defnFind = malloc(6500*sizeof(char));
    char pre[85], now[85];
    for(i=n-1; i>=0; --i) list_store_remove_nth_row(list, i);
    for(i=0; i<n; i++)
    {
        btseln(b, wordFind, defnFind, 6500, &rsize);
        strcpy(now, wordFind);
        if (strcmp(pre,now)==0) break;
        gtk_list_store_append(list, &iter);
        gtk_list_store_set (list, &iter, 0, now, -1);
        strcpy(pre,now);
    }
    strcpy(wordFind,tmp);
}

char* SearchWord(BTA*b, char* wordFind, int *n)
{
    int rsize;
    char defnTemp[6500];
    if (btsel(b, wordFind, defnTemp, sizeof(defnTemp), &rsize) != 0)     *n = 1;

    return defnTemp;
}

static void enter_callback(GtkButton *button,Widgets *w,BTA*b)
{
    GtkTextBuffer *buffer, *clear;
    int i,check = 0;
    GtkTextIter start, end;
    char *tmp = (char *)malloc(6500);
    gchar *textget;
    char suggestword[85];
    b = btopn("BTdata", 0, 0);

    clear = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w->defnview));
    gtk_text_buffer_set_text(clear, "", -1);

    textget = gtk_entry_get_text(GTK_ENTRY(w->entry));
    if (strcmp(textget,"")!=0) gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w->combo), NULL, textget);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w->defnview));
    strcpy(suggestword, textget);
    strcpy(tmp,SearchWord(b,textget,&check));


    GtkTextMark *mark;
    GtkTextIter iter;
    mark = gtk_text_buffer_get_insert (buffer);
    gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);


    if(check != 1)  gtk_text_buffer_set_text(buffer,tmp, -1);
    else
    {
        gtk_text_buffer_insert (buffer, &iter,"\n\tWord not found! You may want to try:\n\t", -1);
        //SOUNDEX
        BTA *sdx = btopn("SOUNDEX", 0, 0);
        char **result;
        result = NextWordSoundex(sdx, soundex(strdup(suggestword)));
        while (*result)
        {
            gtk_text_buffer_insert (buffer, &iter,*result, -1);
            gtk_text_buffer_insert (buffer, &iter,"\n\t", -1);
            result++;
        }
        gtk_text_buffer_insert (buffer, &iter,"Suggested by Soundex.", -1);
        btcls(sdx);
    }
    SearchSuggest(b, NUMBERSUGGEST, suggestword, w->list);
    gtk_entry_set_text(GTK_ENTRY(w->entry),"");

    btcls(b);
}

//MAIN
int
main( int    argc,  char **argv )
{
    //Khai bao
    GtkBuilder *builder;
    GtkWidget  *window;
    GtkWidget  *adddialog, *removedialog, *editdialog;
    GtkWidget  *search, *restoreok;
    GtkWidget  *addok, *removeok, *editok;
    GError     *error = NULL;
    Widgets *S = g_slice_new(Widgets);
    Widgets *A = g_slice_new(Widgets);
    Widgets *R = g_slice_new(Widgets);
    Widgets *E = g_slice_new(Widgets);

    //Load cay Btree tu file BTdata
    printf("D+ Dictionary\n");
    BTA* b;
    btinit();
    b = btopn("BTdata", 0, 0);


    // Khoi tao moi truong GtkBuilder tu file gui.glade
    gtk_init( &argc, &argv );
    builder = gtk_builder_new();
    if( ! gtk_builder_add_from_file( builder, "gui.glade", &error ) )
    {
        g_warning( "%s", error->message );
        g_free( error );
        return( 1 );
    }

    //Lay cua so chinh va cac widget tu file giao dien
    window = GTK_WIDGET( gtk_builder_get_object( builder, "window" ) );

    adddialog = GTK_WIDGET( gtk_builder_get_object( builder, "adddialog" ) );
    removedialog = GTK_WIDGET( gtk_builder_get_object( builder, "removedialog" ) );
    editdialog = GTK_WIDGET( gtk_builder_get_object( builder, "editdialog" ) );

    search = GTK_WIDGET( gtk_builder_get_object( builder, "search" ) );
    S->entry = GTK_WIDGET( gtk_builder_get_object( builder, "searchentry" ) );
    S->combo = GTK_WIDGET( gtk_builder_get_object( builder, "comboboxtext1" ) );
    S->defnview = GTK_WIDGET( gtk_builder_get_object( builder, "definitionview" ) );
    S->list = GTK_LIST_STORE( gtk_builder_get_object( builder, "suggestslist" ) );

    addok = GTK_WIDGET( gtk_builder_get_object( builder, "addok" ) );
    A->entry = GTK_WIDGET( gtk_builder_get_object( builder, "entryadd" ) );
    A->defnview = GTK_WIDGET( gtk_builder_get_object( builder, "textadd" ) );

    removeok = GTK_WIDGET( gtk_builder_get_object( builder, "removeok" ) );
    R->entry = GTK_WIDGET( gtk_builder_get_object( builder, "entryremove" ) );

    editok = GTK_WIDGET( gtk_builder_get_object( builder, "editok" ) );
    E->entry = GTK_WIDGET( gtk_builder_get_object( builder, "entryedit" ) );
    E->defnview = GTK_WIDGET( gtk_builder_get_object( builder, "textedit" ) );

    restoreok = GTK_WIDGET( gtk_builder_get_object( builder, "restoreok" ) );

    //Ket noi tin hieu voi cac widget
    gtk_builder_connect_signals( builder, NULL );

    g_signal_connect(G_OBJECT(search) , "clicked", G_CALLBACK(enter_callback), (gpointer) S);
    g_signal_connect(G_OBJECT(S->entry) , "activate", G_CALLBACK(enter_callback), (gpointer) S);

    g_signal_connect(G_OBJECT(addok) , "clicked", G_CALLBACK(on_addok_clicked), (gpointer) A);

    g_signal_connect(G_OBJECT(removeok) , "clicked", G_CALLBACK(on_removeok_clicked), (gpointer) R);

    g_signal_connect(G_OBJECT(editok) , "clicked", G_CALLBACK(on_editok_clicked), (gpointer) E);

    g_signal_connect(G_OBJECT(restoreok) , "clicked", G_CALLBACK(on_restore_clicked), (gpointer) window);

    //Xoa moi truong GtkBuilder
    g_object_unref( G_OBJECT( builder ) );

    //Hien thi cua so chinh cung cac widget khac
    gtk_widget_show( window );

    //Dong Btree
    btcls(b);

    //Thuc hien vong lap main
    gtk_main();

    //Free du lieu
    g_slice_free (Widgets, S);
    g_slice_free (Widgets, A);
    g_slice_free (Widgets, R);
    g_slice_free (Widgets, E);

    return( 0 );
}
