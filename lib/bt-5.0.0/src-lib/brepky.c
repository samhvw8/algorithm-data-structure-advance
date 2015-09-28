/*
 * $Id: brepky.c,v 1.10 2012/11/05 10:34:58 mark Exp $
 *
 * brepky: replaces key at location loc in block
 *
 * Parameters:
 *   blk    block for which replacement is required
 *   loc    location in block to store information
 *   key    name of key
 *   val    value of key
 *   link1  left link pointer
 *   link2  right link pointer
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

int brepky(BTint blk,int loc,KEYENT* kep,BTint link1,BTint link2)
{
    int idx,ioerr;

    if (loc >= ZMXKEY || loc < 0) {
        bterr("BREPKY",QLOCTB,itostr(loc));
    }
    else {
        ioerr = brdblk(blk,&idx);
        if (idx < 0) {
            bterr("BREPKY",QRDBLK,itostr(blk));
        }
        else {
            ((btact->memrec)+idx)->keyblk[loc] = *kep;
            ((btact->memrec)+idx)->lnkblk[loc] = link1;
            ((btact->memrec)+idx)->lnkblk[loc+1] = link2;
            ((btact->cntrl)+idx)->writes++;
#if DEBUG >= 1
            printf("BREPKY: Replaced target at blk: " ZINTFMT ", pos: %d\n",
                   blk,loc);
            printf(" ..using '%s', val = " ZINTFMT ", llink = " ZINTFMT
                   ", rlink = " ZINTFMT "\n",
                        kep->key,kep->val,link1,link2);
#endif
        }
    }
    return(0);
}
