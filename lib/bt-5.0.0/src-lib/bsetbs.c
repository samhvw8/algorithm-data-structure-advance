/*
 * $Id: bsetbs.c,v 1.7 2010/05/26 12:39:16 mark Exp $
 *
 *
  bsetbs: alter busy state of in-memory block

  void bsetbs(int blk,int busy)

     blk    block which requires setting
     busy   new status (TRUE for busy, FALSE for flushable)

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
#include "btree_int.h"

void bsetbs(BTint blk,int busy)
{
    int idx,status;

    status = brdblk(blk,&idx);
    if (idx < 0) {
        bterr("BSETBS",QRDBLK,itostr(blk));
    }
    else {
        ((btact->cntrl)+idx)->busy = busy;
    }
    return;
}
