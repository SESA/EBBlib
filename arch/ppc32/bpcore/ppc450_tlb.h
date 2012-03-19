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
 * \file bpcore/ppc450_tlb.h
 */

#ifndef _PPC450_TLB_H_ // Prevent multiple inclusion
#define _PPC450_TLB_H_



#include <common/namespace.h>

__BEGIN_DECLS

#include <common/bgp_bitnumbers.h>

#define _BGP_PPC450_TLB_SLOTS (64)

#define _BGP_VMM_SIZE_ERROR  (-1)
#define _BGP_VMM_SIZE_1K     ( 0)          // epn[0..21] (never used)
#define _BGP_VMM_SIZE_4K     ( 1)          // epn[0..19] (rarely used)
#define _BGP_VMM_SIZE_16K    ( 2)          // epn[0..17]
#define _BGP_VMM_SIZE_64K    ( 3)          // epn[0..15]
#define _BGP_VMM_SIZE_256K   ( 4)          // epn[0..13]
#define _BGP_VMM_SIZE_1M     ( 5)          // epn[0..11]
#define _BGP_VMM_SIZE_4M     ( 6)          // epn[0.. 9] (only ppc405 and slab)
#define _BGP_VMM_SIZE_16M    ( 7)          // epn[0.. 7]
#define _BGP_VMM_SIZE_64M    ( 8)          // epn[0.. 5] (neither ppc405 nor ppc440, only slab)
#define _BGP_VMM_SIZE_256M   ( 9)          // epn[0.. 3] (ppc440, ppc450)
#define _BGP_VMM_SIZE_1G     (10)          // epn[0.. 1] (only ppc450)

// Bit Masks for Effective Page Number by Page Size
#define _BGP_VMM_MASK_1K     (0xFFFFFC00) // save epn[0..21]
#define _BGP_VMM_MASK_4K     (0xFFFFF000) // save epn[0..19]
#define _BGP_VMM_MASK_16K    (0xFFFFC000) // save epn[0..17]
#define _BGP_VMM_MASK_64K    (0xFFFF0000) // save epn[0..15]
#define _BGP_VMM_MASK_256K   (0xFFFC0000) // save epn[0..13]
#define _BGP_VMM_MASK_1M     (0xFFF00000) // save epn[0..11]
#define _BGP_VMM_MASK_4M     (0xFFC00000) // save epn[0.. 9] (only ppc405)
#define _BGP_VMM_MASK_16M    (0xFF000000) // save epn[0.. 7]
#define _BGP_VMM_MASK_64M    (0xFC000000) // save epn[0.. 5] (neither ppc405 nor ppc440, only slab)
#define _BGP_VMM_MASK_256M   (0xF0000000) // save epn[0.. 3] (ppc440, ppc450)
#define _BGP_VMM_MASK_1G     (0xC0000000) // save epn[0.. 1] (only ppc450)


// Shift Counts for Effective Pages by Page Size
#define _BGP_VMM_SHIFT_1K    (10)         // get epn[0..21]
#define _BGP_VMM_SHIFT_4K    (12)         // get epn[0..19]
#define _BGP_VMM_SHIFT_16K   (14)         // get epn[0..17]
#define _BGP_VMM_SHIFT_64K   (16)         // get epn[0..15]
#define _BGP_VMM_SHIFT_256K  (18)         // get epn[0..13]
#define _BGP_VMM_SHIFT_1M    (20)         // get epn[0..11]
#define _BGP_VMM_SHIFT_4M    (22)         // get epn[0.. 9] (only ppc405)
#define _BGP_VMM_SHIFT_16M   (24)         // get epn[0.. 7]
#define _BGP_VMM_SHIFT_64M   (26)         // get epn[0.. 5] (neither ppc405 nor ppc440, only slab)
#define _BGP_VMM_SHIFT_256M  (28)         // get epn[0.. 3] (ppc440, ppc450)
#define _BGP_VMM_SHIFT_1G    (30)         // get epn[0.. 1] (only ppc450)


#define TLBW0            0                  // Word 0 of a TLB Entry
#define TLB0_EPN_1K(a)   ((a)&0xFFFFFC00)   //  EA[ 0:21]
#define TLB0_EPN_4K(a)   ((a)&0xFFFFF000)   //  EA[ 0:19]
#define TLB0_EPN_16K(a)  ((a)&0xFFFFC000)   //  EA[ 0:17]
#define TLB0_EPN_64K(a)  ((a)&0xFFFF0000)   //  EA[ 0:15]
#define TLB0_EPN_256K(a) ((a)&0xFFFC0000)   //  EA[ 0:13]
#define TLB0_EPN_1M(a)   ((a)&0xFFF00000)   //  EA[ 0:11]
#define TLB0_EPN_16M(a)  ((a)&0xFF000000)   //  EA[ 0: 7]
#define TLB0_EPN_256M(a) ((a)&0xF0000000)   //  EA[ 0: 3]
#define TLB0_EPN_1G(a)   ((a)&0xC0000000)   //  EA[ 0: 1]
#define TLB0_V          _BN(22)             //  Valid Bit
#define TLB0_TS         _BN(23)             //  Translation Address Space
#define TLB0_SIZE(x)    _B4(27,x)           //  Page Size
#define  TLB0_SIZE_1K    TLB0_SIZE( 0)
#define  TLB0_SIZE_4K    TLB0_SIZE( 1)
#define  TLB0_SIZE_16K   TLB0_SIZE( 2)
#define  TLB0_SIZE_64K   TLB0_SIZE( 3)
#define  TLB0_SIZE_256K  TLB0_SIZE( 4)
#define  TLB0_SIZE_1M    TLB0_SIZE( 5)
#define  TLB0_SIZE_16M   TLB0_SIZE( 7)
#define  TLB0_SIZE_256M  TLB0_SIZE( 9)
#define  TLB0_SIZE_1G    TLB0_SIZE(10)
#define TLB0_TPAR(x)    _B4(31,x)           // Tag Parity Bits (Only valid on tlbre, ignored on tlbwe)
#define TLB0_TID(t)     _B8(39,t)           // TID: to/from MMUCR[STID], not the GPR

#define TLBW1            1                  // Word 1 of a TLB Entry
#define TLB1_RPN_1K(p)   ((p)&0xFFFFFC00)   //  RPN[ 0:21]
#define TLB1_RPN_4K(p)   ((p)&0xFFFFF000)   //  RPN[ 0:19]
#define TLB1_RPN_16K(p)  ((p)&0xFFFFC000)   //  RPN[ 0:17]
#define TLB1_RPN_64K(p)  ((p)&0xFFFF0000)   //  RPN[ 0:15]
#define TLB1_RPN_256K(p) ((p)&0xFFFC0000)   //  RPN[ 0:13]
#define TLB1_RPN_1M(p)   ((p)&0xFFF00000)   //  RPN[ 0:11]
#define TLB1_RPN_16M(p)  ((p)&0xFF000000)   //  RPN[ 0: 7]
#define TLB1_RPN_256M(p) ((p)&0xF0000000)   //  RPN[ 0: 3]
#define TLB1_RPN_1G(p)   ((p)&0xC0000000)   //  RPN[ 0: 1]
#define TLB1_PAR1(x)     _B2(23,x)          //  Parity for TLB Word 1 (valid on tlbre, ignored on tlbwe)
#define TLB1_ERPN(e)     _B4(31,e)          //  Extended RPN: 4 MSb's of 36b Physical Address

#define TLBW2            2                  // Word 2 of a TLB Entry
#define TLB2_PAR2(x)     _B2(1,x)           //  Parity for TLB Word 2 (valid on tlbre, ignored on tlbwe)
#define TLB2_ZERO(x)     _B8(9,x)           //  Reserved Zeros
#define TLB2_FAR         _BN(10)            //  Fixed Address Region
#define TLB2_WL1         _BN(11)            //  Write-Thru L1        (when CCR1[L2COBE]=1)
#define TLB2_IL1I        _BN(12)            //  Inhibit L1-I caching (when CCR1[L2COBE]=1)
#define TLB2_IL1D        _BN(13)            //  Inhibit L1-D caching (when CCR1[L2COBE]=1)
#define TLB2_IL2I        _BN(14)            //  see below (on normal C450: Inhibit L2-I caching (when CCR1[L2COBE]=1)
#define TLB2_IL2D        _BN(15)            //  see below (on normal C450: Inhibit L2-D caching (when CCR1[L2COBE]=1)
#define TLB2_U0          _BN(16)            //  see below (undefined/available on normal C450
#define TLB2_U1          _BN(17)            //  User 1: L1 Transient Enable
#define TLB2_U2          _BN(18)            //  User 2: L1 Store WithOut Allocate
#define TLB2_U3          _BN(19)            //  see below (on normal C450: User 3: L3 Prefetch Inhibit (0=Enabled, 1=Inhibited)
#define TLB2_W           _BN(20)            //  Write-Thru=1, Write-Back=0
#define TLB2_I           _BN(21)            //  Cache-Inhibited=1, Cacheable=0
#define TLB2_M           _BN(22)            //  Memory Coherence Required
#define TLB2_G           _BN(23)            //  Guarded
#define TLB2_E           _BN(24)            //  Endian: 0=Big, 1=Little
#define TLB2_UX          _BN(26)            //  User       Execute Enable
#define TLB2_UW          _BN(27)            //  User       Write   Enable
#define TLB2_UR          _BN(28)            //  User       Read    Enable
#define TLB2_SX          _BN(29)            //  Supervisor Execute Enable
#define TLB2_SW          _BN(30)            //  Supervisor Write   Enable
#define TLB2_SR          _BN(31)            //  Supervisor Read    Enable

// BGP Specific controls
#define TLB2_IL3I        (TLB2_IL2I)        // L3 Inhibit for Instruction Fetches
#define TLB2_IL3D        (TLB2_IL2D)        // L3 Inhibit for Data Accesses
#define TLB2_IL2         (TLB2_U0)          // U0 is L2 Prefetch Inhibit
#define TLB2_T           (TLB2_U1)          // U1 Transient Enabled is supported.
#define TLB2_SWOA        (TLB2_U2)          // U2 Store WithOut Allocate is supported.
#define TLB2_L2_PF_OPT   (TLB2_U3)          // U3 is L2 Optimiztic Prefetch ("Automatic" when 0)

#if !defined(__ASSEMBLY__)

#include <bpcore/bgp_types.h>

typedef union T_BGP_TLB_Word0
               {
               uint32_t word;
               struct {
                      unsigned epn  : 22;
                      unsigned v    :  1;
                      unsigned ts   :  1;
                      unsigned size :  4;
                      unsigned tpar :  4;
                      };
                }
                _BGP_TLB_Word0;

typedef union T_BGP_TLB_Word1
               {
               uint32_t word;
               struct {
                      unsigned rpn  : 22;
                      unsigned par1 :  2;
                      unsigned rsvd :  4;
                      unsigned erpn :  4;
                      };
                }
                _BGP_TLB_Word1;

typedef union T_BGP_TLB_Word2
               {
               uint32_t word;
               struct {
                      unsigned par2 : 2;
                      unsigned zero : 8;
                      unsigned far  : 1;
                      unsigned wl1  : 1;
                      unsigned il1i : 1;
                      unsigned il1d : 1;
                      unsigned il2i : 1;
                      unsigned il2d : 1;
                      unsigned u0   : 1;
                      unsigned u1   : 1;
                      unsigned u2   : 1;
                      unsigned u3   : 1;
                      unsigned w    : 1;
                      unsigned i    : 1;
                      unsigned m    : 1;
                      unsigned g    : 1;
                      unsigned e    : 1;
                      unsigned r25  : 1;
                      unsigned ux   : 1;
                      unsigned uw   : 1;
                      unsigned ur   : 1;
                      unsigned sx   : 1;
                      unsigned sw   : 1;
                      unsigned sr   : 1;
                      };
                }
                _BGP_TLB_Word2;

typedef struct T_BGP_TLB_Entry
                {
                _BGP_TLB_Word0 w0;
                _BGP_TLB_Word1 w1;
                _BGP_TLB_Word2 w2;
                uint32_t       pid;  // pid[24:31] actually MMUCR[STID]
                }
                _BGP_TLB_Entry;

// this structure is only used by the bootloader for statically
//  defined TLBs in bootloader::bgp_tlb_table.c, who's contents
//  are loaded in bootloader::bgp_bootloader.S early chip init.
typedef struct T_BGP_TLB_Data
                {
                _BGP_TLB_Word0 w0;
                _BGP_TLB_Word1 w1;
                _BGP_TLB_Word2 w2;
                }
                _BGP_TLB_Data;


// Returns the TLBE index (if mapped) or -1 otherwise
static inline int _bgp_isMapped( void* address ) {

  do {
    int tlbIndex = -1;
    asm volatile (
		  "  tlbsx. %0,0,%1;"
		  "  beq   1f;"       // conditional store succeeded
		  "  li    %0,-1;"
		  "1:;"
		  : "=r" (tlbIndex)
		  : "r"  (address)
		  : "cc", "memory" );
    return(tlbIndex);
  }
  while(0);
}

#endif // __ASSEMBLY__

__END_DECLS



#endif // Add nothing below this line.

