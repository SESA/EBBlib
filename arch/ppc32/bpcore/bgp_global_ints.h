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
 * \file bpcore/bgp_global_ints.h
 */

#ifndef	_BGP_GLOBAL_INTS_H_ // Prevent multiple inclusion
#define	_BGP_GLOBAL_INTS_H_



#include <common/namespace.h>

__BEGIN_DECLS

#include <bpcore/bgp_types.h>
#include <bpcore/bgp_dcrmap.h>
#include <common/bgp_personality.h>


#define _BGP_DCR_GLOBINT_STATUS             (_BGP_DCR_GLOBINT + 0x000)   // Read-Only


#define _BGP_DCR_GLOBINT_ASSERT_CH(ch)      (_BGP_DCR_GLOBINT + (ch))    // channels 0:3

#define _BGP_DCR_GLOBINT_ASSERT_CH0         (_BGP_DCR_GLOBINT + 0x000)   // Write-Only

#define _BGP_DCR_GLOBINT_ASSERT_CH1         (_BGP_DCR_GLOBINT + 0x001)   // Write-Only

#define _BGP_DCR_GLOBINT_ASSERT_CH2         (_BGP_DCR_GLOBINT + 0x002)   // Write-Only

#define _BGP_DCR_GLOBINT_ASSERT_CH3         (_BGP_DCR_GLOBINT + 0x003)   // Write-Only

#define _BGP_DCR_GLOBINT_RESETS             (_BGP_DCR_GLOBINT + 0x005)   // Write-Only
#define  _BGP_DCR_GLOBINT_RESETS_CH0          _BN(28)
#define  _BGP_DCR_GLOBINT_RESETS_CH1          _BN(29)
#define  _BGP_DCR_GLOBINT_RESETS_CH2          _BN(30)
#define  _BGP_DCR_GLOBINT_RESETS_CH3          _BN(31)

#define _BGP_DCR_GLOBINT_USER_EN            (_BGP_DCR_GLOBINT + 0x006)   // Write-Only
#define  _BGP_DCR_GLOBINT_USER_EN_CH0         _BN(28)
#define  _BGP_DCR_GLOBINT_USER_EN_CH1         _BN(29)
#define  _BGP_DCR_GLOBINT_USER_EN_CH2         _BN(30)
#define  _BGP_DCR_GLOBINT_USER_EN_CH3         _BN(31)

#define _BGP_DCR_GLOBINT_DRIVE_CH(ch)       (_BGP_DCR_GLOBINT_DRIVE_CH0 + (ch))  // channels 0:3

#define _BGP_DCR_GLOBINT_DRIVE_CH0          (_BGP_DCR_GLOBINT + 0x008)   // Write-Only
#define _BGP_DCR_GLOBINT_DRIVE_CH1          (_BGP_DCR_GLOBINT + 0x009)   // Write-Only
#define _BGP_DCR_GLOBINT_DRIVE_CH2          (_BGP_DCR_GLOBINT + 0x00A)   // Write-Only
#define _BGP_DCR_GLOBINT_DRIVE_CH3          (_BGP_DCR_GLOBINT + 0x00B)   // Write-Only
#define  _BGP_DCR_GLOBINT_DRIVE_SEND          (1) // Send interrupt using ARM type
#define  _BGP_DCR_GLOBINT_DRIVE_CLEAR         (0) // Clear Stickey Interrupt and clear Armed state

#define _BGP_DCR_GLOBINT_SET_CH(ch)         (_BGP_DCR_GLOBINT_SET_CH0 + (ch))   // channels 0:3

#define _BGP_DCR_GLOBINT_SET_CH0            (_BGP_DCR_GLOBINT + 0x00C)    // Write-Only
#define _BGP_DCR_GLOBINT_SET_CH1            (_BGP_DCR_GLOBINT + 0x00D)    // Write-Only
#define _BGP_DCR_GLOBINT_SET_CH2            (_BGP_DCR_GLOBINT + 0x00E)    // Write-Only
#define _BGP_DCR_GLOBINT_SET_CH3            (_BGP_DCR_GLOBINT + 0x00F)    // Write-Only
#define   _BGP_DCR_GLOBINT_SET_ARM_OR         (0) // Set mode to Stickey and set Arm to OR
#define   _BGP_DCR_GLOBINT_SET_ARM_AND        (1) // Set mode to Stickey and set Arm to AND

// Bit definitions for Read-side from any GLOBINT DCR
#define   _BGP_DCR_GLOBINT_READ_MODE(x)              _B4( 3,x)  // 0=Direct, 1=Stickey
#define   _BGP_DCR_GLOBINT_READ_STICKEY_RECVD(x)     _B4( 7,x)  // 0=False,  1=True
#define   _BGP_DCR_GLOBINT_READ_ARM_ACTIVE(x)        _B4(11,x)  // 0=False,  1=True
#define   _BGP_DCR_GLOBINT_READ_ARM_TYPE(x)          _B4(15,x)  // 0=OR,     1=AND
#define   _BGP_DCR_GLOBINT_READ_USER_ACCESS          _BN(17)    // Non-Priv access attempted
#define   _BGP_DCR_GLOBINT_READ_PARITY_ERROR         _BN(19)    // Parity Error detected
#define   _BGP_DCR_GLOBINT_READ_DOWNTREE_STATE(x)    _B4(23,x)  // Down-Tree (input) state
#define   _BGP_DCR_GLOBINT_READ_UPTREE_STATE(x)      _B4(31,x)  // Up-Tree (output) state


extern int _bgp_GlobalInts_Init( _BGP_Personality_t *pPers );

// "global barrier" includes Compute Nodes and I/O Nodes. If Single Node, returns immediately.
extern int _bgp_global_barrier( void );
extern int _bgp_global_barrier_timeout( unsigned long long max_time );

/*!\brief Global Interupt DCR. Note that userEnables returns the uptree state in diagnostic read mode.
 *
 * _BGP_DCR_GLOBINT: Global Interrupt DCR 
 * <pre>
 *  Address: 0x660
 *  Offset  R/W   Bits  Function
 *  0x0     W     31    Set interrupt mode to direct and assert value on global interrupt 0 uptree   
 *  0x1     W     31    Same as 0x0, but for channel 1
 *  0x2     W     31    Same as 0x0, but for channel 2
 *  0x3     W     31    Same as 0x0, but for channel 3
 *  0x4     W     31    Reserved
 *  0x5     W     28:31 Individual channel resets
 *  0x6     W     28:31 Set to enable user interrupts. bit 28:chan 0, bit 29: chan 1, etc.
 *  0x8     W     31    If data[31]=1, send intrpt channel 0 using ARM type. 
 *                      If data[31]=0, clear channel 0 sticky intrpt, clear channel 0 armed
 *  0x9     W     31    Same as 0x8, but for channel 1
 *  0xA     W     31    Same as 0x8, but for channel 2
 *  0xB     W     31    Same as 0x8, but for channel 3
 *  0xC     W     31    Set channel 0 interrupt mode to sticky and set ARM type (0=OR, 1=AND)
 *  0xD     W     31    Same as 0xC, but for channel 1
 *  0xE     W     31    Same as 0xC, but for channel 2
 *  0xF     W     31    Same as 0xC, but for channel 3
 *  any     R     0:3   Mode: 0 = direct, 1 = sticky
 *  any     R     4:7   Sticky interrupt received (1 = true)
 *  any     R     8:11  Sticky interrupt ARM active (1 = armed)
 *  any     R     12:15 Sticky interrupt ARM type (0=OR, 1=AND)
 *  any     R     16    NA (always 0)
 *  any     R     17    Unprivileged access to protected register attempted
 *  any     R     18    NA (always 0)
 *  any     R     19    Parity error
 *  any     R     20:23 Downtree (input) state
 *  any     R     24:27 User enables
 *  any     R     28:31 Uptree (output) state
 * </pre>
 *---------------------------------------------------------------------------------------------
*/
typedef struct {
   uint32_t dcr_GlobalInt;          
} DCR_GlobalInt_t;


__END_DECLS



#endif // Add nothing below this line

