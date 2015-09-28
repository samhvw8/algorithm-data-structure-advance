/*
 * $Id: btstk.c,v 1.8 2010/11/21 15:04:28 mark Exp $
 *
 *
 * bpull: pull integer value off stack
 *
 * Parameters:
 *   None
 *
 * bpush: push integer value on stack
 *
 * Parameters:
 *   val    value to push on stack
 *
 * bstkin: initialise stack pointer
 *
 * Parameters:
 *   None
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

#include "bc.h"
#include "bt.h"
#include "btree_int.h"

BTint bpull()
{
    BTint val;

    if (btact->cntxt->stk.stkptr < 0) 
        bterr("BPULL",QSTKUF,NULL);
    else {
        val = btact->cntxt->stk.stk[btact->cntxt->stk.stkptr];
        btact->cntxt->stk.stkptr--;
    }
    return(val);
}

int bpush(BTint val)
{
    if (btact->cntxt->stk.stkptr >= STKMAX) {
        bterr("BPUSH",QSTKOF,NULL);
    }
    else {
        btact->cntxt->stk.stkptr++;
        btact->cntxt->stk.stk[btact->cntxt->stk.stkptr] = val;
    }
    return(0);
}

void bstkin()
{
    btact->cntxt->stk.stkptr = -1;
}

int btstk_depth(void)
{
    return btact->cntxt->stk.stkptr+1;
}
