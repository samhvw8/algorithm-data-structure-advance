/*
 * $Id: bdelk1.c,v 1.21 2012/11/19 15:21:28 mark Exp $
 *
 *
 * bdelk1:  deletes key in index (does the real work)
 *
 * Parameters:
 *    key    name of key to delete
 *
 *  bdelky returns 0 on succesful deletion, error-code otherwise
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

#include "btree.h"
#include "btree_int.h"

int bdelk1(char *key)
{
    BTint cblk,link1,link2,llink,rlink,val,blk;
    int result,pos,type,status;
    char tkey[ZKYLEN];
    KEYENT * kep;

    /* TBD: Why was this check commented out? */
    if (!btact->cntxt->lf.lfexct) {
        bterr("BDELK1",QDELEX,NULL);
        goto fin;
    }
    
    /* handle duplicate keys */       
    if (btdeldup() != ZNULL) {
        goto fin;
    }

    pos = btact->cntxt->lf.lfpos;
    blk = btact->cntxt->lf.lfblk;
    bsrhbk(blk,tkey,&pos,&val,&llink,&rlink,&result);
    if (key != NULL) {
        if (result != 0 || strcmp(tkey,key) != 0) {
            bterr("BDELK1",QDELER,NULL);
            goto fin;
        }
    }
    
    if (llink != ZNULL) {
        /* key not in leaf block, get rightmost leaf key to replace
         * deleted key */
        bleaf(1);
        btact->cntxt->lf.lfpos--; /* decrement pos to last key */
#if DEBUG >= 2
        printf("BDELK1: After bleaf() lfblk: " ZINTFMT ", lfpos: %d\n",
               btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos);
#endif
        kep = bsrhbk(btact->cntxt->lf.lfblk,tkey,&(btact->cntxt->lf.lfpos),
                     &val,&link1,&link2,&result);
        if (result != 0) {
            bterr("BDELK1",QDELRP,itostr(btact->cntxt->lf.lfblk));
            goto fin;
        }
        brepky(blk,pos,kep,llink,rlink);
        llink = ZNULL;
    }
    bremky(btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos);
#if DEBUG >= 2
    printf("BDELK1: After bremky; lfblk: " ZINTFMT ", lfpos: %d\n",
           btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos);
#endif

    type = bgtinf(btact->cntxt->lf.lfblk,ZBTYPE);
    if (type != ZROOT) {
        /* deleted key is in leaf block (one way or the other); try to
         * join adjacent blocks, so one can be freed */
        bjnblk(&cblk);
        if (cblk == ZNULL) {
            /* couldn't join, see if balancing required */
            balblk();
        }
        /* if blocks joined, check parent.  If empty, must demote its
         * parent key */
        while (cblk != ZNULL) {
            bdemte(&cblk);
        }
    }
    /* due to the potential mangling of the block structure when
     * deleting a key, need to re-establish context for next/prev key, if
     * exclusive access (only when no previous error exists) */
    if (!btact->shared && btgerr() == 0) {
        status = bfndky(btact,btact->cntxt->lf.lfkey,&val);
        if (status == QNOKEY) bterr("",0,NULL);
    }
    
fin:
    return btgerr();
}
