/*
 * $Id: bdbug.c,v 1.24 2012/11/15 12:19:37 mark Exp $
 *
 * bdbug: write out internal info
 *
 * Parameters:
 *   b      pointer to BT context
 *   cmd    debug command
 *   blkno  block number
 *
 * N.B. bdbug has rather too much knowledge of the btree internals
 *
 * Copyright (C) 2003, 2004, 2010 Mark Willson.
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

int bdbug(BTA * b,char *cmd,BTint blkno)
{
    int idx,ioerr;
    int i,j;
    BTint tblks,tnkeys;
    DATBLK *d;

    bterr("",0,NULL);
    if ((ioerr=bvalap("BDBUG",b)) != 0) return(ioerr);

    if (btact->shared) {
        if (!block()) {
            bterr("BDBUG",QBUSY,NULL);
            goto fin;
        }
    }
    
    btact = b;          /* set context pointer */
    
    if (btact->idxunt == NULL) {
        bterr("BDBUG",QNOBTF,NULL);
        goto fin;
    }
    if (strcmp(cmd,"super") == 0) {
        fprintf(stdout,
            "  Number of blocks: " Z20DFMT "\n"
            "  # free blocks:    " Z20DFMT "\n"
            "  First free:       " Z20DFMT "\n"
            "  Current root blk: " Z20DFMT "\n"
            "  Current root nm:  %20s\n"
            "  Block overhead:   %20u\n"
            "  Block size:       %20u [memrec size: %u]\n"
            "  Keys per block:   %20u\n",
            btact->cntxt->super.sblkmx,
            btact->cntxt->super.snfree,
            btact->cntxt->super.sfreep,
            btact->cntxt->super.scroot,
            btact->cntxt->super.scclas,
                ZPAD,ZBLKSZ,(unsigned int) sizeof(MEMREC),ZMXKEY);
    }
    else if (strcmp(cmd,"control") == 0) {
        fprintf(stdout,"  Index file: %20s\n"
                "  Shared?:    %20d\n"
                "  Last key:   %20s\n"
                "  Last blk:   " Z20DFMT "\n"
                "  Last pos:   %20d\n"
                "  Exact:      %20d\n"
                "  Dup addr:   " Z20DFMT "\n"
                "  LRU head:   %20d\n"
                "  LRU tail:   %20d\n",
                btact->idxfid,btact->shared,btact->cntxt->lf.lfkey,
                btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos,
                btact->cntxt->lf.lfexct,
                btact->cntxt->lf.draddr,
                btact->cntxt->lru.lruh,btact->cntxt->lru.lrut);
        fprintf(stdout,"%10s%20s%10s%10s%10s\n","Mblk","Blk","Busy","Writes",
                "Lrunxt");
        for (i=0;i<ZMXBLK;i++)
            fprintf(stdout,"%10d" Z20DFMT "%10d%10d%10d\n",
                    i,((btact->cntrl)+i)->inmem,
                    ((btact->cntrl)+i)->busy,
                    ((btact->cntrl)+i)->writes,
                    ((btact->cntrl)+i)->lrunxt);
    }
    else if (strcmp(cmd,"stats") == 0) {
        fprintf(stdout,
                "  Logical reads:   %10d\n"
                "  Logical writes:  %10d\n"
                "  Physical reads:  %10d\n"
                "  Physical writes: %10d\n"
                "  Block splits:    %10d\n"
                "  Block joins:     %10d\n"
                "  Block balances:  %10d\n"
                "  Blocks obtained: %10d\n"
                "  Blocks freed:    %10d\n",
                btact->cntxt->stat.xlogrd,btact->cntxt->stat.xlogwr,
                btact->cntxt->stat.xphyrd,btact->cntxt->stat.xphywr,
                btact->cntxt->stat.xsplit,btact->cntxt->stat.xjoin,
                btact->cntxt->stat.xbal,btact->cntxt->stat.xgot,
                btact->cntxt->stat.xrel);
    }
    else if (strcmp(cmd,"space") == 0) {
        tblks = 0; tnkeys = 0; blkno = ZNULL;
        do {
            bnxtbk(&blkno);
            tblks++;
            ioerr = brdblk(blkno,&idx);
            tnkeys += ((btact->memrec)+idx)->infblk[ZNKEYS];
        } while (blkno != btact->cntxt->super.scroot);
        
        fprintf(stdout,
                " No. of key blocks: " Z20DFMT "\n"
                " Max. poss. keys:   " Z20DFMT "\n"
                " Actual keys:       " Z20DFMT "\n"
                " Occupancy (%c):     %20.2f\n",
                tblks, tblks*ZMXKEY,tnkeys,'%',
                (double) tnkeys/(tblks*ZMXKEY)*100.0);
    }
    else if (strcmp(cmd,"block") == 0) {
        ioerr = brdblk(blkno,&idx);
        if (idx >= 0) {
            fprintf(stdout,
                    "%-12s" Z20DFMT "\n"
                    "%-12s" Z20DFMT "\n"
                    "%-12s" Z20DFMT "\n"
                    "%-12s" Z20DFMT "\n"
                    "%-12s" Z20DFMT "\n"
                    "%-12s" Z20DFMT "\n",
                    "Block Type:",bgtinf(blkno,ZBTYPE),
                    "Misc:",bgtinf(blkno,ZMISC),
                    "Nxblk:",bgtinf(blkno,ZNXBLK),
                    "Nkeys:",bgtinf(blkno,ZNKEYS),
                    "Nblks:",bgtinf(blkno,ZNBLKS),
                    "NxDup:",bgtinf(blkno,ZNXDUP));
            switch (bgtinf(blkno,ZBTYPE)) {
                case ZDUP:
                    btdispdups(blkno);
                    break;
                case ZDATA:
                    d = (DATBLK *) (btact->memrec)+idx;
                    bxdump(d->data,ZBLKSZ-(ZINFSZ*ZBPW));
                    break;
                default:
                    fprintf(stdout,"  %32s %20s%20s%20s%20s\n","Key","Val",
                            "Dup","Llink","Rlink"); 
                    for (j=0;j<((btact->memrec)+idx)->infblk[ZNKEYS];j++)
                        fprintf(stdout,"  %32s" Z20DFMT Z20DFMT Z20DFMT Z20DFMT "\n",
                                ((btact->memrec)+idx)->keyblk[j].key,
                                ((btact->memrec)+idx)->keyblk[j].val,
                                ((btact->memrec)+idx)->keyblk[j].dup,
                                ((btact->memrec)+idx)->lnkblk[j],
                                ((btact->memrec)+idx)->lnkblk[j+1]);
            }
        }
        else {
            bterr("BDBUG",QRDBLK,itostr(blkno));
        }
    }
    else if (strcmp(cmd,"stack") == 0) {
        fprintf(stdout,"%10s%20s\n","Level","Contents");
        for (i=0;i<=btact->cntxt->stk.stkptr;i++)
            fprintf(stdout,"%10d" Z20DFMT "\n",i,btact->cntxt->stk.stk[i]);
    }
    else if (strcmp(cmd,"structure") == 0) {
        BTint nkeys = btkeys(b,blkno == ZNULL);
        if (nkeys == ZNULL) {
            fprintf(stdout,"Index structure damaged\n");
        }
        else {
            fprintf(stdout,"Index structure OK (number of keys: " ZINTFMT ")\n",
                    nkeys);
        }
    }
    else {
        bterr("BDBUG",QBADOP,NULL);
    }
  fin:
    if (btact->shared) bulock();
    return(btgerr());
}
