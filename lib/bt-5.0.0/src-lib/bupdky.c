/*
 * $Id: bupdky.c,v 1.10 2012/10/31 18:39:09 mark Exp $
 *
 * bupdky:  updates value of  key
 *
 * Parameters:
 *   b      pointer to BT context
 *   key    key to update
 *   val    new value of key 
 *        
 * bupdky returns non-ZERO if error occurred
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
#include "btree.h"
#include "btree_int.h"

int bupdky(BTA *b, char *key,BTint val)
{
    int status;
    BTint lval;
    
    bterr("",0,NULL);
    if ((status=bvalap("BUPDKY",b)) != 0) return(status);
    btact = b;

    if (b->cntxt->super.smode != 0) {
        /* read only, can't update */
        bterr("BUPDKY",QNOWRT,NULL);
    }
    else {
        if (btact->shared && !block()) {
            bterr("BUPDKY",QBUSY,NULL);
            goto fin;
            }
        if (key != NULL) status = bfndky(b,key,&lval);
        if (status == 0) {
            bmodky(btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos,val);
        }
    }
fin:
    if (btact->shared) bulock();
    return(btgerr());
}
