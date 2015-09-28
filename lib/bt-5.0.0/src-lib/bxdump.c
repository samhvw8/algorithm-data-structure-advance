/*
 * $Id: bxdump.c,v 1.6 2008/05/21 11:51:28 mark Exp $
 *
 *
 * bxdump - dumps binary data in hex format (with character translation)
 *
 * Parameters:
 *   p    - pointer to buffer
 *   size - size of buffer in bytes
 *
 * Copyright (C) 2003, 2004 Mark Willson.
 *
 * This file is part of the B Tree library.
 *
 * The B Tree library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The B Tree library  is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the B Tree library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <string.h>
#include "btree_int.h"

#define BYTELN 16

#define TRUE 1
#define FALSE 0

static char *charset = 
"................\
................\
 !\"#$%&'()*+,-./\
0123456789:;<=>?\
@ABCDEFGHIJKLMNO\
PQRSTUVWXYZ[.]^_\
`abcdefghijklmno\
pqrstuvwxyz...~.";

void bxdump(char *p,int size)
{
   int bytecount,i,eof;
   int repeating = FALSE;
   char *lastbuf = NULL;
   
    bytecount = 0;
    eof = FALSE;
    while (!eof) {
        eof = (bytecount >= size-1);
        if (eof) break;
        if (lastbuf != NULL) {
            if (memcmp(p+bytecount,lastbuf,BYTELN) == 0) {
                if (!repeating) {
                    repeating = TRUE;
                    printf("........\n");
                }
                bytecount += BYTELN;
                continue;
            }
            else {
                repeating = FALSE;
            }
        }
        lastbuf = p+bytecount;
        
        printf("%08d ",bytecount);
        for (i=0;i<BYTELN;i++) {
            if ((i%4) == 0) printf(" ");
            printf("%02X",(unsigned char) p[bytecount]);
            bytecount++;
        }
        bytecount -= BYTELN;
        printf("  *");
        for (i=0;i<BYTELN;i++) {
            if ((unsigned) p[bytecount] > 127) {
                printf("%c",charset[0]);
            }
            else {
                printf("%c",charset[(int) p[bytecount]]);
            }
            bytecount++;
        }
        printf("*\n");
    }
    return;
}
