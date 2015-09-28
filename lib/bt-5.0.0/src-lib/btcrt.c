/*
 * $Id: btcrt.c,v 1.14 2012/11/15 12:19:37 mark Exp $
 *
 *
 * btcrt:  create B tree index file
 *
 * Parameters:
 *   fid    holds filename of index file to create
 *   nkeys  maximum number of keys required (0
 *          if random files can be grown dynamically)
 *   shared FALSE for exclusive index file access
 *          TRUE for shared access
 *               
 * Returns null if no errors, index context handle otherwise
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "bc.h"
#include "bt.h"
#include "btree.h"
#include "btree_int.h"

#define DEFAULT_ROOT 1

BTA *btcrt(char *fid, int nkeys,int shared)
{
    BTint nblks;
    int idx,ioerr,i;
    
    bterr("",0,NULL);

    btact = bnewap(fid);
    if (btact == NULL) {
        bterr("BTCRT",QNOACT,NULL);
        return NULL;
    }

    if ((btact->idxunt = fopen(fid,"w+b")) == NULL) {
        bterr("BTCRT",QCRTIO,fid);
        return NULL;
    }

    btact->shared = shared;
    strncpy(btact->idxfid,fid,FIDSZ);
    btact->idxfid[FIDSZ-1] = '\0';

    /* initialise bt context areas */
    if (bacini(btact) != 0) 
        return NULL;
    
    /* set current root to ZNULL to indicate creation in progress */
    btact->cntxt->super.scroot = ZNULL;

    /* unconditional lock newly created file */
    if (!block()) {
        bterr("BTCRT",QBUSY,NULL);
        goto fin1;
    }


    btact->cntxt->super.sblkmx = 2;     /* include super and default roots */
    btact->cntxt->super.snfree = 0;     /* count of blocks in free list */
    btact->cntxt->super.sfreep = ZNULL; /* head of free list */
    btact->cntxt->super.smod = 1;       /* ensure block is written */
    btact->cntxt->super.smode = 0;      /* mark index as write-enabled */

    /* set up super root */
    idx = bgtslt();
    ((btact->cntrl)+idx)->inmem = ZSUPER;
    bsetbk((BTint) ZSUPER,(BTint) ZROOT,btact->cntxt->super.snfree,
        btact->cntxt->super.sfreep,2,
           btact->cntxt->super.sblkmx,ZNULL);
    
    strcpy(((btact->memrec)+idx)->keyblk[1].key,"$$super");
    ((btact->memrec)+idx)->keyblk[1].val = ZSUPER;
    ((btact->memrec)+idx)->keyblk[1].dup = ZNULL;
    strcpy(((btact->memrec)+idx)->keyblk[0].key,"$$default");
    ((btact->memrec)+idx)->keyblk[0].val = DEFAULT_ROOT;
    ((btact->memrec)+idx)->keyblk[0].dup = ZNULL;
    for (i=0;i<=2;i++) ((btact->memrec)+idx)->lnkblk[i] = ZNULL;

    /* write it out */
    ioerr = bwrblk(ZSUPER);
    if (ioerr != 0) {
        bterr("BTCRT",QWRSUP,itostr(ZSUPER));
        goto fin;
    }

    /* set up default root */
    idx = bgtslt();
    ((btact->cntrl)+idx)->inmem = 1;
    btact->cntxt->super.scroot = 1;
    strcpy(btact->cntxt->super.scclas,"$$default");
    bsetbk(1,ZROOT,0,ZNULL,0,DEFAULT_ROOT,ZNULL);
    ioerr = bwrblk(DEFAULT_ROOT);
    if (ioerr != 0) {
        bterr("BTCRT",QWRBLK,itostr(1));
        goto fin;
    }
    ioerr = brdblk(DEFAULT_ROOT,&idx);
    if (ioerr != 0) {
        bterr("BTCRT",QRDBLK,itostr(1));
        goto fin;
    }
    bsetbs(DEFAULT_ROOT,TRUE);
    /* initialise free list if required */
    if (nkeys != 0) {
        idx = bgtslt();
        nblks = nkeys/ZMXKEY;
        for (i=2;i<nblks;i++) {
            ((btact->cntrl)+idx)->inmem = i;
            bmkfre(i); 
            btact->cntxt->super.sblkmx++;
        }
    }
    if (shared) bulock();   /* allow access in shared mode */
    if (btgerr() != 0 )
        return(NULL);
    else 
        return(btact);

    /* error encountered - unlock and close file */
fin:
    if (shared) bulock();
    fclose(btact->idxunt);
fin1:
    /* free memory */
    bacfre(btact);
    return(NULL);
}
