/*
 * $Id: bdelky.c,v 1.10 2012/10/18 09:25:51 mark Exp $
 *
 * bdelky:  deletes key in index
 *
 * Parameters:
 *     b       index context pointer
 *     key     name of key to delete 
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

#include "bc.h"
#include "bt.h"
#include "btree.h"
#include "btree_int.h"

int bdelky(BTA *b,char *key)
{

    BTint val;
    int status = 0;

    bterr("",0,NULL);
    if ((status=bvalap("BDELKY",b)) != 0) return(status);
    btact = b;

     if (b->cntxt->super.smode != 0) {
        bterr("BDELKY",QNOWRT,NULL);
    }
    else {
        if (btact->shared && !block()) {
            bterr("BDELKY",QBUSY,NULL);
            goto fin;
        }
        if (key != NULL) status = bfndky(b,key,&val);
        if (status == 0) {
            status = bdelk1(key);
            /* invalidate context on successful deletion */
            /* if (status == 0) bclrlf(); new dup handling means
             * context should be OK*/
        }
    }
fin:
    if (btact->shared) bulock();
    return(btgerr());
}
