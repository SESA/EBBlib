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
 * \file bpcore/bgp_types.h
 */

#ifndef _BGP_TYPES_H_  // Prevent multiple inclusion.
#define _BGP_TYPES_H_

#include <arch/ppc32/common/namespace.h>

__BEGIN_DECLS


#if !defined(__ASSEMBLY__) && !defined(__BGP_HIDE_STANDARD_TYPES__)

#include <arch/ppc32/common/alignment.h>

#ifdef _AIX
#include <inttypes.h>
#else
#include <stdint.h>
#endif

//#include <sys/types.h>

typedef  int8_t  _bgp_i8_t;
typedef uint8_t  _bgp_u8_t;
typedef  int16_t _bgp_i16_t;
typedef uint16_t _bgp_u16_t;
typedef  int32_t _bgp_i32_t;
typedef uint32_t _bgp_u32_t;
typedef  int64_t _bgp_i64_t;
typedef uint64_t _bgp_u64_t;

typedef union T_BGP_QuadWord
               {
               uint8_t   ub[ 16];
               uint16_t  us[  8];
               uint32_t  ul[  4];
               uint64_t ull[ 2];
               float      f[   4];
               double     d[   2];
               }
               ALIGN_QUADWORD _bgp_QuadWord_t;

typedef _bgp_QuadWord_t _QuadWord_t;

#endif // !__ASSEMBLY__ && !__BGP_HIDE_STANDARD_TYPES__

__END_DECLS

#endif // Add nothing below this line.

