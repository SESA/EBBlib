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
 * \file common/alignment.h
 */

#ifndef	_ALIGNMENT_H_ // Prevent multiple inclusion
#define	_ALIGNMENT_H_



#include <arch/ppc32/common/namespace.h>

__BEGIN_DECLS

#if defined(__ASSEMBLY__)

#define ALIGN_L1_DIRTYBIT  3
#define ALIGN_QUADWORD     4
#define ALIGN_L1_CACHE     5
#define ALIGN_L1I_CACHE    5
#define ALIGN_L1D_CACHE    5
#define ALIGN_L3_CACHE     7

#elif defined(__GNUC__) || defined(__xlC__)

#define ALIGN_L1_DIRTYBIT __attribute__ ((aligned (  8)))
#define ALIGN_QUADWORD    __attribute__ ((aligned ( 16)))
#define ALIGN_L1_CACHE    __attribute__ ((aligned ( 32)))
#define ALIGN_L1I_CACHE   __attribute__ ((aligned ( 32)))
#define ALIGN_L1D_CACHE   __attribute__ ((aligned ( 32)))
#define ALIGN_L3_CACHE    __attribute__ ((aligned (128)))

#else

#warning "Need alignment directives for your compiler!"

#define ALIGN_QUADWORD
#define ALIGN_L1_CACHE
#define ALIGN_L1I_CACHE
#define ALIGN_L1D_CACHE
#define ALIGN_L3_CACHE

#endif // __ASSEMBLY__

__END_DECLS



#endif // Add nothing below this line
