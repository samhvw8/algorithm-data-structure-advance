/*
 * $Id: btree.h,v 1.11 2011/06/13 19:34:46 mark Exp $
 * 
 * Standard btree header
 * Defines all API functions
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

#include "bt.h"

extern int btcls(BTA *);
extern BTA *btcrt(char *,int, int);
extern int btchgr(BTA *,char *);
extern int btcrtr(BTA *,char *);
extern int btdups(BTA *,BTint);
extern int btinit(void);
extern BTA *btopn(char *,int,int);
extern int bdbug(BTA *,char *,BTint);
extern int bfndky(BTA *,char *,BTint *);
extern int binsky(BTA *,char *,BTint);
extern int bnxtky(BTA *,char *,BTint *);
extern int bprvky(BTA *,char *,BTint *);
extern int bdelky(BTA *,char *);
extern int btdelr(BTA*,char *);
extern void btcerr(int *,int *,char *,char *);
extern int bupdky(BTA *,char *,BTint);

extern int btins(BTA *,char *,char *,int);
extern int btsel(BTA *,char *,char *,int,int *);
extern int btupd(BTA *,char *,char *,int);
extern int btdel(BTA *,char *);
extern int btseln(BTA *,char *,char *,int,int *);
extern int btselp(BTA *,char *,char *,int,int *);
extern int btrecs(BTA *,char *,int *);

extern int btlock(BTA *);
extern int btunlock(BTA *);

extern int btpos(BTA *,int);
extern int btthresh(BTA*, int);












