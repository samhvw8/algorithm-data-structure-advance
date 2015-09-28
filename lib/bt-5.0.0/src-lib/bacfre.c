/*
 * $Id: bacfre.c,v 1.6 2010/05/26 12:39:16 mark Exp $
 *
 *    bacfre: Frees malloc'd memory acquired by bacini for index context
 *
 *    Parameters:
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

#include "bc.h"
#include "bt.h"

void bacfre(BTA *b)
{
    if (b != NULL) {
        b->idxfid[0] = '\0';
        b->idxunt = NULL;
        if (b->memrec != NULL) free(b->memrec);
        if (b->cntrl != NULL) free(b->cntrl);
        if (b->cntxt != NULL) free(b->cntxt);
    }
}

