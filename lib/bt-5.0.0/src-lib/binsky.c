/*
 * $Id: binsky.c,v 1.14 2012/11/05 10:34:58 mark Exp $
 *
 *
 * binsky:  inserts key into index 
 *
 * Parameters:
 *    b      pointer to BT context      
 *    key    key to insert
 *    val    value of key 
 *
 * binsky returns non-ZERO if error occurred
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

#include "btree.h"
#include "btree_int.h"
#include <string.h>

int binsky(BTA *b, char *key,BTint val)
{
    BTint lval,dups_allowed;
    int status;
    KEYENT keyent;
    
    bterr("",0,NULL);
    if ((status=bvalap("BINSKY",b)) != 0) return(status);

    btact = b;
    if (btact->shared) {
        if (!block()) {
            bterr("BINSKY",QBUSY,NULL);
            goto fin;
        }
    }

    if (b->cntxt->super.smode != 0) {
        /* read only, can't insert */
        bterr("BINSKY",QNOWRT,NULL);
    }
    else {
        dups_allowed = bgtinf(btact->cntxt->super.scroot,ZMISC);
        strncpy(keyent.key,key,ZKYLEN);
        keyent.key[ZKYLEN-1] = '\0';
        keyent.val = val;
        keyent.dup = ZNULL;
        status = bfndky(b,keyent.key,&lval);
        if (status == QNOKEY || (status == 0 && dups_allowed)) {
            /* QNOKEY is not an error in this context; remove it */
            bterr("",0,NULL);
            if (status == 0) {
                /* inserting duplicate key */
                btdupkey(key,val);
            }
            else {
                bleaf(1);
                bputky(btact->cntxt->lf.lfblk,&keyent,ZNULL,ZNULL);
            }
        }
        else {
            bterr("BINSKY",QDUP,key);
        }
    }
fin:
    if (btact->shared) bulock();
    return(btgerr());
}
