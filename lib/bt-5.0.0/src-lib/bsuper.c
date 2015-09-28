/*
 * $Id: bsuper.c,v 1.13 2012/09/29 15:06:41 mark Exp $
 *
 *
 * brdsup  - reads super root
 * bwrsup  - writes super root
 *
 * Both return 0 for success, error code otherwise
 * 
 *
 * Copyright (C) 2003, 2004 Mark Willson.
 *
 * This file is part of the B Tree library.
 *
 * The B Tree library is free software; you can redistribute it and/or
c * modify it under the terms of the GNU General Public License as
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

/* get super root from disk */
int brdsup()
{
    int errcode,idx;
    BTint ver, type, misc;
    
    errcode = brdblk(ZSUPER,&idx);
    if (errcode != 0) {
        bterr("BRDSUP",QRDSUP,(errcode<0)?"(EOF?)":"");
    }
    else {
        ver = bgtinf(ZSUPER,ZBTVER);
        type = bgtinf(ZSUPER,ZBTYPE);
        misc = bgtinf(ZSUPER,ZMISC);
        if (type != ZROOT) {
            bterr("BRDSUP",QBLKNR,itostr(ZSUPER));
        }
        else if (ver == LFSHDR) {
            bterr("BRDSUP",QNOT64BIT,NULL);
        }
        else  if (ver != ZVERS) {
            bterr("BRDSUP",QBADVR,itostr(ZVERS));
        }
        else if (misc == LFSHDR) {
            bterr("BRDSUP",Q64BIT,NULL);
        }
        else {
            /* retain free list pointers et al */
            btact->cntxt->super.snfree = misc;
            btact->cntxt->super.sfreep = bgtinf(ZSUPER,ZNXBLK);
            btact->cntxt->super.sblkmx = bgtinf(ZSUPER,ZNBLKS);
        }
    }
    return btgerr();
}

/* update super root on disk */
int bwrsup()
{
    int errcode,idx;
    BTint nkeys;

    errcode = brdblk(ZSUPER,&idx);
    if (errcode != 0) {
        bterr("BWRSUP",QRDSUP,itostr(ZSUPER));
    }
    else {
        nkeys = bgtinf(ZSUPER,ZNKEYS);
        bsetbk(ZSUPER,ZROOT,btact->cntxt->super.snfree,
               btact->cntxt->super.sfreep,
               nkeys,
               btact->cntxt->super.sblkmx,ZNULL);
        errcode = bwrblk(ZSUPER);
        if (errcode != 0) {
            bterr("BWRSUP",QWRSUP,itostr(ZSUPER));
        }
    }
    return errcode;
}
