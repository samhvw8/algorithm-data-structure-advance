/*
 * $Id: bdemte.c,v 1.10 2012/11/05 10:34:58 mark Exp $
 *
 * bdemte: if non-leaf block is empty, demote parent key
 *
 * Parameters:
 *   cblk   returned potentially dangling block (i.e its parent has 
 *          no keys).  ZNULL if no dangling block.
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

void bdemte(BTint *cblk)
{
    BTint newblk,val,link1,link2,jblk,llink,rlink;
    int nkeys,type,result;
    int rblke;
    char tkey[ZKYLEN];
    KEYENT* kep;
    
    nkeys = bgtinf(btact->cntxt->lf.lfblk,ZNKEYS);
    if (nkeys == 0) {
        type = bgtinf(btact->cntxt->lf.lfblk,ZBTYPE);
        if (type != ZROOT) {
            /* pull parent */
            btact->cntxt->lf.lfpos = bpull();
            btact->cntxt->lf.lfblk = bpull();
            nkeys = bgtinf(btact->cntxt->lf.lfblk,ZNKEYS);
            if (btact->cntxt->lf.lfpos != 0) { 
                /* ensure that empty block is on the right */
                rblke = TRUE;
                btact->cntxt->lf.lfpos--;
            }
            else {
                /* first block empty, so can't put on right */
                rblke = FALSE;
            }
            kep = bsrhbk(btact->cntxt->lf.lfblk,tkey,&btact->cntxt->lf.lfpos,
                         &val, &link1,&link2,&result);
            if (result != 0) {
                bterr("BDEMTE",QDEMSE,NULL);
                goto fin;
            }
#if DEBUG >= 1  
            printf("BDEMTE: Demoting key %s from blk: " ZINTFMT
                   " using orphaned blk " ZINTFMT "\n",tkey,
                   btact->cntxt->lf.lfblk,*cblk);
#endif
            /* remove key from original block */
            bremky(btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos);
            /* prepare for split check */
            if (rblke) 
                jblk = link1;
            else
                jblk = link2;
        
            /* if target block for demotion is full, split it */
            nkeys = bgtinf(jblk,ZNKEYS);
            if (nkeys == ZMXKEY) {
                bsptbk(jblk,&newblk);
                if (newblk == ZNULL) {
                    bterr("BDEMTE",QDEMSP,NULL);
                    goto fin;
                }
                if (rblke) {
                    link1 = newblk;
                }
            }
            if (!rblke) {
                /* copy keys from right block to empty left block */
                nkeys = bgtinf(link2,ZNKEYS);
                bsetbs(link1,1);
                bcpybk(link1,link2,0,0,nkeys);
                bstinf(link1,ZNKEYS,nkeys);
                bsetbs(link1,0);
                llink = *cblk;
                rlink = ZNULL; /* 0 replaced by ZNULL for C */
            }
            else {
                llink = ZNULL;  /* 0 replaced by ZNULL for C */
                rlink = *cblk;
            }
            bputky(link1,kep,llink,rlink);
            bmkfre(link2);
            /* set new potentially dangling block */
            *cblk = link1;
        }
        else { 
            /* empty block is root; copy keys from child and then free
             * the child block */
            nkeys = bgtinf(*cblk,ZNKEYS);
            bcpybk(btact->cntxt->lf.lfblk,*cblk,0,0,nkeys);
            bstinf(btact->cntxt->lf.lfblk,ZNKEYS,nkeys);
            bmkfre(*cblk);
            *cblk = ZNULL;
        }
    }
    else {
        *cblk = ZNULL;
    }
fin:
    return;
}
