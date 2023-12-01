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
	CXDRREWIND( int *ixdrid, int *retval )

#elif  (IFSTYLE==2)
	cxdrrewind( ixdrid, retval )
	int    	*ixdrid;
	int	*retval;

#else
	cxdrrewind_( int *ixdrid, int *retval )
#endif
{
	XDR 	*xdrs;
	u_int	position;

	*retval = 0;	/* No error */

	xdrs     = xdrfile[*ixdrid].xdrs;
	position = 0;

	if( ! xdr_setpos( xdrs, position ) ) {
		if( xdrfile[*ixdrid].return_on_error ) {
			*retval = FXDRERR_REWIND;
			return;
			}
		else
			{
			fprintf( stderr, "FXDR library error while trying to rewind ");
			fprintf( stderr, "file %s\n", 
				xdrfile[*ixdrid].filename );
			exit( -1 );
			}
		}
}
