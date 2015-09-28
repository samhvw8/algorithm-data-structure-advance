/*
 * $Id: bjoin.c,v 1.8 2012/11/05 10:34:58 mark Exp $
 *
 * bjoin: bjoins keys in rblk to lblk (using tkey)
 *
 * Parameters:
 *   lblk   left block of joining pair
 *   rblk   right block of joining pair
 *   tkey   name of parent key
 *   val    value of parent key
 *
 * Parent block of left and right siblings is in last found context.
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

void bjoin(BTint lblk,BTint rblk,KEYENT* kep)
{
    int lnkeys,rnkeys;

#if DEBUG >= 1
    printf("BJOIN: joining lblk: " ZINTFMT ", rblk: " ZINTFMT
           ", using parent: %s\n",lblk,rblk,tkey);
#endif
    bremky(btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos);
    bsetbs(lblk,1);
    bputky(lblk,kep,ZNULL,ZNULL); 
    lnkeys = bgtinf(lblk,ZNKEYS);
    rnkeys = bgtinf(rblk,ZNKEYS);
    bcpybk(lblk,rblk,lnkeys,0,rnkeys); 
    bstinf(lblk,ZNKEYS,lnkeys+rnkeys);
    bmkfre(rblk);
    bsetbs(lblk,0);
    btact->cntxt->stat.xjoin++;
    return;
}
