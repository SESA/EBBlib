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
 * \file common/bgp_bitnumbers.h
 */

#ifndef _BGL_BITNUMBERS_H_  // Prevent multiple inclusion
#define _BGL_BITNUMBERS_H_

#include <arch/ppc32/common/namespace.h>

__BEGIN_DECLS

// These defines allows use of IBM's bit numberings (MSb=0, LSb=31)for multi-bit fields
//  b = IBM bit number of the least significant bit (highest number)
//  x = value to set in field
//  s = size
#define _BS(b,x,s)( ( ( x) & ( 0x7FFFFFFF>> ( 31- ( s)))) << ( 31- ( b)))
#define _BG(b,x,s)( ( _BS(b,0x7FFFFFFF,s) & x ) >> (31-b) )
#define _BS64(b,x,s)( ( ( x) & ( 0x7FFFFFFFFFFFFFFFLL>> ( 63- ( s)))) << ( 63- ( b)))
#define _BG64(b,x,s)( ( _BS64(b, 0x7FFFFFFFFFFFFFFFLL,s) & x ) >> (63-b) )
#define _BN(b)    ((1<<(31-(b))))
#define _B1(b,x)  (((x)&0x1)<<(31-(b)))
#define _B2(b,x)  (((x)&0x3)<<(31-(b)))
#define _B3(b,x)  (((x)&0x7)<<(31-(b)))
#define _B4(b,x)  (((x)&0xF)<<(31-(b)))
#define _B5(b,x)  (((x)&0x1F)<<(31-(b)))
#define _B6(b,x)  (((x)&0x3F)<<(31-(b)))
#define _B7(b,x)  (((x)&0x7F)<<(31-(b)))
#define _B8(b,x)  (((x)&0xFF)<<(31-(b)))
#define _B9(b,x)  (((x)&0x1FF)<<(31-(b)))
#define _B10(b,x) (((x)&0x3FF)<<(31-(b)))
#define _B11(b,x) (((x)&0x7FF)<<(31-(b)))
#define _B12(b,x) (((x)&0xFFF)<<(31-(b)))
#define _B13(b,x) (((x)&0x1FFF)<<(31-(b)))
#define _B14(b,x) (((x)&0x3FFF)<<(31-(b)))
#define _B15(b,x) (((x)&0x7FFF)<<(31-(b)))
#define _B16(b,x) (((x)&0xFFFF)<<(31-(b)))
#define _B17(b,x) (((x)&0x1FFFF)<<(31-(b)))
#define _B18(b,x) (((x)&0x3FFFF)<<(31-(b)))
#define _B19(b,x) (((x)&0x7FFFF)<<(31-(b)))
#define _B20(b,x) (((x)&0xFFFFF)<<(31-(b)))
#define _B21(b,x) (((x)&0x1FFFFF)<<(31-(b)))
#define _B22(b,x) (((x)&0x3FFFFF)<<(31-(b)))
#define _B23(b,x) (((x)&0x7FFFFF)<<(31-(b)))
#define _B24(b,x) (((x)&0xFFFFFF)<<(31-(b)))
#define _B25(b,x) (((x)&0x1FFFFFF)<<(31-(b)))
#define _B26(b,x) (((x)&0x3FFFFFF)<<(31-(b)))
#define _B27(b,x) (((x)&0x7FFFFFF)<<(31-(b)))
#define _B28(b,x) (((x)&0xFFFFFFF)<<(31-(b)))
#define _B29(b,x) (((x)&0x1FFFFFFF)<<(31-(b)))
#define _B30(b,x) (((x)&0x3FFFFFFF)<<(31-(b)))
#define _B31(b,x) (((x)&0x7FFFFFFF)<<(31-(b)))

#ifndef __ASSEMBLY__

// These defines ease extraction of bitfields.  (Not useful in assembler code.)
//  x = 32 bit value
//  b = IBM bit number of least significant bit of field
//  when b is a const, compiler should generate a single rotate-and-mask instruction
#define _GN(x,b)  (((x)>>(31-(b)))&0x1)
#define _G2(x,b)  (((x)>>(31-(b)))&0x3)
#define _G3(x,b)  (((x)>>(31-(b)))&0x7)
#define _G4(x,b)  (((x)>>(31-(b)))&0xF)
#define _G5(x,b)  (((x)>>(31-(b)))&0x1F)
#define _G6(x,b)  (((x)>>(31-(b)))&0x3F)
#define _G7(x,b)  (((x)>>(31-(b)))&0x7F)
#define _G8(x,b)  (((x)>>(31-(b)))&0xFF)
#define _G9(x,b)  (((x)>>(31-(b)))&0x1FF)
#define _G10(x,b) (((x)>>(31-(b)))&0x3FF)
#define _G11(x,b) (((x)>>(31-(b)))&0x7FF)
#define _G12(x,b) (((x)>>(31-(b)))&0xFFF)
#define _G13(x,b) (((x)>>(31-(b)))&0x1FFF)
#define _G14(x,b) (((x)>>(31-(b)))&0x3FFF)
#define _G15(x,b) (((x)>>(31-(b)))&0x7FFF)
#define _G16(x,b) (((x)>>(31-(b)))&0xFFFF)
#define _G17(x,b) (((x)>>(31-(b)))&0x1FFFF)
#define _G18(x,b) (((x)>>(31-(b)))&0x3FFFF)
#define _G19(x,b) (((x)>>(31-(b)))&0x7FFFF)
#define _G20(x,b) (((x)>>(31-(b)))&0xFFFFF)
#define _G21(x,b) (((x)>>(31-(b)))&0x1FFFFF)
#define _G22(x,b) (((x)>>(31-(b)))&0x3FFFFF)
#define _G23(x,b) (((x)>>(31-(b)))&0x7FFFFF)
#define _G24(x,b) (((x)>>(31-(b)))&0xFFFFFF)
#define _G25(x,b) (((x)>>(31-(b)))&0x1FFFFFF)
#define _G26(x,b) (((x)>>(31-(b)))&0x3FFFFFF)
#define _G27(x,b) (((x)>>(31-(b)))&0x7FFFFFF)
#define _G28(x,b) (((x)>>(31-(b)))&0xFFFFFFF)
#define _G29(x,b) (((x)>>(31-(b)))&0x1FFFFFFF)
#define _G30(x,b) (((x)>>(31-(b)))&0x3FFFFFFF)
#define _G31(x,b) (((x)>>(31-(b)))&0x7FFFFFFF)

#endif // __ASSEMBLY__

__END_DECLS

#endif // Add nothing below this line.

