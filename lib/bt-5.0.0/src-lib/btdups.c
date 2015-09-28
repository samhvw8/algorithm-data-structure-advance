/*
 * $Id: btdups.c,v 1.2 2012/06/16 19:39:43 mark Exp $
 *
 * btdups: 
 *
 * Parameters:
 *   b      pointer to BT context
 *   dups   TRUE for dups allowed for curent root, FALSE
 *          for disallowed.
 *
 * Returns zero if no errors, error code otherwise
 *
 * Copyright (C) 2003, 2004. 2010 Mark Willson.
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

#include "btree.h"
#include "btree_int.h"

int btdups(BTA* b, BTint dups)
{
    int status;
    
    bterr("",0,NULL);

    if ((status=bvalap("BTDUPS",b)) != 0) return(status);

    btact = b;

    if (btact->cntxt->super.scroot == ZSUPER) {
        bterr("BTDUPS",QNODUPS,NULL);
        goto fin;
    }
    
    if (btact->shared) {
        if (!block()) {
            bterr("BTDUPS",QBUSY,NULL);
            goto fin;
        }
    }
    bstinf(btact->cntxt->super.scroot,ZMISC,dups);
    if (btact->shared) bulock();
  fin:
    return btgerr();
}
