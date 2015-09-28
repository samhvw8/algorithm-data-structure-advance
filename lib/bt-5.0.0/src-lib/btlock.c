/*
 * $Id: btlock.c,v 1.8 2010/12/31 14:20:52 mark Exp $
 *
 * btlock: locks index file for exclusive use
 *
 * Parameters:
 *   b      index file context pointer
 *    
 * btlock returns 0 for no errors, error code otherwise
 *
 * btunlock: unlocks index file
 * 
 * Parameters:
 *   b      index file context pointer
 *
 * btunlock returns 0 for no errors, error code otherwise
 *
 * NB  btlock may be called multiple times, but for each call, a
 *     corresponding call to btunlock must be made.
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

int btlock(BTA *b)
{
    int result;
    
    bterr("",0,NULL);

    if ((result=bvalap("BTLOCK",b)) != 0) return(result);

    btact = b;      /* set context pointer */
    if (btact->shared) {
        /* only lock if shared */
        if (!block()) {
            bterr("BTLOCK",QBUSY,NULL);
        }
        else {
            /* invalidate context; app must re-establish context while
               index is locked */
            bclrlf();
        }
    }
    return(btgerr());
}

/*

  btunlock: unlocks index file 

  int btunlock(BTA *b)

     b      index file context pointer
     
   btunlock returns 0 
*/

int btunlock(BTA *b)
{
    int result;
    
    bterr("",0,NULL);

    if ((result=bvalap("BTUNLOCK",b)) != 0) return(result);

    btact = b;      /* set context pointer */
    if (btact->shared) {
        bulock();
    }
    return(btgerr());
}

