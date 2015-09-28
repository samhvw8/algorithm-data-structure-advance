/*
 * $Id: bsemap.c,v 1.8 2008/05/08 14:34:53 mark Exp $
 *
 * block   - locks active BT file
 * bunlock - unlocks active BT file
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#include "bc.h"
#include "bt.h"
#include "btree_int.h"

/* #undef DEBUG
   #define DEBUG 2 */

static sigjmp_buf env_alrm;

static struct flock lck = {0, SEEK_SET, 0, 0, 0 };

/* handle timeouts for write access */

static void sigalrm_handler(int signo) 
                            
{
    siglongjmp(env_alrm,1);
    return;
}

/* lock current BT file
 * return FALSE if unable to lock
 */

int block(void)
{
    int i, ierr;
    
    if (btact->lckcnt > 0 ) {
        btact->lckcnt++;
#if DEBUG >= 2
        fprintf(stderr,"BLOCK: soft lock - lcknt %d\n",btact->lckcnt);
#endif      
        return (TRUE);  /* lock already in use for this process */
    }

    lck.l_type = F_WRLCK;
    if (signal(SIGALRM,sigalrm_handler) == SIG_ERR) {
        bterr("BLOCK",QBADAL,NULL);
        return(FALSE);
    }   

    /* save current signal mask in environment; enables proper repeat
     * behaviour when repetive calls to block are made */
    if (sigsetjmp(env_alrm,1) != 0) {
        /* get here when alarm goes off; couldn't lock file */
        signal(SIGALRM,SIG_DFL);
        alarm(0);
        return(FALSE);
    }

    alarm(ZSLEEP);
    
    if (fcntl(btact->fd,F_SETLKW,&lck) == -1) {
        signal(SIGALRM,SIG_DFL);
        alarm(0);
        return(FALSE); 
    }
    alarm(0); 
    
#if DEBUG >= 2
    fprintf(stderr,"BLOCK: hard lock - lcknt %d\n",btact->lckcnt);
#endif
    if (btact->shared && btact->cntxt->super.scroot != ZNULL) {
        /* ensure super block info is up-to-date */
        brdsup(); 
        /* ensure current root is in memory and locked */
        /* if not creating new index file (when scroot is ZNULL) */
        ierr = brdblk(btact->cntxt->super.scroot,&i);
        if (ierr == 0) {
            bsetbs(btact->cntxt->super.scroot,1);
        }
    }
    btact->lckcnt = 1;
    return(TRUE);
}

/* unlock file: it is not an error if file is currently unlocked */

int bulock(void)
{
    if (btact->lckcnt > 1) {
        /* don't unlock if lock request nested (only top level will
         * unlock) */
#if DEBUG >= 2
        fprintf(stderr,"BULOCK: soft unlock - lckcnt %d\n",btact->lckcnt);
#endif      
        btact->lckcnt--;
        return(0);
    }
    else if (btact->lckcnt == 1) {
#if DEBUG >= 2
        fprintf(stderr,"BULOCK: hard unlock - lckcnt %d\n",btact->lckcnt);
#endif
    
        /* write any changed in-memory blocks to file */
        btsync();
    
        if (btact->fd >= 0) {
            lck.l_type = F_UNLCK;
            if (fcntl(btact->fd,F_SETLK,&lck) == -1) {
                bterr("BULOCK",QUNLCK,NULL);
            }
        }
        btact->lckcnt = 0;
    }
    
    return(0);
}   


