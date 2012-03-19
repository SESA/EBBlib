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
 * \file bpcore/bgp_atomic_ops.h
 */

#ifndef	_BGP_ATOMIC_OPS_H_ // Prevent multiple inclusion
#define	_BGP_ATOMIC_OPS_H_



#include <common/namespace.h>
#include <common/alignment.h>
#include <bpcore/ppc450_inlines.h>
#include <bpcore/bgp_types.h>

#if !defined(__ASSEMBLY__)

__BEGIN_DECLS

#ifndef __INLINE__
#define __INLINE__ extern inline
#endif

//@MG: Note alignment need not be this coarse (32B), but must be >= 8B.
typedef struct T_BGP_Atomic
                {
                volatile uint32_t atom;
                }
                ALIGN_L1D_CACHE _BGP_Atomic;

//
// eg: _BGP_Atomic my_atom = _BGP_ATOMIC_INIT( 0 );
//
#define _BGP_ATOMIC_INIT(val) { (val) }

// Read the current value of an atom.
//   Note: volatile attribute ensures value is not cached in a register.
#define _BGP_ATOMIC_READ(pvar) (pvar->atom)

// returns the orginal value of the atom when the atomic addition has succeeded
__INLINE__ uint32_t _bgp_fetch_and_add( _BGP_Atomic *var, uint32_t value )
{
    register uint32_t old_val, tmp_val;

    _bgp_msync();

    do {
       old_val = _bgp_LoadReserved( &(var->atom) );

       tmp_val = old_val + value;
       }
       while( !_bgp_StoreConditional( &(var->atom), tmp_val ) );

   return( old_val );
}

// Set bit(s): returns the orginal value of the atom when the atomic OR has succeeded
__INLINE__ uint32_t _bgp_fetch_and_or( _BGP_Atomic *var, uint32_t mask )
{
    register uint32_t old_val, tmp_val;

    _bgp_msync();

    do {
       old_val = _bgp_LoadReserved( &(var->atom) );

       tmp_val = old_val | mask;
       }
       while( !_bgp_StoreConditional( &(var->atom), tmp_val ) );

   return( old_val );
}


// Clear bit(s): returns the orginal value of the atom when the atomic AND has succeeded
__INLINE__ uint32_t _bgp_fetch_and_and( _BGP_Atomic *var, uint32_t mask )
{
    register uint32_t old_val, tmp_val;

    _bgp_msync();

    do {
       old_val = _bgp_LoadReserved( &(var->atom) );

       tmp_val = old_val & mask;
       }
       while( !_bgp_StoreConditional( &(var->atom), tmp_val ) );

   return( old_val );
}


// Test and Set: Atomically OR mask with atom.
//    If any bits of mask are already set in atom, return 0 (FALSE) with atom unchanged.
//    Otherwise return 1 (TRUE) with atom atomically updated.
__INLINE__ int _bgp_test_and_set( _BGP_Atomic *var, uint32_t mask )
{
    register uint32_t old_val, tmp_val;

    _bgp_msync();

    do {
       old_val = _bgp_LoadReserved( &(var->atom) );

       if ( old_val & mask )
          return(0);

       tmp_val = old_val | mask;
       }
       while( !_bgp_StoreConditional( &(var->atom), tmp_val ) );

   return( 1 );
}


// Compare and Swap:
//   returns:
//      1 = var equals old value, new value atomically stored in var
//      0 = var does not equal old value, and *pOldValue is set to current value.
__INLINE__ int _bgp_compare_and_swap( _BGP_Atomic *var,
                                      uint32_t  *pOldValue,
                                      uint32_t  NewValue )
{
    uint32_t tmp_val;

    do {
       tmp_val = _bgp_LoadReserved( &(var->atom) );
       if ( *pOldValue != tmp_val  )
          {
          *pOldValue = tmp_val;
          return( 0 );
          }
        }
        while( !_bgp_StoreConditional( &(var->atom), NewValue ) );

   return( 1 );
}


__END_DECLS

#endif




#endif // Add nothing below this line
