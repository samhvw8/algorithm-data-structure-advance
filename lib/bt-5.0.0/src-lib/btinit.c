/*
 * $Id: btinit.c,v 1.9 2011/06/13 19:34:46 mark Exp $
 *
 * btinit: initialise B tree tables 
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

/*  Declare context structure for tracking index usage */
BTA btat[ZMXACT];

/* Pointer to active index info */
BTA *btact = NULL;

int btinit()
{
    int i,cnt=0,blksz;
    static int btinited = FALSE;

    if (btinited) {
        bterr("BTINIT",QINERR,NULL);
        return(QINERR);
    }

    /* ensure ZBLKSZ is a power of 2 */
    blksz = ZBLKSZ;
    for (i=0;i<(ZBYTEW*sizeof(int));i++) {
        if ((blksz & 0x1) == 1) cnt++;
        blksz = blksz>>1;
    }
    if (cnt != 1) {
        bterr("BTINIT",QBLKSZERR,itostr(ZBLKSZ));
        return(QBLKSZERR);
    }
    
    for (i=0;i<ZMXACT;i++) {
        btat[i].idxunt = NULL;
        btat[i].idxfid[0] = '\0';
        btat[i].cntrl = NULL;
        btat[i].memrec = NULL;
        btat[i].cntxt = NULL;
        btat[i].fd = -1;
        btat[i].wt_threshold = 0;
    }

    btinited = TRUE;
    return(0);
}
