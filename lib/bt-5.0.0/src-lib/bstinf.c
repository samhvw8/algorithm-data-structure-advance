/*
 * $Id: bstinf.c,v 1.13 2011/06/24 10:13:55 mark Exp $
 *
 *
 *  bstinf: set information about block
 *
 *  int bstinf(int blk,int type,int val)
 *
 *     blk    block for which information must be set
 *     type   type of information to set
 *     val    info value
 *
 * NOTE: The munging performed by bgtinf (and bstinf) to detect LFS vs
 * non-LFS will only only work little-endian architectures.
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
#include "btree_int.h"
#include <math.h>

int bstinf(BTint blk,int type,BTint val)
{
    int ioerr,idx;

    if (type >= ZINFSZ || type < 0)
        bterr("BSTINF",QINFER,NULL);
    else {
        ioerr = brdblk(blk,&idx);
        if (idx < 0) {
            bterr("BSTINF",QRDBLK,itostr(blk));
        }
        else {
            switch (type) {
                case ZBTYPE:
                    ((btact->memrec)+idx)->infblk[type] =
                        (((BTint) ZVERS) << (2*ZBYTEW)) |
                        val;
#ifdef _FILE_OFFSET_BITS
                    /* add 64bit file marker */
                    ((btact->memrec)+idx)->infblk[type] |=
                        (((BTint) LFSHDR) << 32);
#endif                        
                case ZBTVER:
                    break; /* always set implicitly by ZBTYPE */
                default:
                    ((btact->memrec)+idx)->infblk[type] = val;
            }
            ((btact->cntrl)+idx)->writes++;
        }
    }
    return(0);
}


