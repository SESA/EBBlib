/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* (C) Copyright IBM Corp.  2007, 2007                              */
/* IBM CPL License                                                  */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file common/bgp_chipversion.h
 */

#ifndef	_BGP_CHIPVERSION_H_ // Prevent multiple inclusion
#define	_BGP_CHIPVERSION_H_



#include <common/namespace.h>

__BEGIN_DECLS

#define BGP_CHIPVERSION_DD2

#if defined BGP_CHIPVERSION_DD1
//  Settings for DD1
#define BGP_DD1_WORKAROUNDS 1

#elif defined BGP_CHIPVERSION_DD2
//  Settings for DD2

#else
// 
#error "Invalid chip version setting"

#endif


__END_DECLS



#endif // Add nothing below this line.
