/*
 * $Id: bigt.c,v 1.5 2012/11/15 12:19:46 mark Exp $
 * 
 * NAME
 *      bigt - a stress test for the B Tree library, to ensure the 
 *      largest files are handled properly.
 *
 * SYNOPSIS
 *      bigt [-n record_count] [-s datasize]
 *
 *      record_count defines the number of records to be written to
 *      the B Tree index file.  If not specified, INT_MAX (32 or 64
 *      bit) records are attempted to be written.  The switch -s
 *      enables the size of the data record to be changed from the
 *      default (5*ZBLKSZ bytes).
 *
 *  DESCRIPTION
 *      Bigt creates a database named test_db in the working directory
 *      and inserts up to record_count keys and associated data
 *      records.  The default data record is 5*ZBLKSZ bytes, filled with
 *      the character 'D'.
 * 
 *      Bigt should fail gracefully at 2GiB on a machine with 32
 *      bit integers.  If linked against a BT library with LFS
 *      support, it should fail gracefully when disk space is full.
 *
 *  MODIFICATION HISTORY
 *  Mnemonic        Rel Date    Who
 *  BIGT            1.0 040923  mpw
 *      Created.
 *  BIGT            1.1 041004  mpw
 *      Added -n command switch.
 *  BIGT            2.0 100525  mpw
 *      Support for large files.
 *      Catch interrupt and quit gracefully
 *      
 * Copyright (C) 2003, 2004, 2010 Mark Willson.
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
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#include "btree.h"

#define TRUE 1
#define FALSE 0

#define DATASIZE ZBLKSZ*5

#if _FILE_OFFSET_BITS == 64
#define ATOI atoll
#else
#define ATOI atoi
#endif

jmp_buf env;

void break_handler (int sig)
{
    signal(SIGINT,SIG_DFL);
    longjmp(env,1);
}


void print_error(void)
{
    int errorcode,ioerror;
    char rname[ZRNAMESZ],msg[ZMSGSZ];

    btcerr(&errorcode,&ioerror,rname,msg);
    printf("\tBTree error: %d [%s]: %s\n",errorcode,rname,msg);
    return;
}

int main(int argc, char *argv[])
{
    int status, exit_val;
    BTint i;
    char *s;
    char *data = NULL;
    int datasize = DATASIZE;
    char key[ZKYLEN];
    BTint nrecs = BTINT_MAX;
    BTA *bt;

    while (--argc > 0 && (*++argv)[0] == '-') {
        for (s=argv[0]+1;*s != '\0'; s++) {
            switch (*s) {
                case 'n':
                    nrecs = ATOI(*++argv);
                    --argc;
                    break;
                case 's':
                    datasize = atoi(*++argv);
                    --argc;
                    break;
                default:
                    fprintf(stderr,"bigt: unknown command switch: -%c\n",*s);
                    return EXIT_FAILURE;
            }
        }
    }
                    
    if (datasize > 0) {
        data = malloc(datasize);
    }
    else {
        fprintf(stderr,"Illegal datasize specified: %d.\n",datasize);
        return EXIT_FAILURE;
    }

    if (data == NULL) {
        fprintf(stderr,"No memory for data record (%d bytes requested).\n",
                datasize);
        return EXIT_FAILURE;
    }
    
    memset(data,'D',datasize);
    
    if (btinit() != 0) goto fin;
    if ((bt = btcrt("test_db",0,FALSE)) == NULL) goto fin;

    exit_val = EXIT_SUCCESS;
    signal(SIGINT,break_handler);
    if (setjmp(env) == 0) {
        for (i=0;i<nrecs;i++) {
            snprintf(key,ZKYLEN,ZINTFMT,i);
            key[ZKYLEN-1] = '\0';
            status = btins(bt,key,data,datasize);
            if (status != 0) {
                printf("While attempting to insert key: %s;\n",key);
                print_error();
                exit_val = EXIT_FAILURE;
                break;
            }
        }
    }
    else {
        fprintf(stderr,"... terminated by Cntrl-C\n");
    }   
    if (btcls(bt) != 0) goto fin;
    return exit_val;
  fin:
    print_error();
    return EXIT_FAILURE;
}
    
