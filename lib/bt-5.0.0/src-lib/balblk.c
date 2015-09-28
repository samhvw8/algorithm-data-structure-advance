/*
 * $Id: balblk.c,v 1.8 2012/11/05 10:34:58 mark Exp $
 *
 * balblk: attempts to balance blocks
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
#include "bc.h"
#include "bt.h"
#include "btree_int.h"

void balblk() 
{
    BTint val,llink,rlink,cblk;
    int nkeys,cnkeys,result,diff;
    char tkey[ZKYLEN];
    KEYENT* kep;
    
    cblk = btact->cntxt->lf.lfblk;
    /* get parent block */
    btact->cntxt->lf.lfpos = bpull();
    btact->cntxt->lf.lfblk = bpull();
    nkeys = bgtinf(btact->cntxt->lf.lfblk,ZNKEYS);
    cnkeys = bgtinf(cblk,ZNKEYS);
    if (btact->cntxt->lf.lfpos < nkeys) {
        /* check right sister block */
        kep = bsrhbk(btact->cntxt->lf.lfblk,tkey,&btact->cntxt->lf.lfpos,&val,
                     &llink,&rlink,&result);
        if (result != 0) {
            bterr("BALBLK",QBALSE,NULL);
            goto fin;
        }
        nkeys = bgtinf(rlink,ZNKEYS);
        diff = cnkeys-nkeys;
        if (abs(diff) > (int)(ZMXKEY/2)) {
            balbk1(cblk,rlink,diff,kep);
            goto fin;
        }
    }
    if (btact->cntxt->lf.lfpos > 0) {
        /* check left sister block */
        btact->cntxt->lf.lfpos--;
        kep = bsrhbk(btact->cntxt->lf.lfblk,tkey,&btact->cntxt->lf.lfpos,
                     &val,&llink,&rlink,&result);
        if (result != 0) {
            bterr("BALBLK",QBALSE,NULL);
            goto fin;
        }
        nkeys = bgtinf(llink,ZNKEYS);
        diff = nkeys-cnkeys;
        if (abs(diff) > (int)(ZMXKEY/2)) {
            balbk1(llink,cblk,diff,kep);
            goto fin;
        }
        btact->cntxt->lf.lfpos++;
    }
    /* no balancing possible; restore original environment */
    bpush(btact->cntxt->lf.lfblk);
    bpush(btact->cntxt->lf.lfpos);
    btact->cntxt->lf.lfblk = cblk;
fin:
    return;
}
