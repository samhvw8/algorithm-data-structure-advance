/*
 * $Id: bgtslt.c,v 1.9 2010/05/28 10:34:38 mark Exp $
 *
 * bgtslt: frees in-memory slot and returns index
 *
 * Parameters:
 *   None
 *    
 * bgtslt returns index of free memory block
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
#include "bc.h"
#include "bt.h"
#include "btree_int.h"

int bgtslt()
{
    int idx, i;

    idx = -1;
    /* any slots free? */
    for (i=0;i<ZMXBLK;i++) {
#if DEBUG >= 2
        printf("bgtslt: cntrl[%d].inmem = " ZINTFMT "\n",
               i,((btact->cntrl)+i)->inmem);
#endif
        if (((btact->cntrl)+i)->inmem < 0) {
            idx = i;
            break;
        }
    }
    if (idx < 0 ) {
        /* nope, need to free a memory block (least recently used)  */
        idx = bqhead();
    }
    if (idx < 0)
        bterr("BGTSLT",QNOMEM,NULL);
    else {
        /* flush block to disk if slot not empty */
        if (((btact->cntrl)+idx)->inmem >= 0)
            if (bwrblk(((btact->cntrl)+idx)->inmem) != 0)
                bterr("BGTSLT",QWRBLK,itostr(((btact->cntrl)+idx)->inmem));
    }
#if DEBUG >= 2
    printf("bgtslt: found idx of %d\n",idx);
#endif
    return(idx);
}
