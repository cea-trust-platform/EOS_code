/****************************************************************************
* Copyright (c) 2023, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/



#include <stdio.h>
#include <rpc/rpc.h>
#include "cfxdr.h"
#include <stdlib.h>
#include <string.h>
int cxdrgetid() ;

XDR_element     xdrfile[MAX_N_XDR_FILES] = {
        {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL},
        {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL},
        {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL},
        {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL},
        {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL},
        {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL},
        {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL},
        {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL}, {NULL, 0, NULL} };
        
        long
#if defined(cray_twobytecptrs)
        CXDRINIT( int *fname_len, char *filename, int *dummy, int *mode, int *return_on_err )

#elif (IFSTYLE==1)
        CXDRINIT( int *fname_len, char *filename, int *mode, int *return_on_err )

#elif (IFSTYLE==2)
        cxdrinit( fname_len, filename, mode, return_on_err )
        int     *fname_len;
        char    *filename;
        int     *mode;
        int     *return_on_err;

#else
        cxdrinit_( int *fname_len, char *filename, int *mode, int *return_on_err )
#endif
{
        FILE    *f;
        int     xdr_mode, new_xdr_id;
        XDR     *xdrs;
        char    local_filename[2048];

        strncpy( local_filename, filename, *fname_len );
        local_filename[*fname_len] = '\0';

        /* Get the new XDR ID to use when accessing this file */
        if( (new_xdr_id = cxdrgetid()) == -1 ) {
                if( *return_on_err ) {
                        return( -1 );
                        }
                else
                        {
                        fprintf( stderr, "Error while trying to open file %s\n",
                                        local_filename );
                        exit( -1 );
                        }
                }

        /* Save the file name of this XDR file, so that we
         * can print it out in case of errors.
         */
        xdrfile[new_xdr_id].filename = (char *)
                        malloc(sizeof(char)*(*fname_len + 1));
        strcpy( xdrfile[new_xdr_id].filename, local_filename );

        xdrs = (XDR *)malloc( sizeof( XDR ) );
        if( xdrs == NULL ) {
                if( *return_on_err ) {
                        return( -1 );
                        }
                else
                        {
                        fprintf( stderr, "Error on xdr stream create\n" );
                        fprintf( stderr, "filename=%s\n", local_filename );
                        exit( -1 );
                        }
                }

        if( *mode == 1 ) {
                xdr_mode = XDR_DECODE;
                if( (f = fopen( local_filename, "r" )) == NULL ) {
                        if( *return_on_err ) {
                                return( -1 );
                                }
                        else
                                {
                                fprintf( stderr, 
                                        "Error opening XDR file \"%s\" for reading\n",
                                        local_filename );
                                perror( "Reason" );
                                exit( -1 );
                                }
                        }
                }

        else if( *mode == 2 ) {
                xdr_mode = XDR_ENCODE;
                if( (f = fopen( local_filename, "w" )) == NULL ) {
                        if( *return_on_err ) {
                                return( -1 );
                                }
                        else
                                {
                                fprintf( stderr, 
                                        "Error opening XDR file \"%s\" for writing\n",
                                        local_filename );
                                perror( "Reason" );
                                exit( -1 );
                                }
                        }
                }

        else if( *mode == 3 ) {
                xdr_mode = XDR_ENCODE;
                if( (f = fopen( local_filename, "a" )) == NULL ) {
                        if( *return_on_err ) {
                                return( -1 );
                                }
                        else
                                {
                                fprintf( stderr, 
                                        "Error opening XDR file \"%s\" for appending\n",
                                        local_filename );
                                perror( "Reason" );
                                exit( -1 );
                                }
                        }
                }

        else    /* Wrong mode specified */
                {
                if( *return_on_err ) {
                        return( -1 );
                        }
                else
                        {
                        fprintf( stderr, "error: cxdrinit.c called with mode=%d\n", *mode );
                        exit( -1 );
                        }
                }
        
        xdrstdio_create( xdrs, f, (enum xdr_op) xdr_mode );

        xdrfile[new_xdr_id].return_on_error = *return_on_err;
        xdrfile[new_xdr_id].xdrs            = xdrs;

        /* Add the file pointer to the user's data area so
         * that we can close the file when the destroy routine
         * is called!
         */
        xdrs->x_public = (caddr_t)f;

        return( new_xdr_id );
}

int cxdrgetid()
{
        int     i;

        i = 0;
        while( xdrfile[i].filename != NULL ) {
                i++;
                if( i >= MAX_N_XDR_FILES ) {
                        fprintf( stderr, "FXDR library error: too many ");
                        fprintf( stderr, "XDR files open! Max is %d\n",
                                MAX_N_XDR_FILES );
                        return( -1 );
                        }
                }
        return( i );
}
