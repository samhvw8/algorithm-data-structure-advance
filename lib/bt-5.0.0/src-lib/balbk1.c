/*
 * $Id: balbk1.c,v 1.10 2012/11/05 10:34:58 mark Exp $
 *
 * balbk1: balances keys between blocks 
 *
 * Parameters:
 *   lblk   left block of pair 
 *   rblk   right block of pair 
 *   diff   if negative, move diff/2 keys left 
 *          else move diff/2 keys right 
 *   key    name of current parent key for both blocks 
 *   val    value of parent key
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

#include <stdlib.h>
#include <stdio.h>

#include "bc.h"
#include "bt.h"
#include "btree_int.h"

/* #undef DEBUG  */
/* #define DEBUG 1 */

void balbk1(BTint lblk,BTint rblk,int diff,KEYENT* p_kep)
{

    BTint tblk,fblk,tval,link1,link2;
    int i,dir,keypos,result,limit,ppos;
    char tkey[ZKYLEN];
    KEYENT* kep;

#if DEBUG >= 1
    printf("BALBK1: Balancing keys between lblk: " ZINTFMT ", rblk: " ZINTFMT
           "\n",lblk,rblk);
    printf("        with parent: %s\n",p_kep->key);
#endif
    if (diff < 0) {
        dir = 1;
        tblk = lblk;
        fblk = rblk;
        keypos = 0; 
        limit = abs(diff/2);
    }
    else {
        dir = -1;
        tblk = rblk;
        fblk = lblk;
        keypos = bgtinf(fblk,ZNKEYS)-1;
        limit = keypos-abs(diff/2);
    }
    /* copy parent key into to block, will be overwritten later */
    bputky(tblk,p_kep,ZNULL,ZNULL);
    /* move keys from fblk to tblk */
    for (i=keypos;i!=limit;i+=dir) {
        kep = bsrhbk(fblk,tkey,&keypos,&tval,&link1,&link2,&result);
        if (result != 0) {
            bterr("BALBK1",QBALSE,NULL);
            goto fin;
        }
        bremky(fblk,keypos);
        if (dir < 0) keypos--;
        bputky(tblk,kep,ZNULL,ZNULL);
    }
    /* move last key from fblk into parent */
    i = keypos;
    kep = bsrhbk(fblk,tkey,&keypos,&tval,&link1,&link2,&result);
    if (result != 0) {
        bterr("BALBK1",QBALSE,NULL);
        goto fin;
    }
    /* replace parent key; lfpos can be == nkeys as set by bleaf(1) */
    ppos = (btact->cntxt->lf.lfpos==bgtinf(btact->cntxt->lf.lfblk,ZNKEYS))?
        btact->cntxt->lf.lfpos-1:btact->cntxt->lf.lfpos;
    brepky(btact->cntxt->lf.lfblk,ppos,kep,lblk,rblk);
    bremky(fblk,keypos);
    btact->cntxt->stat.xbal++;
fin:
    return;
}
