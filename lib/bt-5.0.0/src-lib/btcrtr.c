/*
 * $Id: btcrtr.c,v 1.11 2012/11/15 12:19:37 mark Exp $
 *
 * btcrtr: creates a new root in index file
 *
 * Parameters:
 *   b      pointer to BT context
 *   root   name of root to create
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
#include <string.h>
#include "bc.h"
#include "btree.h"
#include "btree_int.h"

int btcrtr(BTA *b, char *root)
{
    BTint svblk,blk;
    int status;

    bterr("",0,NULL);

    if ((status=bvalap("BTCRTR",b)) != 0) return(status);

    btact = b;

    if (btact->shared) {
        if (!block()) {
            bterr("BTCRTR",QBUSY,NULL);
            goto fin;
        }
    }

    svblk = btact->cntxt->super.scroot;
    /* unbusy current root */
    bsetbs(btact->cntxt->super.scroot,FALSE);
    /* make current root the superroot (where root names are stored) */
    btact->cntxt->super.scroot = ZSUPER;
    bsetbs(btact->cntxt->super.scroot,TRUE);
    status = binsky(b,root,ZNULL);
    btact->cntxt->super.smod++;     /* super root updated */
    bsetbs(btact->cntxt->super.scroot,FALSE);
    bclrlf();
    /* if ok, set up new root, else restore old root */
    if (status == 0) {
        /* get free block for new root */
        blk = bgtfre();
        if (blk == 0) goto fin;
        /* update root block number */
        status = bupdky(b,root,blk);
        if (status != 0 ) goto fin;
        bsetbk(blk,ZROOT,0,ZNULL,0,blk,ZNULL);
        btact->cntxt->super.scroot = blk;
        strncpy(btact->cntxt->super.scclas,root,ZKYLEN);
        btact->cntxt->super.scclas[ZKYLEN-1] = '\0';
        bsetbs(btact->cntxt->super.scroot,TRUE);
    }
    else {
        btact->cntxt->super.scroot = svblk;
        bsetbs(btact->cntxt->super.scroot,TRUE);
    }
fin:
    if (btact->shared) bulock();
    return(btgerr());
}


    
