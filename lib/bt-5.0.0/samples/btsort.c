/*
 * 	NAME
 *      btsort - an inefficient and limited version of the UNIX sort command.
 *
 * 	SYNOPSIS
 *      btsort <unsorted_data 
 *
 * 	DESCRIPTION
 *      Accepts strings, one per line, from stdin.  On EOF, produces a
 *      sorted list of the input strings on stdout.
 *
 * 	NOTES
 *      Provided as an example of use of the Btree library API. 
 *
 * 	MODIFICATION HISTORY
 * 	Mnemonic	Rel	Date	Who
 *  btsort      1.0 110622  mpw
 * 		Written.
 *
 *  $Id: btsort.c,v 1.2 2011/06/23 09:51:39 mark Exp $		
 */  

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

void print_bterror(void)
{
    int errorcode, ioerr;
    char fname[ZRNAMESZ],msg[ZMSGSZ];

    btcerr(&errorcode,&ioerr,fname,msg);
    fprintf(stderr,"btsort: btree error (%d) [%s] - %s\n",
            errorcode,fname,msg);
}

int main(int argc, char *argv[])
{
    char tmpfile[80];
    char key[ZKYLEN];
    int status,val;
    BTA *dict;

    sprintf(tmpfile,"/tmp/bt_%d",getpid());
    btinit();
    if ((dict = btcrt(tmpfile,0,FALSE)) != NULL) {
        unlink(tmpfile);
        while (fgets(key,ZKYLEN,stdin) != NULL) {
            if (strrchr(key,'\n') != NULL) key[strlen(key)-1] = '\0';
            status = binsky(dict,key,0);
            if (status != 0)  {
                print_bterror();
                exit(EXIT_FAILURE);
            }
        }
        btpos(dict,ZSTART);
        while (bnxtky(dict,key,&val) == 0) puts(key);
    }
    else {
        fprintf(stderr,"Unable to open temp btree index.\n");
    }
    btcls(dict);
    return EXIT_SUCCESS;
}

                
        
