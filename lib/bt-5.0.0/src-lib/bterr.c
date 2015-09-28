/*
 * $Id: bterr.c,v 1.32 2012/11/15 12:19:37 mark Exp $
 *
 * btcerr: returns last error code, io error code and appropriate
 *         message
 *
 * Parameters:
 *   ierr   returned with number of last error
 *   ioerr  returned with number of last i/o error (mostly)
 *   srname returned with name of subroutine which detected
 *          the error
 *    msg    returned with a text message for the error
 *
 * bterr:  set error code
 * bgterr: returns error code
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

#include  <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "bc.h"

static int qerror = 0;
static int qcode = 0;
static char qarg[ZRNAMESZ];
static int syserror;

static char qname[ZMSGSZ];

char *msgblk[] = {
    "null",
    " Block %s is not a root block",
    " Unable to close index file: %s",
    " Unable to create index file: %s",
    " Unable to read source or destination block ",
    " I/O error writing block %s",
    " I/O error reading super root %s",
    " I/O error writing super root",
    " I/O error opening index file: %s",
    " I/O error reading block %s",
    " An index file is already open",
    " Can't split full block",
    " Bad info block index used: %s",
    " Unable to acquire a free memory block",
    " Stack underflow",
    " Stack overflow",
    " Can't insert key at block: %s",
    " Replace location out of range: %s",
    " Split: search for middle key failed",
    " Requested write block not in memory",
    " Balance: search for key failed",
    " Exact flag not set for delete",
    " Internal inconsistency in delete operation",
    " Search for deleted key replacement failed (in block %s)",
    " Demote search failed",
    " Demote split failed",
    " Join search failed",
    " Cannot locate default root ($$default)",
    " Deletion of the current or super root is forbidden",
    " Negative in-memory index encountered",
    " No index file open for this operation",
    " Index file already in use",
    " Debug option not recognised",
    " No more index files may be opened (limit reached)",
    " Invalid index file context pointer",
    " File is busy",
    " Function prohibited in shared access mode [OBSOLETE]",
    " No block available for data storage",
    " Data block usage gone bad: %s",
    " Data address pointer references a non-data block: %s",
    " Index context invalid for current key operation",
    " Circular data segment pointer encountered at: 0x%s",
    " Unlock operation failed",
    " LRU queue corrupt - index not in list",
    " Data record action not permitted for current root",
    " Data record length cannot be negative",
    " Key \"%s\" already exists in index",
    " Key \"%s\" does not exist in index",
    " Write access to index prohibited",
    " Block on free list is not marked as free",
    " Index file is incompatible with current version: %s",
    " Data capacity exceeded at block: %s",
    " Index file is at maximum size",
    " Unable to set alarm for locking",
    " Data record address is negative: %s",
    " Defined block size is not a power of two: %s",
    " Duplicates are not permitted in superroot.",
    " Location search exceeds key count at block: %s",
    " Index file likely not LFS (64bit) enabled; doesn't match library.",
    " Index file likely LFS (64bit) enabled; doesn't match library.",
    " Duplicate key address does not reference a duplicate block: %s.",
    " Duplicate key entry has wrong size.",
    " Bad direction parameter.",
    " No message exists for this error code"
};

#define MSGBLKMAX ((int) (sizeof(msgblk)/sizeof(msgblk[0])))

void btcerr(int *ierr,int *ioerr,char *srname,char *msg)
{
    char tmpfmt[ZMSGSZ];

    memset(msg,0,ZMSGSZ);
    if (qerror == 0) {
        memset(srname,0,ZRNAMESZ);
        *ierr = 0;
        *ioerr = 0;
        return;
    }
    
    strncpy(srname,qname,ZRNAMESZ-1);
    *ierr = qerror;
    *ioerr = qcode;

    if (qerror >= MSGBLKMAX) qerror = MSGBLKMAX-1;

    if (qcode != 0) {
        if (syserror) {
            snprintf(tmpfmt,ZMSGSZ-1,"%s (System error: %s)",msgblk[qerror],
                    strerror(qcode));
        }
        else {
            snprintf(tmpfmt,ZMSGSZ-1,"%s (Info: %d)",msgblk[qerror],qcode);
        }
        if (strlen(qarg) != 0) {
            snprintf(msg,ZMSGSZ-1,tmpfmt,qarg);
        }
        else {
            strncpy(msg,tmpfmt,ZMSGSZ-1);
        }
    }
    else {
        snprintf(msg,ZMSGSZ-1,msgblk[qerror],qarg);
    }
    
    return;
}
/*
  bterr: save error and io codes and return

  void bterr(char *name,int ierr,char* arg)

    name   name of function which detected the error
    ierr   error code
    arg    argument to use with error message

  Only the first error is recorded.  To reset saved error codes, use
  bterr("",0,NULL);
  
*/

void bterr(char *name, int errorcode, char* arg)
{

    if (strlen(name) == 0) {
        errno = 0;
        syserror = FALSE;
        qname[0] = '\0';
        qerror = 0;
        qcode = 0;
        qarg[0] = '\0';
    }
    else if (qerror == 0) {
        strncpy(qname,name,ZRNAMESZ-1);
        qerror = errorcode;
        if (arg != NULL) strncpy(qarg,arg,ZRNAMESZ-1);
        if (errno != 0) {
            syserror = TRUE;
            qcode = errno; /* set to system errorcode */
            errno = 0;
        }
    }
}

/* Return last error code */

int btgerr() {
    return qerror;
}
