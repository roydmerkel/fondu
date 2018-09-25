/* Copyright (C) 2018-2018 by Roy Merkel */
/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <fondu/unbin.h>

/* A MacBinary file */
/*  http://www.lazerware.com/formats/macbinary.html */
/*    begins with a 128 byte header */
/*	(which specifies lengths for data/resource forks) */
/*	(and contains mac type/creator data) */
/*	(and other stuff) */
/*	(and finally a crc checksum)
/*    is followed by the data section (padded to a mult of 128 bytes) */
/*    is followed by the resource section (padded to a mult of 128 bytes) */



/* frombin filenames */

int GetUnbinHandles(char *filename, FILE ** dataHandle, int *dataLength, FILE** resourceHandle, int *resourceLength, unsigned char (*outHeader)[128], char (*outName)[80]) {
    FILE *binfile;
    int i,ch;

    if(dataHandle == NULL)
    {
        fprintf( stderr, "INTERNAL: Null data handle given\n");
        return 0;
    }

    if(dataLength == NULL)
    {
        fprintf( stderr, "INTERNAL: Null data handle given\n");
        return 0;
    }

    if(resourceHandle == NULL)
    {
        fprintf( stderr, "INTERNAL: Null resource handle given\n");
        return 0;
    }
    
    if(resourceLength == NULL)
    {
        fprintf( stderr, "INTERNAL: Null data handle given\n");
        return 0;
    }

    if(outHeader == NULL)
    {
        fprintf( stderr, "INTERNAL: Null output header given\n");
        return 0;
    }
    
    if(outName == NULL)
    {
        fprintf( stderr, "INTERNAL: Null output name given\n");
        return 0;
    }
    
    *dataHandle = NULL;
    *resourceHandle = NULL;

    binfile = fopen(filename,"r");
    if ( binfile==NULL ) {
        fprintf( stderr, "Cannot open %s\n", filename);
        return 0;
    }

    fread(*outHeader,1,sizeof(*outHeader),binfile);
    if ( (*outHeader)[0]!=0 || (*outHeader)[74]!=0 || (*outHeader)[82]!=0 || (*outHeader)[1]<=0 ||
            (*outHeader)[1]>33 || (*outHeader)[63]!=0 || (*outHeader)[2+(*outHeader)[1]]!=0 ) {
        fprintf( stderr, "%s does not look like a macbinary file\n", filename );
        fclose(binfile);
        return 0;
    }
    strncpy((*outName),(char *) (*outHeader)+2,(*outHeader)[1]);
    (*outName)[(*outHeader)[1]] = '\0';
    (*dataLength) = (((*outHeader)[0x53]<<24)|((*outHeader)[0x54]<<16)|((*outHeader)[0x55]<<8)|(*outHeader)[0x56]);
    (*resourceLength) = (((*outHeader)[0x57]<<24)|((*outHeader)[0x58]<<16)|((*outHeader)[0x59]<<8)|(*outHeader)[0x5a]);
fprintf( stderr, " %s => %s, dfork len=%d rfork len=%d\n", filename, (*outName), (*dataLength), (*resourceLength) );

    {
        if ( (*dataLength)>0 ) {
            *dataHandle = fopen(filename,"r");
            if ( *dataHandle==NULL ) {
                fprintf( stderr, "Cannot open %s\n", filename);
                fclose(binfile);
                return 0;
            }
            fseek(*dataHandle,128,SEEK_SET);
        }

        if ( (*resourceLength)>0 ) {
            *resourceHandle = fopen(filename,"r");
            if ( *resourceHandle==NULL ) {
                fprintf( stderr, "Cannot open %s\n", filename);
                fclose(binfile);
                fclose(*dataHandle);
                *dataHandle = NULL;
                return 0;
            }
            fseek(*resourceHandle,128 + (((*dataLength)+127)&~127),SEEK_SET);
        }
    }
    fclose(binfile);
    return 1;
}

