/*
 *  $Id: btr.c,v 1.26 2012/11/24 16:35:44 mark Exp $
 *  
 *  NAME
 *      btr - attempts to recover corrupt btree index file
 *  
 *  SYNOPSIS
 *      btr [-adfkrv] [-n cnt] [--] old_file new_file
 *  
 *  DESCRIPTION
 *      btr will attempt the copy the contents of the btree inex file
 *      old_file into new_file, mediated by the following command
 *      arguments:
 *
 *      -k        copy keys only (default)
 *      -d        copy keys and data
 *      -n cnt    accept up to cnt io errors before failing
 *      -a        allow duplicates in the new btree index file
 *      -v        be verbose (up to three levels of verbosity by -vv
 *                and -vvv)
 *      -f        overwrite new_file if it exists
 *      -r        request full recovery attempt
 *      
 *       btr will also attempt to copy index files created with
 *       previous versions of btree, but recovery is limited as
 *       necessary information for btree index reconstruction is not
 *       available in earlier versions of the index structure.  This
 *       mostly affects multi-rooted trees, where the roots will be
 *       lost and all keys copied into the $$default root.
 *
 *  NOTES
 *      Open corrupt btree file using btr version of btopn (if
 *      necessary), to bypass some consistency checks.
 *
 *      Try and read superroot.  If successful, store root names and root
 *      blocks.  Only the roots in the superroot are retained.
 *
 *      For each block, starting from 1, read it in.  If marked as
 *      ZROOT or ZINUSE, extract the keys and values directly from the
 *      in-memory array.  If -k specified, write key and value to new
 *      btree index file.  If -d specified, if value is a valid
 *      draddr, try and read data record.  Data record addresses are
 *      stored in a supporting bt index file, so that we can detect
 *      circular references.  If data record read OK, write key and
 *      record to new btree file.
 *
 *      In version 4 (and later) of the btree index, each ZINUSE block
 *      will contain the root block it belongs to.  This will allow us to
 *      partition keys by their roots, although if we cannot read the
 *      superroot correctly, the original root names will be lost.
 *
 *      Version 5 handles duplicate keys through the introduction of
 *      duplicate key blocks (essentially data blocks maintaining a
 *      list of duplicate keys).  The MEMREC4 data structure is
 *      defined in btr.h to allow earlier versions of btree library
 *      index files to be migrated and/or recovered.
 * 
 *  BUGS
 *      btr delves into the innards of a btree index file and should
 *      not be used as a typical example of use of the btree API.
 *  
 *  MODIFICATION HISTORY
 *  Mnemonic        Rel Date     Who
 *  BTR             1.0 20110401 mpw
 *    Created.
 *    
 * Copyright (C) 2011,2012 Mark Willson.
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
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "btree.h"
#include "btree_int.h"
#include "btr.h"

/* #undef DEBUG */
/* #define DEBUG 1 */


#define VERSION "$Id: btr.c,v 1.26 2012/11/24 16:35:44 mark Exp $"
#define KEYS    1
#define DATA    2
#define DLOOP -1
#define IOERROR -2
#define BAD_DRADDR -3
#define DR_READ_ERROR -4

/* version of btlib for which full recovery is possible, that is each
 * block records the root of the tree.
 * Versions later than this, coincidently, have a new index block
 * structure for better handling of duplicate keys
 */
#define FULL_RECOVERY_VERSION 4

/* GLOBALS --------------------------------------------------------------*/

char *prog;
char *data_record;
int data_record_size = ZBLKSZ;
int limited_recovery = FALSE;
BTint src_file_ver = ZVERS;
char usage_str[] = "btr [-adfkrv] [-n cnt] [--] old_file new_file";
    
/* Recovery statistics */
struct {
    int   nioerrs;
    BTint keys;
    BTint records;
    BTint seg_addr_loops;
    BTint key_blocks;
    BTint dup_blocks;
    BTint data_blocks;
    BTint free_blocks;
    BTint total_blocks;
    BTint bad_draddrs;
    BTint dr_read_errors;
    BTint bad_blocks;
    BTint bad_index_blocks;
} stats;

/* v4 keyblk generally holds more keys */
#if ZMXKEY > ZMXKEY4
#define ZRNKEYS ZMXKEY
#else
#define ZRNKEYS ZMXKEY4
#endif

/* Hold keys and values read from block */
struct bt_block_keys {
    int nkeys;
    KEYENT keyblk[ZRNKEYS];
};

typedef struct bt_block_keys BTKEYS;
BTKEYS* superroot_keys = NULL;

/* END GLOBALS ---------------------------------------------------------*/

int file_exists(char *pathname)
{
    FILE* f;

    f = fopen(pathname,"r");
    if (f == NULL) {
        return FALSE;
    }
    else {
        fclose(f);
    }
    return TRUE;
}


void print_bterror(void)
{
    int errorcode, ioerr;
    char fname[ZRNAMESZ],msg[ZMSGSZ];

    btcerr(&errorcode,&ioerr,fname,msg);
    fprintf(stderr,"%s: btree error (%d) [%s] - %s\n",
            prog,errorcode,fname,msg);
}

void kalloc(char **buf,int bufsiz)
{
    *buf = malloc(bufsiz);
    /* fprintf(stderr,"..allocating %d bytes\n",bufsiz); */
    if (buf == NULL) {
        fprintf(stderr,"%s: cannot acquire enough memory (%d bytes)\n",
                prog,bufsiz);
        exit(EXIT_FAILURE);
    }
}

/* ==================================================================
 * Following functions are btr versions of btlib routines, modifed
 * to handle v4 and earlier index files and block structure
 * ==================================================================*/

/* Open btree index file in recovery mode (i.e. limited checking) */
BTA *btropn(char *fid,int vlevel,int full_recovery)
{
    int idx,ioerr;
    
    bterr("",0,NULL);

    btact = bnewap(fid);
    if (btact == NULL) {
        bterr("BTROPN",QNOACT,NULL);
        return NULL;
    }
    if ((btact->idxunt = fopen(fid,"r+b")) == NULL) {
        bterr("BTROPN",QOPNIO,fid);
        return NULL;
    }
    strncpy(btact->idxfid,fid,FIDSZ);
    btact->idxfid[FIDSZ-1] = '\0';
    if (bacini(btact) != 0) {
        fclose(btact->idxunt);
        goto fin;
    }
    
    btact->shared = FALSE;
    btact->cntxt->super.smod = 0;
    btact->cntxt->super.scroot = 0;

    /* always lock file */
    if (!block()) {
        bterr("BTROPN",QBUSY,NULL);
        goto fin;
    }
    /* read in super root */
    ioerr = brdblk(ZSUPER,&idx);
    if (ioerr != 0) {
        bterr("BRDSUP",QRDSUP,(ioerr<0)?"(EOF?)":"");
        goto fin;
    } 
    if (bgtinf(ZSUPER,ZBTYPE) != ZROOT) {
        fprintf(stderr,"%s: superroot is not a root: possible file damage?\n",
                prog);
        /* superroot data cannot be trusted */
        btact->cntxt->super.snfree = 0;
        btact->cntxt->super.sfreep = ZNULL;
        btact->cntxt->super.sblkmx = BTINT_MAX;
    }
    else {
        /* retain free list pointers et al */
        btact->cntxt->super.snfree = bgtinf(ZSUPER,ZMISC);
        btact->cntxt->super.sfreep = bgtinf(ZSUPER,ZNXBLK);
        btact->cntxt->super.sblkmx = bgtinf(ZSUPER,ZNBLKS);
    }
    src_file_ver = bgtinf(ZSUPER,ZBTVER);
    if (!full_recovery && (src_file_ver < FULL_RECOVERY_VERSION ||
                           src_file_ver == LFSHDR)) {
        limited_recovery = TRUE;
        if (src_file_ver == LFSHDR) {
            fprintf(stderr,"%s: LFS source file version unreadable; "
                    "setting to 0.\n",prog);
            src_file_ver= 0;
        }
        fprintf(stderr,"%s: index file is version: 0x" ZXFMT ". "
                "Running in limited recovery mode.\n",
                prog,src_file_ver);    
    }
    return btact;
fin:
    bacfre(btact);
    return NULL;
}

/*------------------------------------------------------------------------
 * Return size and next segment address of data record segment
 * indicated by draddr
 *----------------------------------------------------------------------*/

int btrseginfo(BTint draddr, int *size, BTint *nextseg) 
{
    BTint blk;
    int offset;
    int status,  idx;
    DATBLK4 *d;

    cnvdraddr(draddr,&blk,&offset);
    
    status = brdblk(blk,&idx);
    if (status != 0) {
        return(status);
    }

    d = (DATBLK4 *) (btact->memrec)+idx;
    *size = rdsz(d->data+offset);
    *nextseg = rdint(d->data+offset+ZDRSZ);
    return(0);
}

int btrseldt(BTint draddr, char *data, int dsize) 
{
    BTint dblk;
    int status, idx, type,
        segsz = 0, cpsz = -1;
    int offset = 0;
    int totsz = 0;
    int sprem = dsize;
    
    DATBLK4 *d;

    while (sprem > 0 && draddr != 0) {
        /* unpick data pointer */
        cnvdraddr(draddr,&dblk,&offset);
        type = bgtinf(dblk,ZBTYPE);
        if (type != ZDATA && type != ZDUP) {
            bterr("BTRSELDT",QNOTDA,itostr(dblk));
            totsz = -1;
            goto fin;
        }
        status = brdblk(dblk,&idx);
        d = (DATBLK4 *) (btact->memrec)+idx;
#if DEBUG > 0
        fprintf(stderr,"BTRSELDT: Using draddr 0x" ZXFMT " (" ZINTFMT
                ",%d), found 0x" ZXFMT "\n",
                draddr,dblk,offset,*(d->data+offset+ZDOVRH));
#endif

        segsz = rdsz(d->data+offset);
        draddr = rdint(d->data+offset+ZDRSZ);
#if DEBUG > 0
        fprintf(stderr,"BTRSELDT: Seg size: %d, next draddr: 0x" ZXFMT "\n",
                segsz,draddr);
#endif
        cpsz = (segsz>sprem)?sprem:segsz;
#if DEBUG > 0
        fprintf(stderr,"BTRSELDT: copying %d bytes\n",cpsz);
#endif      
        memcpy(data,d->data+offset+ZDOVRH,cpsz);
        data += cpsz;
        sprem -= cpsz;
        totsz += cpsz;
    }
    
fin:
    return(totsz);
}

/*-----------------------------------------------------------
 * return number of bytes occupied by data record pointed to by draddr
 *----------------------------------------------------------- */
#define BUFSZ 80

int btrrecsz(BTint draddr, BTA* dr_index)
{
    BTint blk,newdraddr;
    BTA* b;
    int offset, segsz, recsz, status;
    
    recsz = 0;
    while (draddr != 0) {
        if (dr_index != NULL) {
            /* recovery mode; check for circular draddr references */
            char dr_str[BUFSZ];
            b = btact;              /* save active index handle */
            snprintf(dr_str,BUFSZ,ZXFMT,draddr);
            dr_str[BUFSZ-1] = '\0';
            status = binsky(dr_index,dr_str,0);
            btact = b;              
            if (status != 0) {
                if (status == QDUP) {
                    bterr("",0,NULL);
                    bterr("BTRRECSZ",QDLOOP,dr_str);
                }
                goto fin;
            }
        }

        cnvdraddr(draddr,&blk,&offset);
#if DEBUG > 0
        fprintf(stderr,"BTRRECSZ: Processing draddr: " ZINTFMT ", blk: " ZINTFMT
                ", offset: %d\n",draddr,blk,offset);
#endif
        /* ensure we are pointing at a data block */
        if (bgtinf(blk,ZBTYPE) != ZDATA) {
            bterr("BRECSZ",QNOTDA,itostr(blk));
            return(0);
        }
        btrseginfo(draddr,&segsz,&newdraddr);
        
#if DEBUG > 0       
        fprintf(stderr,"BTRRECSZ: Seg size: %d, next seg: " ZINTFMT "\n",
                segsz,newdraddr);
#endif      
        if (newdraddr == draddr) {
            /* next segment address should never refer to current
               segment */
            char dr_str[BUFSZ];
            snprintf(dr_str,ZKYLEN,ZXFMT,draddr);
            dr_str[ZKYLEN-1] = '\0';
            bterr("BTRRECSZ",QDLOOP,dr_str);
            return(0);
        }
        draddr = newdraddr;
        recsz += segsz;
    }
  fin:
    return(recsz);
}

/* ========================================================================
 * End of btlib modified routines
 * ========================================================================*/

int load_block(BTA* in, BTint blkno, int vlevel)
{
    int status, idx;
    
    btact = in;
    status = brdblk(blkno,&idx);
    if (status != 0) {
        if (feof(btact->idxunt)) {
            idx = EOF;
        }
        else {
            if (vlevel >= 3) {
                fprintf(stderr,"%s: I/O error: %s\n",prog,strerror(errno));
            }
            stats.nioerrs++;
            idx = IOERROR;
        }
    }
    return idx;
}

/* copy keys from block */
BTKEYS* get_keys(int idx, int nkeys, BTKEYS* k)
{
    int j;
    BTKEYS* keys;
    MEMREC4* m4;
    
    if (k == NULL) {
        keys = malloc(sizeof(BTKEYS));
        if (keys == NULL) {
            fprintf(stderr,"%s: get_keys: unable to allocate memory.\n",
                    prog);
            return NULL;
        }
    }
    else {
        keys = k;
    }
    if (src_file_ver > FULL_RECOVERY_VERSION) {
        for (j=0;j<nkeys;j++) {
            keys->keyblk[j] = ((btact->memrec)+idx)->keyblk[j];
        }
    }
    else {
        m4 = (MEMREC4*) (btact->memrec)+idx;
        for (j=0;j<nkeys;j++) {
            strncpy(keys->keyblk[j].key,m4->keyblk[j],ZKYLEN);
            keys->keyblk[j].key[ZKYLEN-1] = '\0';
            keys->keyblk[j].val = m4->valblk[j];
            keys->keyblk[j].dup = ZNULL;
        }
    }
    keys->nkeys = nkeys;
    return keys;
}

/* load_root_names reads in the keys from the superroot.  Since we
 * can't trust the index, only the superroot block is processed.
 * We assume that there will be fewer roots than ZMXKEY.  If this
 * assumption is incorrect, then roots are named using the root block
 * number (for btree index files >= FULL_RECOVERY_VERSION). 
 */

int load_superroot_names(BTA* in,int vlevel)
{
    int i, idx, nkeys;
    
    idx = load_block(in,ZSUPER,vlevel);
    if (idx < 0) return idx;
    nkeys = bgtinf(ZSUPER,ZNKEYS);
    superroot_keys = get_keys(idx,nkeys,NULL);
    if (vlevel >= 1 && nkeys > 0) {
        printf("\nAttempting to recover the following roots:\n");
        printf("%-32s %s\n","RootName","BlockNum");
        for (i=0; i<nkeys; i++) {
            printf("%-32s " ZINTFMT "\n",superroot_keys->keyblk[i].key,
                   superroot_keys->keyblk[i].val);
        }
        puts("");
    }
    return 0;
}

char* name_of_root(BTint blkno)
{
    int j;
    static char root_name[ZKYLEN];
    
    for (j=0;j<superroot_keys->nkeys;j++) {
        if (superroot_keys->keyblk[j].val == blkno) {
            return superroot_keys->keyblk[j].key;
        }
    }
    snprintf(root_name,ZKYLEN,"root_" ZINTFMT,blkno);
    root_name[ZKYLEN-1] = '\0';
    return root_name;
}

int valid_draddr(BTint draddr)
{
    BTint dblk;
    int offset,type;

    cnvdraddr(draddr,&dblk,&offset);
    type = bgtinf(dblk,ZBTYPE);
    return (dblk > ZSUPER &&
            dblk < btact->cntxt->super.sblkmx &&
            (type == ZDATA || type == ZDUP) &&
            offset >= 0 &&
            offset < ZBLKSZ);
}
 
int copy_data_record(BTA* in, BTA* out, BTA* da, char* key, BTint draddr,
                     int vlevel)
{
    int status,rsize;

    btact = in;
    if (!valid_draddr(draddr)) {
        if (vlevel >=3) {
            fprintf(stderr,"btr: invalid data address for key %s: 0x" ZINTFMT
                    "\n",key,draddr);
        }
        status = BAD_DRADDR;
        goto fin;
    }
    
    /* Call brecsz with BTA*, which will cause it to record all
     * draddrs, and error on a duplicate */
    rsize = (src_file_ver > FULL_RECOVERY_VERSION)?
        brecsz(draddr,da):
        btrrecsz(draddr,da);
    status = btgerr();
    if (status != 0) {
        print_bterror();
        status = DR_READ_ERROR;
        goto fin;
    }
    
    if (rsize > data_record_size) {
        free(data_record);
        data_record_size = rsize;
        kalloc(&data_record,data_record_size);
    }
    if (vlevel >= 3) {
        fprintf(stderr,"Reading data record for key: %s; draddr: " ZINTFMT
                " ", key, draddr);
    }
    rsize = (src_file_ver > FULL_RECOVERY_VERSION)?
        bseldt(draddr,data_record,rsize):
        btrseldt(draddr,data_record,rsize);
    status = btgerr();
    if (status == 0) {
        if (vlevel >= 3) {
            fprintf(stderr," (%d bytes read)\n",rsize);
        }
        stats.records++;
        status = btins(out,key,data_record,rsize);
    }
    else {
        print_bterror();
        status = DR_READ_ERROR;
        if (vlevel >= 3)
            fprintf(stderr,"\n");
    }
  fin:
    /* attempt to ignore (but count) errors on input
       side */
    if (status == QDLOOP) {
        /* mostly likely problem on input side; let's
           just copy the key */                        
        stats.seg_addr_loops++;
        status = DLOOP;
    }
    if (status == BAD_DRADDR) stats.bad_draddrs++;
    if (status == DR_READ_ERROR) stats.dr_read_errors++;
    if (status < 0) {
        /* copy_data_record can't access the data
         * record; insert key only, if so. */
        status = binsky(out,key,draddr);
    }
    if (status == 0) {
        stats.keys++;
    }
    return status;
}

int handle_dups (BTA* in, BTA* out, BTA* da, BTint blk, int mode,
                 char* current_root, BTint root, int vlevel)
{
    BTint draddr, mx;
    DKEY* dkey;
    int nkeys = 0;
    
    if (bgtinf(blk,ZBTYPE) != ZDUP) {
        return btgerr();
    }

    draddr = mkdraddr(blk,0);
    mx = mkdraddr(blk,bgtinf(blk,ZNKEYS));

    while (draddr < mx) {
        dkey = getdkey(draddr);
#if DEBUG >= 1
        fprintf(stderr,"handle_dups: draddr: " ZINTFMT ", dkey->key: %s, val: "
                ZINTFMT ", del: %d, blink: "
                ZINTFMT ", flink: " ZINTFMT "\n",
                draddr, dkey->key, dkey->val, dkey->deleted,
                dkey->blink, dkey->flink);
#endif  
        if (dkey == NULL) break;
        if (!dkey->deleted) {
            if (mode == DATA) {
                /* ignore return status */
                copy_data_record(in,out,da,dkey->key,
                                 dkey->val,vlevel);
            }
            else {
                if (binsky(out,dkey->key,dkey->val) != 0) break;
                stats.keys++;
            }
            /* reset btact to input file (btr digs into bt innards)*/
            btact = in;
            nkeys++;
        }
        draddr += sizeof(DKEY)+ZDOVRH;
    }
    return btgerr();
}
    

int copy_index(int mode, BTA *in, BTA *out, BTA *da, int vlevel, int ioerr_max,
               int allow_dups)
{
    int j,idx,status,block_type,nkeys;
    BTint blkno,root;
    BTKEYS* keys = NULL;
    char current_root[ZKYLEN+1];
    char root_name[ZKYLEN+1];
    char* btype[] = {
        "ZSUPER","ZROOT","ZINUSE","ZFREE","ZDATA","ZDUP"
    };  
    
    strncpy(current_root,"$$default",ZKYLEN);
    strncpy(root_name,"*UNKNOWN*",ZKYLEN);
    status = load_superroot_names(in,vlevel);
    if (!limited_recovery && status < 0) return status;
    
    for (blkno=1;blkno<BTINT_MAX;blkno++) {
       idx = load_block(in,blkno,vlevel);
        if (idx < 0 || stats.nioerrs > ioerr_max) {
            return idx;
        }
        stats.total_blocks++;
        nkeys = bgtinf(blkno,ZNKEYS);
        block_type = bgtinf(blkno,ZBTYPE);
        if (!limited_recovery) {
            root = bgtinf(blkno,ZNBLKS);
            if (root <= 0 ) root = 1;
        }
        else {
            root = ZNULL;
        }
        if (vlevel >= 2) {
            fprintf(stderr,"Processing block: " Z20DFMT
                    ", ZNKEYS: %8d [%s," ZINTFMT ",%6s]\n",blkno,nkeys,
                    current_root,root,btype[block_type]);
        }   
        if (block_type == ZROOT || block_type == ZINUSE) {
            /* TDB: perform various checks on block info */
            if (nkeys < 0 || nkeys > ZRNKEYS) {
                if (vlevel >= 2) {
                    fprintf(stderr,"btr: block: " ZINTFMT
                            ", bad ZNKEYS value: %d\n",
                            blkno,nkeys);
                }
                stats.bad_index_blocks++;
                continue;
            }
            stats.key_blocks++;
             /* copy keys from block; re-use keyblk */
            keys = get_keys(idx,nkeys,keys);
            if (!limited_recovery) {
                /* check for multi-root index */
                strncpy(root_name,name_of_root(root),ZKYLEN);
                root_name[ZKYLEN-1] = '\0';
                if (strcmp(root_name,current_root) != 0) {
                    /* attempt to switch to root; create if it
                     * doesn't exist */
                    status = btchgr(out,root_name);
                    if (status == QNOKEY) {
                        status = btcrtr(out,root_name);
                    }
                    if (status != 0) {
                        print_bterror();
                        return status;
                    }
                    if (allow_dups) {
                        status = btdups(out,TRUE);
                        if (status != 0) {
                            print_bterror();
                            return status;
                        }
                    }
                    strncpy(current_root,root_name,ZKYLEN);
                    current_root[ZKYLEN-1] = '\0';
                }   
            }
           /* insert into new btree file */
            for (j=0;j<nkeys;j++) {
                /* ignore dup key; dups will be inserted by ZDUP
                 * block handling */
                if (keys->keyblk[j].dup == ZNULL) {
                    if (mode == KEYS) {
                        stats.keys++;
                        status = binsky(out,keys->keyblk[j].key,
                                        keys->keyblk[j].val);
                    }
                    else if (mode == DATA) {
                        status = copy_data_record(in,out,da,keys->keyblk[j].key,
                                                  keys->keyblk[j].val,vlevel);
                    }
                    else {
                        fprintf(stderr,"%s: unknown copy mode: %d\n",prog,mode);
                        return 0;
                    }
                }
                if (status != 0) {
                    print_bterror();
                    return status;
                }
            }
        }
        else if (block_type == ZDUP) {
            /* handle dup key blocks */
            status = handle_dups(in,out,da,blkno,mode,current_root,root,vlevel);
            if (status != 0) return status;
            stats.dup_blocks++;
        }
        else if (block_type == ZFREE) {
            stats.free_blocks++;
        }
        else if (block_type == ZDATA) {
            stats.data_blocks++;
        }
        else {
            if (vlevel >= 3) {
                fprintf(stderr,"%s: ignoring block " ZINTFMT
                        " of unknown type 0x%x\n",prog,blkno,block_type);
                }   
            stats.bad_blocks++;
        }
    }
    return status;
}

int main(int argc, char *argv[])
{
    int exit_status;
    int more_args = TRUE;
    BTA *in, *out, *da = NULL;
    char current_root[ZKYLEN],*s;
    int copy_mode = KEYS;
    int ioerror_max = 0;
    int vlevel = 0;
    int preserve = TRUE;
    int allow_dups = FALSE;
    int status;
    int full_recovery = FALSE;
    char *infile;
    
    current_root[0] = '\0';
    s = strrchr(argv[0],'/');
    prog = (s==NULL)?argv[0]:(s+1);

    while (more_args && --argc > 0 && (*++argv)[0] == '-') {
        for (s=argv[0]+1; *s != '\0'; s++) {
            switch (*s) {
                case 'a':
                    allow_dups = TRUE;
                    break;
                case 'd':
                    copy_mode = DATA;
                    break;
                case 'f':
                    preserve = FALSE;
                    break;
                case 'k':
                    copy_mode = KEYS;
                    break;
                case 'n':
                    ioerror_max = atoi(*++argv);
                    break;
                case 'v':
                    vlevel++;
                    break;
                case 'r':
                    full_recovery = TRUE;
                case '-' :
                    more_args = FALSE;
                    ++argv;
                    break;
                 default:
                     fprintf(stderr,"%s: unknown switch: '-%c'\n"
                             "%s: usage: %s\n",prog,*s,prog,usage_str);
                     exit(EXIT_FAILURE);     
            }
        }
    }

    if (argc < 2) {
        fprintf(stderr,"%s: too few arguments.\n",prog);
        fprintf(stderr,"%s: usage: %s\n",prog,usage_str);
        exit(EXIT_FAILURE);
    }
    
    if (vlevel > 0) {
        fprintf(stderr,"BTree Recovery: %s\n",VERSION);
    }
        
    if (btinit() != 0) {
        print_bterror();
        return EXIT_FAILURE;
    }

    memset((void *) &stats,0,sizeof(stats));

    if (copy_mode == DATA) {
        /* create index for remembering disk addresses */
        da = btcrt(".bt_da.db",0,FALSE);
        if (da == NULL) {
            print_bterror();
            return EXIT_FAILURE;
        }
        kalloc(&data_record,data_record_size);
    }
    
    exit_status = EXIT_SUCCESS;
    infile = *argv++;
    if (preserve && file_exists(*argv)) {
        fprintf(stderr,"%s: target index file (%s) already exists.\n",
                prog,*argv);
        exit_status = EXIT_FAILURE;
    }
    else {
        /* attempt to open input file normally */
        in = btopn(infile,0,FALSE);
        if (in == NULL) {
            /* open file using fallback routine */
            in = btropn(infile,vlevel,full_recovery);
            if (in == NULL) {
                print_bterror();
                return EXIT_FAILURE;
            }
        }
        out = btcrt(*argv,0,FALSE);
        if (out == NULL) {
            print_bterror();
            return EXIT_FAILURE;
        }
        if (allow_dups) {
            status = btdups(out,TRUE);
            if (status != 0) {
                print_bterror();
                return EXIT_FAILURE;
            }
        }   
        status = copy_index(copy_mode,in,out,da,vlevel,ioerror_max,
                            allow_dups);
        if (vlevel >= 3 && status != EOF) {
            printf("btr: copy_index return status: %d\n",status);
        }
        btcls(in);
        btcls(out);
        if (da != NULL) btcls(da);
        puts("\nBTRecovery Statistics:");
        printf("  %-26s " Z20DFMT "\n","Total Blocks Processed:",
               stats.total_blocks);
        printf("  %-26s " Z20DFMT "\n","Key Blocks Processed:",
               stats.key_blocks);
        printf("  %-26s " Z20DFMT "\n","Keys Processed:",stats.keys);
        printf("  %-26s " Z20DFMT "\n","Data Blocks Encountered:",
               stats.data_blocks);
        printf("  %-26s " Z20DFMT "\n","Dup Blocks Processed:",
               stats.dup_blocks);
        printf("  %-26s " Z20DFMT "\n","Free Blocks Encountered:",
               stats.free_blocks);
        printf("  %-26s " Z20DFMT "\n","Data Records Processed:",stats.records);
        printf("  %-26s " Z20DFMT "\n","Data Record Read Errors:",
               stats.dr_read_errors);
        printf("  %-26s " Z20DFMT "\n","Record Address Loops:",
               stats.seg_addr_loops);
        printf("  %-26s " Z20DFMT "\n","Bad Blocks (invalid type):",
               stats.bad_blocks); 
        printf("  %-26s " Z20DFMT "\n","Bad Index Blocks:",
               stats.bad_index_blocks);
       if (vlevel > 0) {
            fprintf(stdout,"  %-26s %20d\n","I/O errors encountered:",
                    stats.nioerrs);
        }
    }
    
    return exit_status;
}
