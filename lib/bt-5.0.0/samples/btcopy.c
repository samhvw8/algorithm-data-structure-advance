/*
 * 	NAME
 *      btcopy - copies keys and values from an existing btree file to
 *      a new one.
 *
 * 	SYNOPSIS
 *      btcopy old_file new_file
 *
 * 	DESCRIPTION
 *      btcopy copies all the keys and associated values from an
 *      existing btree file (old_file) to a newly created btree file
 *      (new_file).
 *
 * 	NOTES
 *      Provided as an example of use of the Btree library API.  No
 *      error checking implemented, to make the processing clearer.
 *
 * 	MODIFICATION HISTORY
 * 	Mnemonic	Rel	Date	Who
 *  btcopy      1.0 110622  mpw
 * 		Written.
 *
 *  $Id: btcopy.c,v 1.2 2011/06/23 09:49:37 mark Exp $ 		
 */

#include <stdio.h>
#include "btree.h"

int main(int argc, char *argv[])
{
    BTA *b1,*b2;
    char key[ZKYLEN+1];
    BTint i;
 
    if (argc != 3) {
        fprintf(stderr,"btcopy: usage: btcopy old_file new_file\n");
        return 1;
    }
    btinit();
    b1 = btopn(argv[1],0,FALSE);
    if (b1 != NULL) {
        b2 = btcrt(argv[2],0,FALSE);
        if (b2 != NULL) {
            puts("..copying");
            btpos(b1,ZSTART);
            while (bnxtky(b1,key,&i) == 0) binsky(b2,key,i);
            btcls(b2);
        }
        btcls(b1);
    }
    return 0;
}
