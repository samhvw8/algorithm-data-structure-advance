/*
 * $Id: bprvky.c,v 1.11 2012/11/15 12:19:37 mark Exp $
 *
 * bprvky:  returns previous key from index
 *
 * Parameters:
 *   b       index file context pointer
 *   key     returned with previous key
 *   val     returned with value of key
 *
 * bprvky returns non-ZERO if an error occurred
 *
 * Copyright (C) 2010, Mark Willson.
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
#include "btree.h"

/* #undef DEBUG */
/* #define DEBUG 1 */

int bprvky(BTA* b,char *key,BTint *val)
{
    int idx,nkeys,status;
    int found;
    
    bterr("",0,NULL);
    if ((status=bvalap("BNXTKY",b)) != 0) return(status);

    btact = b;          /* set global context pointer */

    if (btact->shared) {
        if (!block()) {
            bterr("BPRVKY",QBUSY,NULL);
            goto fin;
        }
    }
    /* handle duplicate positioning */
    found = btduppos(ZPREV,val);
    if (found > 0) {
        goto fin;
    }
    else if (found == 0) {
        strncpy(key,btact->cntxt->lf.lfkey,ZKYLEN);
        key[ZKYLEN-1] = '\0';
        goto fin;
    }
   
    if (btact->shared && btact->cntxt->lf.lfblk != ZNULL) {
        /* position to last found key via bfndky, since context could
         * have been invalidated by concurrent updates by other users.
         * Note we don't care if the key is found or not, so the error
         * status is always cleared. */
        status = bfndky(btact,btact->cntxt->lf.lfkey,val);
        bterr("",0,NULL);
    }
    
    found = FALSE;
    while (btact->cntxt->lf.lfblk != ZNULL && !found) {
        status = brdblk(btact->cntxt->lf.lfblk,&idx);
        if (idx < 0) {
            bterr("BPRVKY",QRDBLK,itostr(btact->cntxt->lf.lfblk));
            break;
        }
        nkeys = bgtinf(btact->cntxt->lf.lfblk,ZNKEYS);
#if DEBUG >= 1
        printf("BPRVKY: lfblk: " ZINTFMT ", lfpos: %d, lexct: %d, nkeys: %d\n",
               btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos,
               btact->cntxt->lf.lfexct,nkeys);
#endif
        if (btact->cntxt->lf.lfpos < 0 || nkeys == 0) {
            /* finished with this block (or no key was found at all),
             * get parent from stack */
            btact->cntxt->lf.lfpos = bpull()-1;
            btact->cntxt->lf.lfblk = bpull();
            btact->cntxt->lf.lfexct = FALSE; 
            /* invalidate existing dup chain */
            btact->cntxt->lf.draddr = ZNULL; 
            continue;
        }

        if (btact->cntxt->lf.lfpos == nkeys) {
            btact->cntxt->lf.lfpos--;
            btact->cntxt->lf.lfexct = TRUE;    
            /* invalidate existing dup chain */
            btact->cntxt->lf.draddr = ZNULL; 
        }
        else {
            if (btact->cntxt->lf.lfexct) {
                idx = bleaf(1); 
                if (idx < 0) continue;
                btact->cntxt->lf.lfpos--;
                /* invalidate existing dup chain */
                btact->cntxt->lf.draddr = ZNULL; 
            }
            else if (((btact->memrec)+idx)->lnkblk[btact->cntxt->lf.lfpos]
                     == ZNULL) {
                /* lfexct ignored for leaf block, otherwise
                 * positioning doesn't work properly. */
                btact->cntxt->lf.lfpos--;
                btact->cntxt->lf.lfexct = TRUE;                
                /* invalidate existing dup chain */
                btact->cntxt->lf.draddr = ZNULL; 
            }
            else {
                btact->cntxt->lf.lfexct = TRUE;
            }
        }
#if DEBUG >= 1
        printf("BPRVKY(2): lfblk: " ZINTFMT
               ", lfpos: %d, lexct: %d, nkeys: %d\n", 
               btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos,
               btact->cntxt->lf.lfexct,nkeys);
#endif  
        if (btact->cntxt->lf.lfpos >= 0) {
            found = TRUE;
            strncpy(key,
                   ((btact->memrec)+idx)->keyblk[btact->cntxt->lf.lfpos].key,
                   ZKYLEN);
            key[ZKYLEN-1] = '\0';
            /* remember found key (need for shared mode) */
            strncpy(btact->cntxt->lf.lfkey,key,ZKYLEN);
            btact->cntxt->lf.lfkey[ZKYLEN-1] = '\0';
            *val = ((btact->memrec)+idx)->keyblk[btact->cntxt->lf.lfpos].val;
            btduppos(ZPREV,val);
        }
    }
    if (btact->cntxt->lf.lfblk == ZNULL) {
        /* end of index reached */
        bterr("BPRVKY",QNOKEY,NULL);
    }
fin:
    if (btact->shared) bulock();
    return(btgerr());
}
