/*
 * $Id: bvalap.c,v 1.6 2010/05/26 12:39:16 mark Exp $
 *
 * bvalap -  Validates the index context pointer passed
 *
 * Parameters:
 *   fn  - calling routine (for proxy error message)
 *   b   - index context pointer
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

int bvalap(char *fn,BTA *b)
{
    if (b < btat || b >= btat+ZMXACT) {
        bterr(fn,QBADAP,0);
        return(QBADAP);
    }
    if (b->idxunt == NULL) {
        bterr(fn,QBADAP,0);
        return(QBADAP);
    }
    return(0);
}
