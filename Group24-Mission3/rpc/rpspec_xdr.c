/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpspec.h"

bool_t
xdr_arga (XDR *xdrs, arga *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->arga1))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->arga2))
		 return FALSE;
	return TRUE;
}