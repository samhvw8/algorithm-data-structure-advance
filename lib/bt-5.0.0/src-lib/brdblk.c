/*
 * $Id: brdblk.c,v 1.9 2010/07/01 09:43:51 mark Exp $
 *
 * brdblk: reads block from disk into memory
 *
 * Parameters:
 *  blk    number of block to read into memory
 *  idx    returned with memory index of block
 *
 * brdblk returns non-ZERO if an i/o error occured
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

int brdblk(BTint blk,int *idx)
{
    BTint pos;
    int i,ioerr;

    ioerr = 0;
    *idx = -1;
    /* is block already in memory? */
    for (i=0;i<ZMXBLK;i++) {
        if (((btact->cntrl)+i)->inmem == blk) {
            btact->cntxt->stat.xlogrd++;
            *idx = i;
#if DEBUG > 0
            fprintf(stderr,"BRDBLK: found blk " ZINTFMT " at index %d\n",
                    blk,i);
#endif
            goto fin;
        }
    }

    if (i == ZMXBLK) {
        /* nope, get free slot */
        i = bgtslt();
    }
            
    if (i >= 0) {
        /* read block from disk */
        pos = fseeko(btact->idxunt, blk*ZBLKSZ,SEEK_SET);
        if (pos >= 0) {
            if ((ioerr = fread((btact->memrec)+i,sizeof(char),
                               ZBLKSZ,btact->idxunt)) == ZBLKSZ) {
                btact->cntxt->stat.xphyrd++;
                ioerr = 0;
                ((btact->cntrl)+i)->inmem = blk;
                *idx = i;
#if DEBUG > 0
                fprintf(stderr,"BRDBLK: reading block " ZINTFMT
                        ", into idx %d\n",blk,i);
#endif              
            }
            else {
                ioerr = -1;
            }
        }
        else {
            ioerr = -1;
        }
    }
  fin:
    if (*idx >= 0) bqmove(*idx);
    return(ioerr);
}
