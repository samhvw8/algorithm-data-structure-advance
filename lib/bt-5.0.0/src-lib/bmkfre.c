/*
 * $Id: bmkfre.c,v 1.8 2012/10/18 09:25:51 mark Exp $
 * 
 *  bmkfre: return block to free list
 *
 *  Parameters:
 *    blk    number of block to free up
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

void bmkfre(BTint blk)
{
    BTint croot = btact->cntxt->super.scroot;
    
    if (bgtinf(blk,ZBTYPE) == ZDUP && bgtinf(croot,ZNXDUP) == blk) {
        /* freeing active dup block */
        bstinf(croot,ZNXDUP,ZNULL);
    }
    bsetbk(blk,ZFREE,0,btact->cntxt->super.sfreep,0,0,ZNULL);
    btact->cntxt->super.sfreep = blk;
    btact->cntxt->super.snfree++;
    btact->cntxt->stat.xrel++;
    btact->cntxt->super.smod++;     /* super block updated */

    return;
}




