/*
 * $Id: bc.h,v 1.38 2012/11/15 12:18:57 mark Exp $
 *
 * Copyright (C) 2003, 2004, 2012 Mark Willson.
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

#ifndef _btconst_
#define _btconst_

#include <limits.h>

/*  B tree constants */

/* Support for large files (> 2GB); define macro _FILE_OFFSET_BITS=64 */
#if _FILE_OFFSET_BITS == 64
typedef long long BTint;
#define BTINT_MAX 9223372036854775807LL
#define ZXFMT "%llx"
#define ZINTFMT "%lld"
#define Z20DFMT "%20lld"
#else
typedef int BTint;
#define BTINT_MAX INT_MAX
#define ZXFMT "%x"
#define Z20DFMT "%20d"
#define ZINTFMT "%d"
#endif

#define DEBUG 0

#define TRUE 1
#define FALSE 0

#define LFSHDR 0xFFBADDEF

/* Implementation defined constants */

/* bits per byte */
#define ZBYTEW 8

/* number of bytes per BTint */
#if _FILE_OFFSET_BITS == 64 
#define ZBPW 8
#else
#define ZBPW 4
#endif


/* maximum key size (bytes) */
#define ZKYLEN 32

#if _FILE_OFFSET_BITS==64
/* number of in-memory blocks (assume LFS users are memory rich) */
#define ZMXBLK  16
/* block size in bytes for LFS users (must be power of 2) */
#define ZBLKSZ 8192
#else
/* number of in-memory blocks (3 is the minimum) */
#define ZMXBLK  3
/* block size in bytes (must be power of 2) */
#define ZBLKSZ 1024
#endif

/* number of keys per block */
#define ZMXKEY ((ZBLKSZ-ZBPW-ZINFSZ*ZBPW)/(ZKYLEN+(3*ZBPW))) 
/* number of pad words required */
#define ZPAD ((ZBLKSZ-(ZKYLEN+(3*ZBPW))*ZMXKEY-ZBPW-ZINFSZ*ZBPW)/ZBPW)
/* threshold for block joining */
#define ZTHRES 3
/* number of bt indexes that may be open concurrently */
#define ZMXACT 5
/* number of seconds to wait for locked file to unlock */
#define ZSLEEP 5

/* Message buffer sizes */
#define ZRNAMESZ 16
#define ZMSGSZ 132

/* Indexes into block info words - first ZINFSZ words in every block 
 *
 * ZBTYPE   ZSUPER      ZROOT           ZINUSE       ZFREE   ZDATA 
 * ZMISC    #free blks  dups_allowed    Unused       Unused  bytes free
 * ZNXBLK   free list   data blk list   Parent blk # flink   flink 
 * ZNKEYS   # keys      # keys          # keys       Unused  next free byte 
 * ZNBLKS   # blocks    root blk #      root blk #   Unused  blink
 * ZNXDUP   Unused      Act. dup blk    Unused       Unused  min seg size
 * 
 */

/* ZVERS must be incremented when structure of B Tree index file
 * changes */
#define ZVERS 0x5

/* ZBTYPE and ZBTVER share information word 0 */
#define ZBTYPE 0
#define ZBTVER -1
#define ZMISC  1
#define ZNXBLK 2
#define ZNKEYS 3
#define ZNBLKS 4
#define ZNXDUP 5


/* position constants */
#define ZSTART 1
#define ZEND 2

/* max info word index */
#define ZINFSZ 6

/* block type of root */
#define ZROOT 1
/* block type of inuse */
#define ZINUSE 2
/* block type of free */
#define ZFREE 3
/* block type of data */
#define ZDATA 4
/* block type for duplicate keys */
#define ZDUP 5
/* super block location in index file */
#define ZSUPER 0
/* null block pointer */
#define ZNULL -1

/*  Data block defines:
 *      
 *      Each data record segment is prefixed by ZDOVRH bytes of
 *      information (six bytes for a 32 bit int implementation).
 *      These are used as follows:
 *
 *          Bytes 1 and 2: the size of the data segment in bytes
 *          (maximum size of a data segment is therefore 65536 bytes).
 *          
 *          Bytes 3-6: segment address of the next segment of this
 *          data record (0 if the last (or only) segment)).
 */

#define ZDRSZ 2
#define ZDOVRH  (ZDRSZ+ZBPW)

/* minimum number of bytes for a data segment */
#define ZDSGMN 7    

/* Previous/next key constants */
#define ZPREV 1
#define ZNEXT 2

/* Perform some simple constant consistency checks */

/* Enforce 3 in-memory blocks as minimum
    Need 3 when a root block is split 
*/
#if (ZMXBLK < 3) 
#error Must have minimum of three in-memory blocks
#endif

/* Ensure block joining will take place
*/
#if (ZMXKEY-ZTHRES <= 0)
#error Definition of ZMXKEY and ZTHRES inconsistent
#endif

/*
   Error codes
*/

#define QBLKNR   1
#define QCLSIO   2
#define QCRTIO   3
#define QCPBLK   4

#define QWRBLK   5
#define QRDSUP   6
#define QWRSUP   7
#define QOPNIO   8

#define QRDBLK   9
#define QIXOPN  10
#define QSPLIT  11
#define QINFER  12

#define QNOMEM  13
#define QSTKUF  14
#define QSTKOF  15
#define QBLKFL  16

#define QLOCTB  17
#define QSPKEY  18
#define QWRMEM  19
#define QBALSE  20

#define QDELEX  21
#define QDELER  22
#define QDELRP  23
#define QDEMSE  24

#define QDEMSP  25
#define QJNSE   26
#define QNODEF  27
#define QDELCR  28

#define QBADIX  29
#define QNOBTF  30
#define QINERR  31
#define QBADOP  32

#define QNOACT  33
#define QBADAP  34
#define QBUSY   35
#define QNOTOP  36 /* obsolete in v5 */

#define QNOBLK  37
#define QNEGSZ  38
#define QNOTDA  39
#define QBADCTXT  40

#define QDLOOP  41
#define QUNLCK  42
#define QLRUER  43
#define QDAERR  44

#define QDNEG   45
#define QDUP    46
#define QNOKEY  47
#define QNOWRT  48

#define QNOTFR  49
#define QBADVR  50
#define QDAOVR  51
#define QF2BIG  52

#define QBADAL  53
#define QDRANEG 54

#define QBLKSZERR 55
#define QNODUPS 56

#define QPOSERR 57

#define QNOT64BIT 58
#define Q64BIT 59

#define QNOTDUP 60
#define QDUPSZ 61
#define QBADIR 61

#endif /* _btconst_ */
