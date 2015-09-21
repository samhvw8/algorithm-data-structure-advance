/**
 * utility header 
 */
#ifndef _UTILITY_G_H_
#define _UTILITY_G_H_
#define INVALID -1
#define MAXOPTION 100
#define TABS 8
#define TAB 2
#define JOINTL "."
#define JOINTR "."
#define ROW '='
#define COLL "||"
#define COLR "||"
#ifndef PERROR
#define PERROR fprintf(stderr,"\t\tERROR in %s:%d\n",__FILE__,__LINE__)
#endif
/**
 *
  /========== Menu ==========\
  >|  1. Test 1   2. Test 2 |<
  >|  3. Test 3   4. Test 4 |<
  >|  5. Test 5   6. Test 6 |<
  >|  0. Exit               |<
  \==========================/
          Your choice:
 */
// ================================================
//print character  c with (times) 
extern void repchar(int times,char c);
// max lengthgth in 2 demention string for menu **
extern int count2da(char arr[][MAXOPTION],unsigned int num);
// check string is all number or not
extern int checkan(char arr[],int length);
// Menu 
extern int get_menu(const char *a,int o,int EXIT);
// my fflush
extern void mfflush() ;

extern float ran_float(int limdown,int limup);

// standardized name 
extern void stand_name(char *str);

extern int nmstr(char *b,char *a);

#endif 
