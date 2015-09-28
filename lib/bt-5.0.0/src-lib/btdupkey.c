/*
 * $Id: btdupkey.c,v 1.19 2012/11/15 12:19:37 mark Exp $
 *
 *
 * btdupkey:  provides routines to handle duplicate key insertions,
 *            traversal and deletions. None form part of the BTLIB API.
 * 
 * Copyright (C) 2012 Mark Willson.
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

#include "btree.h"
#include "btree_int.h"
#include <string.h>

/* #undef DEBUG */
/* #define DEBUG 1 */

static KEYENT keyent;

/* Copy keyentry from MEMREC block to local keyent variable */

KEYENT* getkeyent(BTint blk, int pos)
{

    int idx,ioerr;
    
    ioerr = brdblk(blk,&idx);
    if (ioerr == 0) {
        keyent = ((btact->memrec)+idx)->keyblk[pos];
        return &keyent;
    }
    else {
        bterr("GETKEYENT",QRDBLK,itostr(blk)); 
        return NULL;
    }
}

/* Copy local keyent into MEMREC block; update # of writes */

int putkeyent(BTint blk, int pos)
{
    int ioerr,idx;
    
    ioerr = brdblk(blk,&idx);
    if (ioerr == 0) {
        ((btact->memrec)+idx)->keyblk[pos] = keyent;
    }
    else {
        bterr("PUTKEYENT",QRDBLK,itostr(blk)); 
        return QRDBLK;
    }
    ((btact->cntrl)+idx)->writes++;
    return 0;
}

/* Return pointer to dup key entry at draddr */

DKEY* getdkey(BTint draddr)
{
    static DKEY dkey;
    BTint blk;
    int offset,sz;

    cnvdraddr(draddr,&blk,&offset);
    if (bgtinf(blk,ZBTYPE) != ZDUP) {
        bterr("GETDKEY",QNOTDUP,itostr(draddr)); 
    }
    else {
        sz = bseldt(draddr,(char *) &dkey,sizeof(DKEY));
        if (sz == sizeof(DKEY)) {
            return &dkey;
        }
        else {
            bterr("GETDKEY",QDUPSZ,NULL); 
        }
    }
    return NULL;
}

/* Update dkey entry at draddr */

int putdkey(BTint draddr, DKEY* dkey)
{
    bupddt(draddr,(char *) dkey,sizeof(struct bt_dkey));
    return btgerr();
}

/* Add duplicate key to index */

int btdupkey(char *key, BTint val)
{
    int n =  btact->cntxt->lf.lfpos;
    BTint cblk = btact->cntxt->lf.lfblk;
    KEYENT* keyent;
    DKEY dkey;
    DKEY* dkeyp;
    BTint draddr;

    keyent = getkeyent(cblk,n);
    if (keyent == NULL) {
        bterr("BTDUPKEY",QRDBLK,itostr(cblk));
        return btgerr();
    }
    
    /* set dup entry invariants */
    strncpy(dkey.key,key,ZKYLEN);
    dkey.key[ZKYLEN-1] = '\0';
    dkey.deleted = FALSE;
    dkey.flink = ZNULL;
    
    /* if first duplicate, need to handle original key */
    if (keyent->dup == ZNULL) {
        /* construct duplicate key entry for original key */
        dkey.val = keyent->val;
        dkey.blink = ZNULL;
        draddr = binsdt(ZDUP,(char *) &dkey,sizeof(struct bt_dkey));
        if (draddr == ZNULL) {
            return btgerr();
        }
        keyent->val = draddr;
        keyent->dup = draddr;
#if DEBUG >= 1
        fprintf(stderr,"BTDUPKEY(1): keyent->key: %s, val: " ZINTFMT
                ", dup: " ZINTFMT "\n",keyent->key,keyent->val,keyent->dup);
#endif
    }
    /* add new duplicate key */
    dkey.val = val;
    dkey.blink = keyent->dup;
    draddr = binsdt(ZDUP,(char *) &dkey,sizeof(struct bt_dkey));
#if DEBUG >= 1
    fprintf(stderr,"BTDUPKEY(2): draddr: " ZINTFMT ", dkey->key: %s, val: "
            ZINTFMT ", del: %d, blink: "
            ZINTFMT ", flink: " ZINTFMT "\n",
            draddr, dkey.key, dkey.val, dkey.deleted,
            dkey.blink, dkey.flink);
#endif
    if (draddr == ZNULL) {
        return btgerr();
    }
    /* set flink of previous dup entry */
    dkeyp = getdkey(keyent->dup);
    if (dkeyp == NULL) return btgerr();
    dkeyp->flink = draddr;
    if (putdkey(keyent->dup,dkeyp) == 0) {
        /* set new dup list tail pointer */
        keyent->dup = draddr;
    }
#if DEBUG >= 1
    fprintf(stderr,"BTDUPKEY(3): draddr: " ZINTFMT ", dkeyp->key: %s, val: "
            ZINTFMT ", del: %d, blink: "
            ZINTFMT ", flink: " ZINTFMT "\n",
            draddr, dkeyp->key, dkeyp->val, dkeyp->deleted,
            dkeyp->blink, dkeyp->flink);
#endif
    putkeyent(cblk,n);
    return btgerr();;
}

/* return val from next/prev undeleted dup key
   return value: 0 = value returned; >0 error code; ZNULL no more dups
   keys
*/
int btduppos(int direction, BTint *val)
{
    BTint newaddr;
    DKEY* dkey;
    KEYENT* keyent;

    if (direction != ZNEXT && direction != ZPREV) {
        bterr("BTDUPPOS",QBADIR,NULL);
        return btgerr();
    }

    /* if not exact position, and not in dup chain, nothing to do */
    if (btact->cntxt->lf.draddr == ZNULL && !btact->cntxt->lf.lfexct) {
        return ZNULL;
    }
    
    if (btact->cntxt->lf.draddr == ZNULL) {
        /* not in dup key chain, but are we at the start or end of
           one? */
        if ((keyent = getkeyent(btact->cntxt->lf.lfblk,btact->cntxt->lf.lfpos))
            == NULL) return ZNULL;
#if DEBUG >= 1
        fprintf(stderr,"BTDUPPOS: keyent->key: %s, val: " ZINTFMT
                ", dup: " ZINTFMT "\n",keyent->key,keyent->val,keyent->dup);
#endif  
        if (keyent->dup == ZNULL) return ZNULL;
        newaddr = (direction==ZNEXT?keyent->val:keyent->dup);
        dkey = getdkey(newaddr);
        if (dkey == NULL) {
            return btgerr();
        }
    }   
    else {
        /* find next/prev non-deleted duplicate key */
        dkey = getdkey(btact->cntxt->lf.draddr);
        if (dkey == NULL) {
            /* if shared, then assume invalid dup address is because
             * the block has been freed/re-used. */
            if (btact->shared && btgerr() == QNOTDUP) {
                bterr("",0,NULL);
                return ZNULL;
            }
            return btgerr();
        }
        do {
            newaddr = (direction==ZNEXT?dkey->flink:dkey->blink);
            if (newaddr == ZNULL) {
                /* at beginning/end of chain; must move to next key */
                btact->cntxt->lf.lfexct = TRUE;
                btact->cntxt->lf.draddr = ZNULL;
                return ZNULL;
            }
            dkey = getdkey(newaddr);
            if (dkey == NULL) {
                return btgerr();
            }
#if DEBUG >= 1
            fprintf(stderr,"BTDUPPOS: draddr: " ZINTFMT ", dkey->key: %s, val: "
                    ZINTFMT ", del: %d, blink: "
                    ZINTFMT ", flink: " ZINTFMT "\n",
                    newaddr, dkey->key, dkey->val, dkey->deleted,
                    dkey->blink, dkey->flink);
#endif
        } while (dkey->deleted);
    }
        
    btact->cntxt->lf.draddr = newaddr;
    btact->cntxt->lf.lfexct = TRUE;
    *val = dkey->val;
    return 0;
}

/* Delete dkey entry; mark as deleted and update data block */

int deldkey(BTint draddr, DKEY* dkey)
{
    BTint blk;
    int offset;
    
    /* update deleted dup key */
    dkey->deleted = TRUE;
    if (putdkey(draddr,dkey) != 0) return btgerr();
    cnvdraddr(draddr,&blk,&offset);
    /* update used space in dup block; we expect deldat to leave
     * the data record intact */
    deldat(blk,offset);
    return 0;
}

/* Delete duplicate key, if it exists.  Return 0 for successful
 * duplicate key deletion, ZNULL for (apparently) nothing done, error
 * code otherwise.
 */

int btdeldup ()
{
    DKEY* dkey;
    KEYENT* keyent;
    BTint flink = ZNULL, blink = ZNULL, draddr = btact->cntxt->lf.draddr,
        cblk = btact->cntxt->lf.lfblk;
    int status = ZNULL, pos = btact->cntxt->lf.lfpos;
    
    /* either bfndky or btduppos will set context dup draddr if we
     * are at a duplicate key */
    
    if (btact->cntxt->lf.draddr != ZNULL) {
        status = 0;
        dkey = getdkey(draddr);
        if (dkey == NULL) return btgerr();
        keyent = getkeyent(cblk,pos);
        if (keyent == NULL) return btgerr();
        if (dkey->blink == ZNULL && dkey->flink == ZNULL) {
            /* last dup key entry. return status of ZNULL to indicate
             * index key to dup chain is to be deleted. */
            btact->cntxt->lf.draddr = ZNULL;
            status = ZNULL;  
        }
        else if (dkey->blink == ZNULL) {
            /* deleting first in chain */
            keyent->val = dkey->flink;
            flink = dkey->flink;
        }
        else if (dkey->flink == ZNULL) {
            /* deleting last in chain */
            keyent->dup = dkey->blink;
            blink = dkey->blink;
        }
        else {
            /* in middle of chain */
            flink = dkey->flink;
            blink = dkey->blink;
        }
        if (deldkey(draddr,dkey) != 0) return btgerr();;

        /* update previous/next entries as required */
        if (blink != ZNULL) {
            dkey = getdkey(blink);
            if (dkey == NULL) return btgerr();
            dkey->flink = flink;
            if (putdkey(blink,dkey) != 0) return btgerr();
        }
        if (flink != ZNULL) {
            dkey = getdkey(flink);
            if (dkey == NULL) return btgerr();
            dkey->blink = blink;
            if (putdkey(flink,dkey) != 0) return btgerr();
        }
        /* Mark inexact match, as we are pointing at a deleted key */
        btact->cntxt->lf.lfexct = FALSE;
        putkeyent(cblk,pos);
    }
    return status;
}

/* Display duplicate key entries in blk */
int btdispdups(BTint blk)
{
    BTint draddr, mx;
    DKEY* dkey;
    
    if (bgtinf(blk,ZBTYPE) != ZDUP) {
        return btgerr();
    }

    draddr = mkdraddr(blk,0);
    mx = mkdraddr(blk,bgtinf(blk,ZNKEYS));

    fprintf(stdout,"%20s%32s%20s%20s%20s%20s\n","Draddr","Key","Val","Del?",
            "Blink", "Flink");
    while (draddr < mx) {
        dkey = getdkey(draddr);
        if (dkey == NULL) break;
        fprintf(stdout,"" Z20DFMT "%32s" Z20DFMT "%20d" Z20DFMT Z20DFMT "\n",
                draddr, dkey->key, dkey->val, dkey->deleted, dkey->blink,
                dkey->flink);
        draddr += sizeof(DKEY)+ZDOVRH;
    }
    return btgerr();
}

/* Return # of keys in a dkey chain */

int btcntdups(BTint draddr)
{
    int count = 0;
    DKEY* dkey;

    while (draddr != ZNULL) {
        dkey = getdkey(draddr);
        count++;
        draddr = dkey->flink;
    }
    return count;
}

/* Return # of keys in an index block, including duplicates */

int btcntkeys(BTint blk)
{
    int tcnt = 0, kcnt, nkeys, i;
    KEYENT* keyent;
    
    nkeys = bgtinf(blk,ZNKEYS);
    for (i=0; i< nkeys; i++) {
        keyent = getkeyent(blk,i);
        if (keyent == NULL) return ZNULL;
        if (keyent->dup != ZNULL) {
            kcnt = btcntdups(keyent->val);
        }
        else {
            kcnt = 1;
        }
        tcnt += kcnt;
    }
    return tcnt;
}

/* update value of duplicate key.
 * return 0 if updated, ZNULL if nothing done, error code otherwise.
 */
int btdupupd(BTint val)
{
    DKEY* dkey;
    BTint draddr = btact->cntxt->lf.draddr;

    if (draddr != ZNULL) {
        dkey = getdkey(draddr);
        if (dkey == NULL) return btgerr();
        dkey->val = val;
        putdkey(draddr,dkey);
    }
    return draddr;
}

