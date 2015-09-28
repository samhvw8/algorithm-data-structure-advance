/*
 * $Id: bsptbk.c,v 1.12 2012/11/15 12:19:37 mark Exp $
 *
 *
  bsptbk: splits block into two

  int bsptbk(int blk,int *newblk)

    blk    block number of block to split
    newblk holds new block number, or ZNULL if split failed

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

#include <string.h>

#include "bc.h"
#include "bt.h"
#include "btree_int.h"

int bsptbk(BTint blk,BTint *newblk)
{
    BTint val,link1,link2,blk1,blk2,pblk;
    int sp,sp2,type;
    int result = 0;
    char tkey[ZKYLEN];
    KEYENT* kep;
    
    *newblk = ZNULL;
    tkey[0] = '\0';
    type = bgtinf(blk,ZBTYPE);
#if DEBUG > 0
    printf("BSPTBK: splitting block " ZINTFMT " of type %d\n",blk,type);
#endif    
    sp = ZMXKEY/2;
    sp2 = ZMXKEY%2==0?sp-1:sp;
    if (type == ZROOT) {
        /* splitting root, so need two free blocks */
        blk1 = bgtfre();
        if (blk1 < 0) return(ZNULL);
        blk2 = bgtfre();
        if (blk2 < 0) return(ZNULL);
        /* copy first half of root to first block */
        bcpybk(blk1,blk,0,0,sp);
        /* copy second half of root to second block */
        bcpybk(blk2,blk,0,sp+1,sp2);
        bsetbk(blk1,ZINUSE,0,blk,sp,btact->cntxt->super.scroot,ZNULL);
        bsetbk(blk2,ZINUSE,0,blk,sp2,btact->cntxt->super.scroot,ZNULL);
        /* find middle key */
        kep = bsrhbk(blk,tkey,&sp,&val,&link1,&link2,&result);
        if (result != 0) {
            bterr("BSPTBK",QSPKEY,NULL);
            goto fin;
        }
        /* insert at first position in root */
        brepky(blk,0,kep,blk1,blk2);
        bstinf(blk,ZNKEYS,1);
        *newblk = blk1;
#if DEBUG >= 2
        fprintf(stderr,"BSPTBK: block " ZINTFMT ", split into " ZINTFMT
                " and " ZINTFMT "\n",
                blk,blk1,blk2);
#endif      
    }
    else {
        /* splitting non-root, so need one block */
        bsetbs(blk,1);
        *newblk = bgtfre();
        if (*newblk < 0) return (ZNULL);
        /* copy second half of block to new block */
        bcpybk(*newblk,blk,0,sp+1,sp2);
        /* get parent block from last found block */
        pblk = btact->cntxt->lf.lfblk;
        /* set info for new block */
        bsetbk(*newblk,ZINUSE,0,pblk,sp2,btact->cntxt->super.scroot,ZNULL);
        /* find middle key */
        kep = bsrhbk(blk,tkey,&sp,&val,&link1,&link2,&result);
#if DEBUG >= 2
        fprintf(stderr,"Promoted key: %s into parent block %d\n",tkey,pblk);
#endif      
        if (result != 0) {
            bterr("BSPTBK",QSPKEY,NULL);
            goto fin;
        }
        /* truncate old block */
        bstinf(blk,ZNKEYS,sp);
        /* insert middle key into parent block */
        bputky(pblk,kep,blk,*newblk);
        bsetbs(blk,0);
    }
    btact->cntxt->stat.xsplit++;
fin:
    return (result==0?0:ZNULL);
}
