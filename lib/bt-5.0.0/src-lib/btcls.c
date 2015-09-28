/*
 * $Id: btcls.c,v 1.9 2010/12/31 14:20:52 mark Exp $
 *
 *
 * btcls: close B tree index file
 *
 * Parameters:
 *   b  - pointer to BT context
 *     
 * Returns zero if no errors, error code otherwise
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
#include "btree.h"
#include "btree_int.h"

int btcls(BTA* b)
{
    int ioerr;
  
    bterr("",0,NULL);

    if ((ioerr=bvalap("BTCLS",b)) != 0) return(ioerr);

    btact = b;      /* set context pointer */


    /* Undo any locks and flush buffers before closing file */
    while (btact->lckcnt > 0) bulock();
    
    /* close index file and free context memory */
    ioerr = fclose(btact->idxunt);
    if (ioerr != 0) {
        bterr("BTCLS",QCLSIO,btact->idxfid);
    }
    else {
        btact->idxunt = NULL;
        bacfre(btact);
    }
    return(btgerr());
}

/*
  btsync: sync memory blocks with B tree index file
    
  returns zero if no errors, error code otherwise
*/
int btsync()
{
    int i,ioerr;

    if (btact->idxunt != NULL) {
        /* write out all in-memory blocks */
        for (i=0;i<ZMXBLK;i++) {
            ioerr = bwrblk(((btact->cntrl)+i)->inmem);
            if (ioerr != 0) {
                bterr("BTSYNC",QWRBLK,itostr(((btact->cntrl)+i)->inmem));
                goto fin;
            }
        } 
        /* write super block if necessary */
        if (btact->cntxt->super.smod != 0) {
            if ( bwrsup() != 0) goto fin;
            btact->cntxt->super.smod = 0;
        }
    }

    /* force writes to disk */
    fflush(btact->idxunt);

    /* re-initialise control blocks to ensure blocks will be re-read
       from disk */
    initcntrl(btact);
    
fin:
    return(btgerr());
}


