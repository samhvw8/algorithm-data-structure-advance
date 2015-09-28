/*
 * 	NAME
 *      corrupt - corrupt files for btr testing
 *
 * 	SYNOPSIS
 *      corrupt filename offset length [character]
 *
 * 	DESCRIPTION
 *      Clobbers part of a file (defined by byte offset and length)
 *      with character.  If character is not provided, \0 is the
 *      default.
 *
 * 	NOTES
 *      Only the righteous files will survive.
 *
 * 	MODIFICATION HISTORY
 * 	Mnemonic	Rel	Date	Who
 *  corrupt     1.0 110621  mpw
 * 		Written.
 *
 *  $Id: corrupt.c,v 1.3 2011/06/23 10:01:33 mark Exp $		
 */  

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int offset, length;
    char ch = '\0';
    char *buf;
    char *filename;
    FILE* f;

    if (argc < 4 || argc > 5) {
        fprintf(stderr,"corrupt: usage: corrupt filename offset length [character]");
        return EXIT_FAILURE;
    }
    filename = argv[1];
    offset = strtol(argv[2],NULL,10);
    length = strtol(argv[3],NULL,10);
    if (argc > 4) ch = argv[4][0];

    f = fopen(filename,"r+b");
    if (f == NULL) {
        fprintf(stderr,"corrupt: unable to open file: %s\n",filename);
        return EXIT_FAILURE;
    }

    if (fseek(f,offset,SEEK_SET) != 0){
        fprintf(stderr,"corrupt: fseek failure.\n");
        return EXIT_FAILURE;
    }
    buf = malloc(length);
    if (buf == NULL) {
        fprintf(stderr,"corrupt: unable to allocate memory.");
        return EXIT_FAILURE;
    }
    memset(buf,ch,length);
    if (fwrite(buf,1,length,f) != length) {
        fprintf(stderr,"corrupt: did not write %d characters.",length);
        return EXIT_FAILURE;
    }
    fclose(f);
    return EXIT_SUCCESS;
}

            
    
    
