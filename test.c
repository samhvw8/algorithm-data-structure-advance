#include <stdio.h>
#include <stdlib.h>


int main(){
     size_t a = 10;
     a -= 20;
     printf("%ul \n", a);
     if((long int)a >= 0){
          printf("true\n");
     } else{
          printf("false\n");
     }
     return 0;
}
