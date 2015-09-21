#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "utility.h"
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
void repchar(int times,char c){
  int i;
  for(i = 0; i < times; ++i) printf("%c",c);
}
void indent(int INDT){
  repchar(INDT,'\t');
}
// max lengthgth in 2 demention string for menu **
int count2da(char arr[][MAXOPTION],unsigned int num){
  unsigned int i,m; m = 0;
  for (i = 0; i < num; ++i) if(m < strlen(arr[i]))m = strlen(arr[i]);
  if(m > 4) return m;
  else return 4;
}
// check string is all number or not
int checkan(char arr[],int length){
  length--;
  int i;
  for (i = 0 ; i < length ; i++) if(arr[i] < '0' || arr[i] > '9') return 0;
    return 1;
}
// Menu 
int get_menu(const char *a,int o,int EXIT){
    /**
     * Require checkan ,count2da,repchar  function
     * o is number of option
     * option device by  character'|'
     */
    int id1,j,id,id2;
    id1=j=id=0;
    char matrix[o][MAXOPTION];
    int c,i,option = INVALID;
    char input[MAXOPTION];
     while(a[id] != '\0') {
        if(a[id] != ';' ){matrix[id1][j] = a[id];j++;id++;}
        else {matrix[id1][j] = '\0';j=0;id1++;id++;}
          }
     matrix[id1][j] = '\0';
      //
    do {
      int flag = 0;
      i=0;if(o<=5|| ((count2da(matrix,o)+7)*2+2+TABS+TAB*TABS) > 80){
        if(count2da(matrix,o)%2!=0) {
          printf("\n");repchar(TAB,'\t');printf(JOINTL);repchar(((count2da(matrix,o)+4)/2),ROW);printf(" Menu ");repchar(((count2da(matrix,o)+4)/2-1),ROW);printf(JOINTR"\n");
        } else {
          printf("\n");repchar(TAB,'\t');printf(JOINTL);repchar(((count2da(matrix,o)+4)/2-1),ROW);printf(" Menu ");repchar(((count2da(matrix,o)+4)/2-1),ROW);printf(JOINTR"\n");
       

        } for(id2 = 0 ; id2 < o;id2++){
          repchar(TAB,'\t');printf(COLL"%3d. %-*s"COLR"\n",id2+1,count2da(matrix,o)+1,matrix[id2]);
        
        }  if(EXIT == 1){
          repchar(TAB,'\t');printf(COLL"  0. %-*s"COLR"\n",(count2da(matrix,o)+1),"Exit");
        } 
        repchar(TAB,'\t');printf(JOINTR);repchar(((count2da(matrix,o)+7)*2)/2+1,ROW);printf(JOINTL"\n"); repchar(TAB,'\t');printf("Your choice:   ");
      } else {
        printf("\n");repchar(TAB,'\t');printf(JOINTL);repchar(count2da(matrix,o)+4,ROW);printf(" Menu ");repchar(count2da(matrix,o)+4,ROW);printf(JOINTR"\n");
        for(id2 = 0 ; id2 < o;id2++) {
          if(flag == 0){
            repchar(TAB,'\t');printf(COLL"%3d. %-*s",id2+1,count2da(matrix,o)+1,matrix[id2]);flag++;
          } else {
            printf("%3d. %-*s"COLR"\n",id2+1,count2da(matrix,o)+1,matrix[id2]);flag=0;
          }
        }
        if(EXIT == 1) {
          if(o%2 == 0){
            repchar(TAB,'\t');printf(COLL"  0. %-*s"COLR"\n",(count2da(matrix,o)+1)*2+5,"Exit");
          } else printf("  0. %-*s"COLR"\n",(count2da(matrix,o)+1),"Exit");
        }
        repchar(TAB,'\t');printf(JOINTR);repchar((count2da(matrix,o)+7)*2,ROW);printf(JOINTL"\n"); repchar(TAB,'\t');printf("Your choice:   ");}
        //
        while((c=getchar())!= '\n') { input[i] = c; i++;} input[i]='\0';i++;
        //
        if(checkan(input,i)) option = atoi(input);
        //
        if(option <= o && option >= 0 && checkan(input,i) && i != 1) {
	  if(option != 0) {
	       repchar(TAB,'\t');
	       printf("Selected Option Number : %d \n",option);
	  }
	  else {
	    if(EXIT) {
	      repchar(TAB,'\t');
	      printf("Selected Option Number : %d \n",option);
	    }
	    else {
	      repchar(TAB,'\t');printf("Invalid input. Please retry!\n");
	      option = INVALID;
	    }
	  }
	} 
        else {
            repchar(TAB,'\t');printf("Invalid input. Please retry!\n");
            option = INVALID;
        }
    } while (option == INVALID);
 return option;
}
// my fflush
void mfflush() {
  int ch;
  while((ch = getchar()) && ch != '\n' && ch != EOF);
}


/**
 * random Floating-point numbers
 * @param  limdown [limit downer]
 * @param  limupr  [limit upper]
 * @return         [Floating-point numbers]
 */
float ranfloat(int limdown,int limup){
    int range = limup - limdown -1;
    float a,b;

    a = rand()%range+1;
    b = rand()%99+1;
    return limdown+a+b/100;
}


// standardized name 
void standname(char *str){
  char s[strlen(str)+1];
  unsigned int i,z=0,f=1;
  while(str[z] == ' ')z++;
  for(i = z, z= 0 ; i<strlen(str) ;i++){
      if(str[i]!=' '&& f == 1 && str[i] <= 'z' && str[i] >= 'a'){s[z] = str[i] - 'a' + 'A';f=0;z++;i++;}
      if(str[i]!=' '&& f == 1 && str[i] <= 'Z' && str[i] >= 'A'){s[z] = str[i];f=0;z++;i++;}
      if(str[i]!=' '){s[z] = str[i];z++;}
      if(str[i]==' ' && f == 0) {s[z] = str[i];f = 1;z++;}
      if(f == 1) while(str[i]== ' ' && str[i+1]== ' ') i++;
  }
  if(s[z-1]== ' ') z--;
  for (i = 0; i < z; ++i)str[i] = s[i];
  str[i] = '\0';
}

int nmstr(char *b,char *a){
  char *str = NULL;
  str = strstr(b,a);
  if(str == NULL) return 0;
  return 1;
}


