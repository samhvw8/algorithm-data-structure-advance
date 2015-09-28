/*
 * $Id: bmodky.c,v 1.10 2012/10/31 18:39:09 mark Exp $
 *
 * bmodky: replaces value of key at location loc in block
 *
 * Parameters:
 *   blk    block for which replacement is required
 *   loc    location in block to store information
 *   val    new value of key
 *
 * Returns non-zero if an error occurred.
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

#include "bc.h"
#include "bt.h"
#include "btree_int.h"

int bmodky(BTint blk,int loc,BTint val)
{
    int idx,ioerr;

    if (loc >= ZMXKEY || loc < 0) {
        bterr("BMODKY",QLOCTB,itostr(loc));
    }
    else if (btdupupd(val) == ZNULL) {
        ioerr = brdblk(blk,&idx);
        if (idx < 0) {
            bterr("BMODKY",QRDBLK,itostr(blk));
        }
        else {
            ((btact->memrec)+idx)->keyblk[loc].val = val;
            ((btact->cntrl)+idx)->writes++;
#if DEBUG >= 1
            printf("BMODKY: Modifed target at blk: " ZINTFMT ", pos: %d\n",
                   blk,loc);
            printf(" ..using new val = " ZINTFMT "\n",val);
#endif
        }
    }
    return btgerr();
}
