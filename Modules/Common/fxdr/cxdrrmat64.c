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

extern XDR_element xdrfile[MAX_N_XDR_FILES];

        void
#if (IFSTYLE==1)
        CXDRRMAT64( int *ixdrid, int *nels, float *r, int *retval )

#elif  (IFSTYLE==2)
        cxdrrmat64( ixdrid, nels, r, retval )
        int     *ixdrid;
        int     *nels;
        float   *r;
        int     *retval;

#else
        cxdrrmat64_( int *ixdrid, int *nels, float *r, int *retval )
#endif
{
        XDR     *xdrs;
        u_int   actual_nels, ne;

        *retval = 0;    /* No error */

        xdrs = xdrfile[*ixdrid].xdrs;

        if( xdrs->x_op == XDR_ENCODE ) {
                if( *nels < 0 ) {
                        if( xdrfile[*ixdrid].return_on_error ) {
                                *retval = FXDRERR_WRNEGNELS;
                                return;
                                }
                        else
                                {
                                fprintf( stderr, "FXDR library error in call to ");
                                fprintf( stderr, "write real64 array to file %s:\n",
                                                        xdrfile[*ixdrid].filename );
                                fprintf( stderr, "negative number of elements specified!\n" );
                                exit( -1 );
                                }
                        }
                ne = *nels;
                if( ! xdr_array( xdrs, (char **)&r, &ne, ne, 8, (xdrproc_t)xdr_double )) {
                        if( xdrfile[*ixdrid].return_on_error ) {
                                *retval = FXDRERR_WRITEERR;
                                return;
                                }
                        else
                                {
                                fprintf( stderr, "FXDR library error in call to ");
                                fprintf( stderr, "write real (64 bit) array to file %s:\n",
                                                xdrfile[*ixdrid].filename );
                                fprintf( stderr, "write error (disk full?)\n" );
                                exit( -1 );
                                }
                        }
                }
        else
                {
                ne = *nels;
                if( ! xdr_array( xdrs, (char **)&r, &actual_nels, ne, 8, (xdrproc_t)xdr_double )) {
                        if( xdrfile[*ixdrid].return_on_error ) {
                                *retval = FXDRERR_READERR;
                                return;
                                }
                        else
                                {
                                fprintf( stderr, "FXDR library error in call to ");
                                fprintf( stderr, "read real (64 bit) array from file %s:\n",
                                                xdrfile[*ixdrid].filename );
                                fprintf( stderr, "read error (end of file?)\n" );
                                exit( -1 );
                                }
                        }
                if( actual_nels != (u_int)*nels ) {
                        if( xdrfile[*ixdrid].return_on_error ) {
                                *retval = FXDRERR_READWRONGNELS;
                                return;
                                }
                        else
                                {
                                fprintf( stderr, "FXDR library error!  Asked for " );
                                fprintf( stderr, "%d real64 elements, but actually read %d!\n",
                                                *nels, actual_nels );
                                fprintf( stderr, "Error occured in file %s\n", 
                                        xdrfile[*ixdrid].filename );
                                exit( -1 );
                                }
                        }
                } 
}
