/*
 * $Id: bcpybk.c,v 1.8 2012/09/29 15:06:41 mark Exp $
 *
 * bcpybk: copies contents of one block to another
 *
 * Parameters:
 *     tblk   block number of to-block
 *     fblk   block number of from-block
 *     ts     first key number in to-block
 *     fs     first key number in from-block
 *     n      number of keys to copy
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


#include <string.h>
#include "bc.h"
#include "bt.h"
#include "btree_int.h"

int bcpybk(BTint tblk,BTint fblk,int ts,int fs,int n)
{
    int tidx,fidx,inc,i,j,ioerr;

    if (ts == 0)
        inc = 0;
    else
        inc = 1;

    /* get to-block in memory */
    ioerr = brdblk(tblk,&tidx);
    if (tidx < 0) {
        bterr("BCPYBK",QCPBLK,NULL);
        goto fin;
    }
    /* wire it down to ensure that it isn't replaced by from-block */
    bsetbs(tblk,1);
    /* get from-block in memory */
    ioerr = brdblk(fblk,&fidx);
    /* can un-busy to-block now */
    bsetbs(tblk,0);
    if (fidx < 0) {
        bterr("BCPYBK",QCPBLK,NULL); 
        goto fin;
    }
    /* copy keys and links */
    j = fs;
    for (i=ts;i<ts+n;i++) {
        ((btact->memrec)+tidx)->keyblk[i] = ((btact->memrec)+fidx)->keyblk[j];
        ((btact->memrec)+tidx)->lnkblk[i+inc] = 
            ((btact->memrec)+fidx)->lnkblk[j];
        j = j+1;
    }
    ((btact->memrec)+tidx)->lnkblk[ts+n] = ((btact->memrec)+fidx)->lnkblk[j];
    ((btact->cntrl)+tidx)->writes += n;
fin:
    return(0);
}
