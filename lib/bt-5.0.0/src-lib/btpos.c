/*
 * $Id: btpos.c,v 1.6 2012/11/15 12:19:37 mark Exp $
 *
 * btpos: Positions index to beginning or end of whole file or
 *        duplicate key section.
 *
 * Parameters:
 *    b      index file context pointer
 *    pos    ZSTART or ZEND
 *    
 * When btpos returns (with no error), with an argument of ZSTART,
 * bnxtky will return the first key in the index (or the first key in
 * the duplicate section).  With ZEND, bprvky will return the last key in
 * the index (or the last key of the duplicate section).
 *  
 * Returns zero if no errors, error code otherwise
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
#include "btree.h"
#include "btree_int.h"

int btpos(BTA *b,int pos)
{
    int status;
    char key[ZKYLEN];
    BTint val, link1, link2;
    
    
    
    bterr("",0,NULL);
    if ((status=bvalap("BTPOS",b)) != 0) return(status);

    btact = b;      /* set context pointer */

    if (btact->shared) {
        if (!block()) {
            bterr("BTPOS",QBUSY,NULL);
            goto fin;
        }
    }

    /* initialise context */
    bclrlf();
    bstkin();
    bpush(ZNULL);
    bpush(ZNULL);
    btact->cntxt->lf.lfblk = btact->cntxt->super.scroot;
    if (pos == ZSTART) {
        btact->cntxt->lf.lfkey[0] = '\0'; /* set lfkey for shared                                              mode */
        btact->cntxt->lf.lfpos = 0;
        bleaf(0);
    }
    else if (pos == ZEND) {
        int loc;
        btact->cntxt->lf.lfpos = bgtinf(btact->cntxt->super.scroot,ZNKEYS);
        if (btact->cntxt->lf.lfpos == 0) return 0;
        bleaf(1);
        /* set lfkey for shared mode */
        loc = btact->cntxt->lf.lfpos-1;
        bsrhbk(btact->cntxt->lf.lfblk,key,&loc,&val,
               &link1,&link2,&status);
        if (status < 0) {
            bterr("BTPOS",QPOSERR,itostr(btact->cntxt->lf.lfblk));
        }
        else {
            key[strlen(key)-1]++;   /* make lfkey greater than last key */
            strncpy(btact->cntxt->lf.lfkey,key,ZKYLEN);
            btact->cntxt->lf.lfkey[ZKYLEN-1] = '\0';
        }
    }
 
    if (btact->shared) bulock();
    
  fin:
    return btgerr();
}

