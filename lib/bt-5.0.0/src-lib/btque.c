/*
 * $Id: btque.c,v 1.7 2007/04/13 10:00:24 mark Exp $
 *
 * B tree routines to handle least recently used queue for blocks
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

void bqadd(int idx)
{
    if (btact->cntxt->lru.lruh < 0) 
        btact->cntxt->lru.lruh = idx;
    else
        ((btact->cntrl)+(btact->cntxt->lru.lrut))->lrunxt = idx;
        
    btact->cntxt->lru.lrut = idx;
    ((btact->cntrl)+idx)->lrunxt = -1;
#if DEBUG >= 2
    printf("bqadd: lrut: %d, lruh: %d\n",btact->cntxt->lru.lrut,
        btact->cntxt->lru.lruh);
#endif
}

int bqhead()
{
    int idx;

    idx = btact->cntxt->lru.lruh;
    while (idx >= 0) {
        if (((btact->cntrl)+idx)->busy) {
            bqmove(idx); /* move busy block to lrut */
            idx = btact->cntxt->lru.lruh;  /* get new head of lru queue */
        }
        else
            break;
    }
    return (idx);
}

int bqmove(int idx)
{
    int i;
    
#if DEBUG >= 4
    printf("Before move of idx %d:  lruh: %d, lrut: %d\n",idx,btact->cntxt->lru.lruh,btact->cntxt->lru.lrut);
    for (i=0;i<ZMXBLK;i++) printf("lrunxt[%d] = %d  ",i,((btact->cntrl)+i)->lrunxt);
    printf("\n");
#endif
    if (idx < 0) {
        bterr("BQMOVE",QBADIX,itostr(idx));
        goto fin;
    }
    if (idx == btact->cntxt->lru.lrut) return(0);
     
    if (idx == btact->cntxt->lru.lruh)
        btact->cntxt->lru.lruh = ((btact->cntrl)+idx)->lrunxt;
    else {
        i = btact->cntxt->lru.lruh;
        while (i>=0) {
            if (((btact->cntrl)+i)->lrunxt == idx) break;
            i = ((btact->cntrl)+i)->lrunxt;
        }
        if (i >= 0)
            ((btact->cntrl)+i)->lrunxt = ((btact->cntrl)+idx)->lrunxt;
        else {
            bterr("BQMOVE",QLRUER,itostr(idx));
        }
    }
    ((btact->cntrl)+btact->cntxt->lru.lrut)->lrunxt = idx;
    ((btact->cntrl)+idx)->lrunxt = -1;
    btact->cntxt->lru.lrut = idx;
#if DEBUG >= 4
    printf("After move:  lruh: %d, lrut: %d\n",btact->cntxt->lru.lruh,
           btact->cntxt->lru.lrut);
    for (i=0;i<ZMXBLK;i++) printf("lrunxt[%d] = %d  ",i,
                                  ((btact->cntrl)+i)->lrunxt);
    printf("\n");
#endif
fin:
    return(0);
}

