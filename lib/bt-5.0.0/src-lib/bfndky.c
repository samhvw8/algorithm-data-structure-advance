/*
 * $Id: bfndky.c,v 1.17 2012/11/15 12:19:37 mark Exp $
 *
 * bfndky: finds key in index
 *
 * Parameters:
 *    b      index file context pointer
 *    key    key to search for
 *    val    returned with key value, if key found
 *    
 * index is left positioned at next key (for use by bnxtky)
 *  
 * bfndky returns 0 for no errors, error code otherwise
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
#include "btree.h"
#include "btree_int.h"

/* #undef DEBUG */
/* #define DEBUG 2 */

int bfndky(BTA *b,char *key,BTint *val)
{
    BTint cblk, link1, link2, newblk;
    int index, result, nkeys, status;
    char lkey[ZKYLEN];
    BTint duplicates_allowed;
        
    bterr("",0,NULL);
    status = QNOKEY;
    if ((result=bvalap("BFNDKY",b)) != 0) return(result);

    btact = b;      /* set context pointer */

    if (btact->shared) {
        if (!block()) {
            bterr("BFNDKY",QBUSY,NULL);
            goto fin;
        }
    }

    /* take local copy of key, truncating if necessary */
    strncpy(lkey,key,ZKYLEN);
    lkey[ZKYLEN-1] = '\0';
    
    /* initialise stack etc */
    cblk = btact->cntxt->super.scroot;
    duplicates_allowed = bgtinf(cblk,ZMISC);
    bstkin();
    bclrlf();
    strncpy(btact->cntxt->lf.lfkey,lkey,ZKYLEN);
    btact->cntxt->lf.lfkey[ZKYLEN-1] = '\0';

    while (cblk != ZNULL) {
#if DEBUG >= 2
        fprintf(stderr,"BFNDKY: searching block " ZINTFMT "\n",cblk);
#endif      
        nkeys = bgtinf(cblk,ZNKEYS);
        if (nkeys == ZMXKEY && btact->cntxt->super.smode == 0) {
            /* split if block full and updating permitted */
            bsptbk(cblk,&newblk);
            if (newblk < 0) {
                bterr("BFNDKY",QSPLIT,NULL);
                break;
            }
            /* if split occured, then must re-examine parent */
            if (cblk != btact->cntxt->super.scroot) {
                index  = btact->cntxt->lf.lfpos;
                cblk = btact->cntxt->lf.lfblk;
                btact->cntxt->lf.lfpos = bpull();
                btact->cntxt->lf.lfblk = bpull();
            }
        }
        else {
            index = -1;
            bpush(btact->cntxt->lf.lfblk);
            bpush(btact->cntxt->lf.lfpos);
            bsrhbk(cblk,lkey,&index,val,&link1,&link2,&result);
            btact->cntxt->lf.lfblk = cblk;
            btact->cntxt->lf.lfpos = index; /* if block is empty, leave lfpos at -1 */
            if (result < 0) {
                /* must examine left block */
                cblk = link1;
            }
            else if (result > 0) {
                /* must examine right block */
                cblk = link2;
                /* increment index to indicate this key "visited" */
                btact->cntxt->lf.lfpos++;
            }
            else {
                status = 0;
                btact->cntxt->lf.lfexct = TRUE;
                cblk = ZNULL;
                btduppos(ZNEXT,val);
            }
        }
    }
  fin:
    if (btact->shared) bulock();
    /* non-zero status indicates no such key found */
    if (status) bterr("BFNDKY",QNOKEY,lkey);
    return(btgerr());
}

