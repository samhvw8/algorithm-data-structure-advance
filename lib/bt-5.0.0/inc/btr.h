/*
 * $Id: btr.h,v 1.2 2012/11/11 16:09:16 mark Exp $
 *
 * Copyright (C) 2011 Mark Willson.
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

/* Constants for bt v4 in-memory blocks  */

/* Info words in block for v4 MEMREC */
#define ZINFSZ4 5

/* number of keys per block */
#define ZMXKEY4 ((ZBLKSZ-ZBPW-ZINFSZ4*ZBPW)/(ZKYLEN+2*ZBPW)) 
/* number of pad words required */
#define ZPAD4 ((ZBLKSZ-(ZKYLEN+2*ZBPW)*ZMXKEY4-ZBPW-ZINFSZ4*ZBPW)/ZBPW)

/* bt v4 in-memory structure (v5 includes flag for duplicate key) */
struct bt_memrec4 {
   BTint infblk[ZINFSZ4];
   char keyblk[ZMXKEY4] [ZKYLEN];
   BTint valblk[ZMXKEY4];
   BTint lnkblk[ZMXKEY4+1];
#if ZPAD4 != 0
    BTint padblk[ZPAD4];
#endif
};

typedef struct bt_memrec4 MEMREC4;

/* Structure describing a data block (v4 and earlier) */

struct bt_datblk4 {
    BTint infblk[ZINFSZ4];
    char data[ZBLKSZ-(ZINFSZ4*ZBPW)];
};

typedef struct bt_datblk4 DATBLK4;
