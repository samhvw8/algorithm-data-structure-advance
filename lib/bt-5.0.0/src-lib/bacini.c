/*
 * $Id: bacini.c,v 1.7 2008/05/08 14:34:53 mark Exp $
 *
 *  bacini: Initialise BTA structure, malloc'ing space as required
 *
 *  Parameters:
 *      b - pointer to BT context
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

#include <stdlib.h>
#include <stdio.h>

#include "bc.h"
#include "bt.h"
#include "btree_int.h"

int bacini(BTA *b)
{

    /* get file descriptor for bt file (used for file locking) */
    btact->fd = fileno(btact->idxunt);
    btact->lckcnt = 0;
    
    if ((b->cntrl=(CNTRL *) malloc(sizeof(CNTRL)*ZMXBLK)) == NULL)
        goto err;
    if ((b->memrec=(MEMREC *) malloc(sizeof(MEMREC)*ZMXBLK)) == NULL)
        goto err;
    if ((b->cntxt=(CNTXT *) malloc(sizeof(CNTXT))) == NULL)
        goto err;

    initcntrl(b);
    
    bclrlf();
    bclrst();
    return(0);
err:
    bterr("BACINI",QNOMEM,NULL);
    return(QNOMEM);
}

void initcntrl(BTA *b)
{
    CNTRL *c;
    int i;

    /* initialise lru queue */
    b->cntxt->lru.lrut = -1;
    b->cntxt->lru.lruh = -1;

    /* initialise control data */
    c = b->cntrl;
    for (i=0;i<ZMXBLK;i++) {
        (c+i)->inmem = -1;
        (c+i)->busy = 0;
        (c+i)->writes = 0;
        bqadd(i);
    }
}
