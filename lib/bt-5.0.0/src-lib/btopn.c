/*
 * $Id: btopn.c,v 1.13 2012/11/15 12:19:37 mark Exp $
 *
 * btopn: opens existing B tree index
 *
 * Parameters:
 *   fid     name of file to open
 *   mode    if ZERO, index file can be updated
 *   shared  set TRUE if index file is to be shared
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
#include "bt.h"
#include "btree.h"
#include "btree_int.h"

BTA *btopn(char *fid, int mode, int shared)
{
    int status;

    bterr("",0,NULL);

    btact = bnewap(fid);
    if (btact == NULL) {
        bterr("BTOPN",QNOACT,NULL);
        return NULL;
    }
    if ((btact->idxunt = fopen(fid,"r+b")) == NULL) {
        bterr("BTOPN",QOPNIO,fid);
        return NULL;
    }
    strncpy(btact->idxfid,fid,FIDSZ);
    btact->idxfid[FIDSZ-1] = '\0';
    if (bacini(btact) != 0) {
        fclose(btact->idxunt);
        goto fin1;
    }
    
    btact->shared = shared;
    btact->cntxt->super.smod = 0;
    btact->cntxt->super.scroot = 0;

    /* always lock file; shared will unlock at routine exit */
    if (!block()) {
        bterr("BTOPN",QBUSY,NULL);
        goto fin;
    }
    /* read in super root */
    if (brdsup() != 0) goto fin;

    /* change to default root */
    status = btchgr(btact,"$$default");
    if (status != 0) goto fin;
    btact->cntxt->super.smode = mode;

    if (btgerr() != 0) 
        goto fin;
    if (shared) bulock();
    return(btact);
fin:
    if (shared) bulock();
fin1:
    bacfre(btact);
    return(NULL);
}



