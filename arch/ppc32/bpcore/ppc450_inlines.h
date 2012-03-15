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
 * \file bpcore/ppc450_inlines.h
 */

#ifndef	_PPC450_INLINES_H_ // Prevent multiple inclusion
#define	_PPC450_INLINES_H_



#include <arch/ppc32/common/namespace.h>

#if !defined(__ASSEMBLY__) && !defined(__BGP_HOST_COMPILED__) && !defined(__MAMBO__)

__BEGIN_DECLS

#include <arch/ppc32/bpcore/bgp_types.h>
#include <arch/ppc32/bpcore/ppc450_core.h>
//#include <arch/ppc32/bpcore/cnk/VirtualMap.h>
//#include <arch/ppc32/bpcore/cnk/bgp_cnk_Tweaks.h>

extern inline uint32_t _bgp_cntlz( uint32_t v )
{
   uint32_t rval;

   asm volatile ("cntlzw %0,%1" : "=r" (rval) : "r" (v) );

   return( rval );
}

extern inline uint32_t _bgp_mfmsr( void )
{
   uint32_t tmp;

   do {
      asm volatile ("mfmsr %0": "=&r" (tmp) : : "memory" );
      }
      while(0);

   return( tmp );
}

extern inline void _bgp_mtmsr( uint32_t value )
{
   do {
      asm volatile ("mtmsr %0": : "r" (value) : "memory" );
      }
      while(0);
}


#define _bgp_mfspr( SPRN )\
({\
   unsigned int tmp;\
   do {\
      asm volatile ("mfspr %0,%1" : "=&r" (tmp) : "i" (SPRN) : "memory" );\
      }\
      while(0);\
   tmp;\
})\

#define _bgp_mtspr( SPRN, value ) do { asm volatile ("mtspr %0,%1": : "i" (SPRN), "r" (value) : "memory" ); } while(0)


#define _bgp_msync(void)       do { asm volatile ("msync" : : : "memory"); } while(0)

extern inline void _bgp_msync_nonspeculative( void )
{
    do {
       asm volatile ("   b 1f;"
                     "   nop;"
                     "1: msync;"
                     : : : "memory");
       }
       while(0);
}

#define _bgp_mbar(void)        do { asm volatile ("mbar"  : : : "memory"); } while(0)

extern inline void _bgp_mbar_nonspeculative( void )
{
    do {
       asm volatile ("   b 1f;"
                     "   nop;"
                     "1: mbar;"
                     : : : "memory");
       }
       while(0);
}

#define _bgp_isync(void)       do { asm volatile ("isync" : : : "memory"); } while(0)
#define _bgp_rfi(void)         do { asm volatile ("rfi"   : : : "memory"); } while(0)
#define _bgp_dccci(void)       do { asm volatile ("dccci 0,0" : : : "memory"); } while(0)

// Privileged DCR access when ADDR is < 10 bits in length (Use *x versions below instead)
#define _bgp_mfdcr(DCRN)       ({ uint32_t VALUE; asm volatile ("mfdcr %0,%1": "=r" (VALUE) : "i" (DCRN)); VALUE; })
#define _bgp_mtdcr(DCRN,VALUE) { asm volatile("mtdcr %0,%1": :"i" (DCRN), "r" (VALUE)); }

// Privileged instruction for accessing Privileged or User accessible DCRs
extern inline uint32_t _bgp_mfdcrx( uint32_t dcrn )
{
   uint32_t value;

   do {
      asm volatile ("mfdcrx %0,%1": "=r" (value) : "r" (dcrn) : "memory" );
      }
      while(0);

   return( value );
}

extern inline void _bgp_mtdcrx( uint32_t dcrn, uint32_t value )
{
   do {
      asm volatile("mtdcrx %0,%1": :"r" (dcrn), "r" (value) : "memory" );
      }
      while(0);
}


// Non-Privileged instruction for accessing User accessible DCRs
extern inline uint32_t _bgp_mfdcrux( uint32_t dcrn )
{
   uint32_t value;

   do {
      asm volatile ("mfdcrux %0,%1": "=r" (value) : "r" (dcrn) : "memory" );
      }
      while(0);

   return( value );
}

extern inline void _bgp_mtdcrux( uint32_t dcrn, uint32_t value )
{
   do {
      asm volatile("mtdcrux %0,%1": :"r" (dcrn), "r" (value) : "memory" );
      }
      while(0);
}


//
// _bgp_mttlb: Install a TLB entry.
//
//  This version is interrupt-safe during the update, as it first invalidates
//   the slot, fills-in words 1 and 2, then validates by writing word 0.
//
extern inline void _bgp_mttlb( uint32_t slot, uint32_t w0, uint32_t w1, uint32_t w2 )
{
   uint32_t zero = 0;

   do { asm volatile ("tlbwe %4,%0,0;"
                      "tlbwe %2,%0,1;"
                      "tlbwe %3,%0,2;"
                      "tlbwe %1,%0,0;"
                      :
                      : "b" (slot),
                        "r" (w0),
                        "r" (w1),
                        "r" (w2),
                        "r" (zero)
                      : "memory" );
      } while(0);
}

#if 0
// slightly unsafe version should an interrupt happen between the update of
//  word 0 and words 1,2.
#define _bgp_mttlb(slot, w0, w1, w2) do { asm volatile ("tlbwe %1,%0,0;" \
                                                        "tlbwe %2,%0,1;" \
                                                        "tlbwe %3,%0,2;" \
                                                        :                \
                                                        : "r" (slot),    \
                                                          "r" (w0),      \
                                                          "r" (w1),      \
                                                          "r" (w2)       \
                                                        : "memory" );    \
                                        } while(0)
#endif

#define _bgp_mftlb(slot, w0, w1, w2) do { asm volatile ("tlbre %0,%3,0;" \
                                                        "tlbre %1,%3,1;" \
                                                        "tlbre %2,%3,2;" \
                                                        : "=&r" (w0),     \
                                                          "=&r" (w1),     \
                                                          "=&r" (w2)      \
                                                        : "r" (slot)     \
                                                        : "memory" );    \
                                        } while(0)


#define _bgp_dcache_zero_line_index(v,i) do { asm volatile ("dcbz %1,%0" : : "r" (v), "Ob" (i) : "memory"); } while(0)
#define _bgp_dcache_zero_line(v)         do { asm volatile ("dcbz  0,%0" : : "r" (v) : "memory"); } while(0)
#define _bgp_dcache_flush_line(v)        do { asm volatile ("dcbf  0,%0" : : "r" (v) : "memory"); } while(0)
#define _bgp_dcache_store_line(v)        do { asm volatile ("dcbst 0,%0" : : "r" (v) : "memory"); } while(0)
#define _bgp_dcache_touch_line(v)        do { asm volatile ("dcbt  0,%0" : : "r" (v)           ); } while(0)
#define _bgp_dcache_invalidate_line(v)   do { asm volatile ("dcbi  0,%0" : : "r" (v) : "memory"); } while(0)
#define _bgp_icache_invalidate_line(v)   do { asm volatile ("icbi  0,%0" : : "r" (v) : "memory"); } while(0)
#define _bgp_icache_touch_line(v)        do { asm volatile ("icbt  0,%0" : : "r" (v)           ); } while(0)
#define _bgp_dcache_invalidate_all(void) do { asm volatile ("dccci 0,0"  : : : "memory"); } while(0)
#define _bgp_icache_invalidate_all(void) do { asm volatile ("iccci 0,0"  : : : "memory"); } while(0)

#ifndef __xlC__
extern inline void _bgp_dcache_store_bytes( uint32_t addr, uint32_t bytes )
{
   uint32_t p = (addr & L1D_CACHE_MASK);
   uint32_t b = ((bytes + (L1D_CACHE_LINE_SIZE-1)) & L1D_CACHE_MASK);
   uint32_t ofs;

   for ( ofs = 0 ; ofs < b ; ofs += L1D_CACHE_LINE_SIZE )
      {
      asm volatile ("dcbst %1,%0" : : "b" (p), "Ob" (ofs) : "memory");
      }
}

extern inline void _bgp_dcache_touch_bytes( uint32_t addr, uint32_t bytes )
{
   uint32_t p = (addr & L1D_CACHE_MASK);
   uint32_t b = ((bytes + (L1D_CACHE_LINE_SIZE-1)) & L1D_CACHE_MASK);
   uint32_t ofs;

   for ( ofs = 0 ; ofs < b ; ofs += L1D_CACHE_LINE_SIZE )
      {
      asm volatile ("dcbt %1,%0" : : "b" (p), "Ob" (ofs) : "memory");
      }
}

extern inline void _bgp_dcache_invalidate_bytes( uint32_t addr, uint32_t bytes )
{
   uint32_t p = (addr & L1D_CACHE_MASK);
   uint32_t b = ((bytes + (L1D_CACHE_LINE_SIZE-1)) & L1D_CACHE_MASK);
   uint32_t ofs;

   for ( ofs = 0 ; ofs < b ; ofs += L1D_CACHE_LINE_SIZE )
      {
      asm volatile ("dcbi %1,%0" : : "b" (p), "Ob" (ofs) : "memory");
      }
}

extern inline void _bgp_dcache_flush_bytes( uint32_t addr, uint32_t bytes )
{
   uint32_t p = (addr & L1D_CACHE_MASK);
   uint32_t b = ((bytes + (L1D_CACHE_LINE_SIZE-1)) & L1D_CACHE_MASK);
   uint32_t ofs;

   for ( ofs = 0 ; ofs < b ; ofs += L1D_CACHE_LINE_SIZE )
      {
      asm volatile ("dcbf %1,%0" : : "b" (p), "Ob" (ofs) : "memory");
      }
}

#endif
extern inline void _bgp_dcache_flush_all( void )
{
   uint32_t blind = _BGP_VA_BLIND;
   uint32_t l0 = 0;
   uint32_t l1 = (L1D_CACHE_LINE_SIZE * 1);
   uint32_t l2 = (L1D_CACHE_LINE_SIZE * 2);
   uint32_t l3 = (L1D_CACHE_LINE_SIZE * 3);
   int i = (L1D_CACHE_LINES / 4);

   while( i-- )
      {
      asm volatile ("dcbz %0,%1;"
                    "dcbz %0,%2;"
                    "dcbz %0,%3;"
                    "dcbz %0,%4;"
                    :               /* no outputs */
                    : "b" (blind),  /* inputs     */
                      "b" (l0),
                      "b" (l1),
                      "b" (l2),
                      "b" (l3)
                    : "memory"  ); /* clobbers    */

      blind += (L1D_CACHE_LINE_SIZE * 4);
      }
}

extern inline void _bgp_dcache_flush_all_va( uint32_t vaddr )
{
   uint32_t buffer = vaddr;
   uint32_t l0 = 0;
   uint32_t l1 = (L1D_CACHE_LINE_SIZE * 1);
   uint32_t l2 = (L1D_CACHE_LINE_SIZE * 2);
   uint32_t l3 = (L1D_CACHE_LINE_SIZE * 3);
   int i = (L1D_CACHE_LINES / 4);

   while( i-- )
      {
      asm volatile ("dcbt %0,%1;"
                    "dcbt %0,%2;"
                    "dcbt %0,%3;"
                    "dcbt %0,%4;"
                    :               /* no outputs */
                    : "b" (buffer),  /* inputs     */
                      "b" (l0),
                      "b" (l1),
                      "b" (l2),
                      "b" (l3)
                    : "memory"  ); /* clobbers    */

      buffer += (L1D_CACHE_LINE_SIZE * 4);
      }
}


// This routine flushes the L1 D-cache set that contains vaddr.
//  vaddr can be anywhere in the L1 set.
//  each L1 set is 64 lines.
extern inline void _bgp_dcache_flush_set( uint32_t vaddr )
{
#define FLUSH_INCR (0x0200)
   uint32_t blind = (_BGP_VA_BLIND + (vaddr & 0x01e0));
   uint32_t l0 = 0;
   uint32_t l1 = (FLUSH_INCR * 1);
   uint32_t l2 = (FLUSH_INCR * 2);
   uint32_t l3 = (FLUSH_INCR * 3);
   int i = (64 / 4);

   while( i-- )
      {
      asm volatile ("dcbt %0,%1;"
                    "dcbt %0,%2;"
                    "dcbt %0,%3;"
                    "dcbt %0,%4;"
                    :               /* no outputs */
                    : "b" (blind),  /* inputs     */
                      "b" (l0),
                      "b" (l1),
                      "b" (l2),
                      "b" (l3)
                    : "memory"  ); /* clobbers    */

      blind += (FLUSH_INCR * 4);
      }
}

// Lock/Stick support
extern inline uint32_t _bgp_LoadReserved( volatile uint32_t *pVar )
{
   do {
      register uint32_t Val;

      asm volatile ("lwarx   %[rc],0,%[pVar];"
                    : [rc] "=&b" (Val)
                    : [pVar] "b" (pVar)
                    : "memory" );

      return( Val );
      }
      while( 0 );
}

// Lock/Stick support: does msync before the lwarx
extern inline uint32_t _bgp_LoadReservedMsync( volatile uint32_t *pVar )
{
   do {
      register uint32_t Val;

      asm volatile ("msync;"
                    "lwarx   %[rc],0,%[pVar];"
                    : [rc] "=&b" (Val)
                    : [pVar] "b" (pVar)
                    : "memory" );

      return( Val );
      }
      while( 0 );
}


#if ( _BGP_LOCKSTICK_WORKAROUND )  // see BPC Bugs Issue 816.

// Returns: 0 = Conditional Store Failed,
//          1 = Conditional Store Succeeded.
extern inline int _bgp_StoreConditional( volatile uint32_t *pVar, uint32_t Val )
{
   // before the stick, we must cause a store to be visable on the bus.

   do {
      int rc = 1; // assume success

      _bgp_dcache_store_line( &rc );
      _bgp_mbar();

      asm volatile ("  dccci   0,0;"
                    "  stwcx.  %2,0,%1;"
                    "  beq     1f;"       // conditional store succeeded
                    "  li      %0,0;"
                    "1:;"
                    : "=b" (rc)
                    : "b"  (pVar),
                      "b"  (Val),
                      "0"  (rc)
                    : "cc", "memory" );
      return(rc);
      }
      while(0);
}

#else // not _BGP_LOCKSTICK_WORKAROUND

//#warning "Lock-n-Stick Work-Around is disabled!"

// Returns: 0 = Conditional Store Failed,
//          1 = Conditional Store Succeeded.
extern inline int _bgp_StoreConditional( volatile uint32_t *pVar, uint32_t Val )
{
   do {
      register int rc = 1; // assume success

      asm volatile ("  stwcx.  %2,0,%1;"
                    "  beq     1f;"       // conditional store succeeded
                    "  li      %0,0;"
                    "1:;"
                    : "=b" (rc)
                    : "b"  (pVar),
                      "b"  (Val),
                      "0"  (rc)
                    : "cc", "memory" );

      return( rc );
      }
      while(0);
}

#endif // _BGP_LOCKSTICK_WORKAROUND


// Non-Privileged!
extern inline uint64_t _bgp_GetTimeBase( void )
{
   union {
         uint32_t ul[2];
         uint64_t ull;
         }
         hack;
   uint32_t utmp;

   do {
      utmp       = _bgp_mfspr( SPRN_TBRU );
      hack.ul[1] = _bgp_mfspr( SPRN_TBRL );
      hack.ul[0] = _bgp_mfspr( SPRN_TBRU );
      }
      while( utmp != hack.ul[0] );

   return( hack.ull );
}

// Privileged!
extern inline void _bgp_SetTimeBase( uint64_t pclks )
{
   union {
         uint32_t ul[2];
         uint64_t ull;
         }
         hack;

   hack.ull = pclks;

   do {
      uint32_t old_msr = _bgp_mfmsr(); // ensure no interrupts during update
      _bgp_mtmsr( 0 );
      _bgp_mtspr( SPRN_TBWL, 0 );           // ensure no roll over while updating
      _bgp_mtspr( SPRN_TBWU, hack.ul[0] );
      _bgp_mtspr( SPRN_TBWL, hack.ul[1] );
      _bgp_mtmsr( old_msr );                // restore original msr
      }
      while( 0 );
}

// read my CPU ID (0 to 3) from SPRN_PIR[ PIR_PIN ]
extern inline int _bgp_PhysicalProcessorID( void )
{
   return( (int)(_bgp_mfspr(SPRN_PIR) & PIR_PIN(0xFF)) );
}

// read the PPC450 Processor Version
extern inline uint32_t _bgp_ProcessorVersion( void )
{
   return( (int)(_bgp_mfspr(SPRN_PVR)) );
}

#define _bgp_CurrentStackAddress() ({uint32_t tmp; asm volatile("la %0,0(1)": "=r" (tmp)) ;tmp;})

// single inline no-op instruction
extern inline void _bgp_nop( void )
{
   do {
      asm volatile("nop;");
      }
      while(0);
}

extern inline void _bgp_Delay( uint32_t pclks )
{
   while( pclks-- )
      {
      asm volatile("nop;");
      }
}

extern inline void _bgp_Delay_ns( uint32_t nsecs, uint32_t core_freq )
{
  uint32_t pclks = (((int)(nsecs*16*1024/1000)*core_freq)>>14)+1;
  //printf("Called _bgp_Delay_ns with %i nsecs, converted to %i pclks\n",nsecs,pclks);
  _bgp_Delay(pclks);
}


extern inline int _bgp_SystemCall( uint32_t sc_num )
{
   do {
      register int sc asm("r0") = sc_num;
      register int rc asm("r3") = 5;

      asm volatile ( "sc;"
                     : "+r" (rc),
                       "+r" (sc)
                     : "0"  (rc),
                       "1"  (sc)
                     : "memory" );
      return( rc );
      }
      while( 0 );
}

// mbar'd volatile 32bit write (volatile store that avoids write-combining)
extern inline void _bgp_Out32( uint32_t *vaddr, uint32_t value )
{
   // left-side casts are deprecated by gcc, so define a new variable
   volatile uint32_t *va = (volatile uint32_t *)vaddr;

   *va = (value);
   _bgp_mbar();
}

// volatile 32bit read
extern inline uint32_t _bgp_In32( uint32_t *vaddr )
{
   volatile uint32_t *va = (volatile uint32_t *)vaddr;

   _bgp_mbar();

   return( *va );
}

// mbar'd volatile 64bit write (volatile store that avoids write-combining)
extern inline void _bgp_Out64( uint64_t *vaddr, uint64_t value )
{
   union {
         double d;
         unsigned long long ull;
         }
         tmp;
   register volatile double *pdbl = (volatile double *)vaddr;
   register double dval;

   tmp.ull = value;

   do {
      asm volatile ("lfd   %0,0(%1);"
                    "stfd  %0,0(%2);"
                    "mbar;"
                    : "=&b" (dval)
                    : "b"   (&tmp.d),
                      "b"   (pdbl)
                    : "memory" );
      }
      while( 0 );
}

// volatile 64bit read
extern inline uint64_t _bgp_In64( uint64_t *vaddr )
{
   volatile double *va = (volatile double *)vaddr;

   union {
         double d;
         unsigned long long ull;
         }
         tmp;

   tmp.d = *va;

   _bgp_mbar();

   return( tmp.ull );
}

// Double-Hummer Inlines
#define STR(s) #s

#define _bgp_QuadLoad(v,f)  asm volatile( "lfpdx " STR(f) ",0,%0" :: "r" (v) : "fr" STR(f) )

#define _bgp_QuadStore(v,f) asm volatile( "stfpdx " STR(f) ",0,%0" :: "r" (v) : "memory" )

#define _bgp_QuadMove(src,dst,f)  asm volatile( "lfpdx  " STR(f) ",0,%0;" \
                                                "stfpdx " STR(f) ",0,%1;" \
                                                :                         \
                                                : "r" (src),              \
                                                  "r" (dst)               \
                                                : "memory",               \
                                                  "fr" STR(f) )


#define _bgp_mov128b(si,sb,di,db) \
do { \
  asm volatile("lfpdx 0,%0,%1;stfpdx 0,%2,%3": :"Ob" (si), "r" (sb), "Ob" (di), "r" (db):"fr0","memory"); \
} while(0)

#define _bgp_mov128b_update(si,sb,di,db) \
do { \
  asm volatile("lfpdux 0,%0,%1;stfpdux 0,%2,%3": :"Ob" (si), "r" (sb), "Ob" (di), "r" (db):"fr0","memory"); \
} while(0)



__END_DECLS

#endif // Not __ASSEMBLY__, Not __BGP_HOST_COMPILED__, Not __MAMBO__



#endif // Add nothing below this line

