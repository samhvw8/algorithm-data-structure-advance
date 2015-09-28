/*
 * $Id: bt.h,v 1.14 2012/10/07 19:49:30 mark Exp $
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

/* B tree structure header */

#ifndef _btheader_
#define _btheader_
#include <stdio.h>

#include "bc.h"

/*
  B tree cache manager structure
*/
struct bt_cntrl {
   BTint inmem;  /* holds block number of in-memory copy */
   int busy;   /* if non-ZERO, block cannot be flushed to disk */
   int writes; /* if non-ZERO, number of writes to this block */
   int lrunxt;  /* index of next in-memory block for LRU queue */
};

typedef struct bt_cntrl CNTRL;

/* Tail and head of least recently used queue */

struct bt_lru {
    int lrut;
    int lruh;
};

/*
   B tree in-memory cached blocks
*/

/* struct bt_memrec { */
/*     BTint infblk[ZINFSZ]; */
/*     char keyblk[ZMXKEY] [ZKYLEN]; */
/*     BTint dup[ZMXKEY]; /\* if non-ZNULL, key is dup; points to tail of dup */
/*                     * key chain *\/ */
/*     BTint valblk[ZMXKEY]; */
/*     BTint lnkblk[ZMXKEY+1]; */
/* #if ZPAD != 0 */
/*     BTint padblk[ZPAD]; */
/* #endif */
/* }; */

struct bt_keyent {
    char key[ZKYLEN];
    BTint val;
    BTint dup;
};

typedef struct bt_keyent KEYENT;
    
struct bt_memrec {
    BTint infblk[ZINFSZ];
    KEYENT keyblk[ZMXKEY];
    BTint lnkblk[ZMXKEY+1];
#if ZPAD != 0
    BTint padblk[ZPAD];
#endif
};

typedef struct bt_memrec MEMREC;

/* Structure describing a data block */

struct bt_datblk {
    BTint infblk[ZINFSZ];
    char data[ZBLKSZ-(ZINFSZ*ZBPW)];
};

typedef struct bt_datblk DATBLK;

/* Duplicate key entry in block type ZDUP */
struct bt_dkey {
    char key[ZKYLEN];
    int deleted;
    BTint val;
    BTint flink;
    BTint blink;
};

typedef struct bt_dkey DKEY;

/* context from last find operation */

struct bt_lf {
    char lfkey[ZKYLEN];
    BTint lfblk;
    int lfpos;
    int lfexct;
    BTint draddr;
};

/* B tree statistic counters */

struct bt_stats {
    int xlogrd;
    int xlogwr;
    int xphyrd;
    int xphywr;
    int xsplit;
    int xjoin;
    int xbal;
    int xgot;
    int xrel;
};

typedef struct bt_stats STATS;

/*  B tree stack */

#define STKMAX 40

struct bt_stk {
    int stkptr;
    BTint stk[STKMAX];
};


/* B tree: Super root information plus current root */

struct bt_super {
    BTint sblkmx;
    BTint snfree;
    BTint sfreep;
    BTint scroot;
    int smode;
    int smod;
    char scclas[ZKYLEN];
};

/* Common structure for all context information for an index */

struct bt_cntxt {
    struct bt_lru lru;
    struct bt_lf lf;
    struct bt_stk stk;
    struct bt_stats stat;
    struct bt_super super;
};

typedef struct bt_cntxt CNTXT;

#define FIDSZ 72

struct bt_active {
    FILE *idxunt;
    char idxfid[FIDSZ];
    int fd;                 /* used to hold index file descriptor */
    int lckcnt;             /* re-entrant lock count */
    int shared;
    int wt_threshold;       /* write through threshold */
    CNTRL *cntrl;
    MEMREC *memrec;
    CNTXT *cntxt;
};

typedef struct bt_active BTA;

extern BTA btat[];                  /* might not need this if we can access     
                                        everything through btact */

extern BTA *btact;                  /* point to active index info */

#endif
