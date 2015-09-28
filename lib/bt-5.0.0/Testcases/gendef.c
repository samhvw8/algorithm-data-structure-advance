#include <stdio.h>
#include <stdlib.h>

#define getrandom( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))


main(argc,argv)
int argc;
char *argv[];
{
	int i,j,count;
	char name[5];

	if (argc < 2 ) {
		fprintf(stderr,"gendef: count arg missing\n");
		exit(1);
	}
	count = atoi(argv[1]);
	if (count >= 10000000) {
		fprintf(stderr,"gendef: count arg too large\n");
		exit(1);
	}
	for (i=0;i<count;i++) {
		for (j=0;j<4;j++) name[j] = getrandom(64,126);
/* 		strcpy(name,"aaaa"); */
		name[4] = '\0';
		printf("d %s%07\n",name,i);
	}
}
