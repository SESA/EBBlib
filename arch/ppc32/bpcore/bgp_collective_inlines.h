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
 * \file bpcore/bgp_collective_inlines.h
 */

#ifndef _BGP_COLLECTIVE_INLINES_H_ // Prevent multiple inclusion
#define _BGP_COLLECTIVE_INLINES_H_



#include <common/namespace.h>

__BEGIN_DECLS

#include <common/linkage.h>
#include <bpcore/bgp_types.h>
#include <bpcore/ppc450_inlines.h>
#include <bpcore/collective.h>
#include <common/bgp_personality.h>


#ifndef __INLINE__
#define __INLINE__ extern inline
#endif

/*---------------------------------------------------------------------------*
 *  Tree Fifo Status Access Functions                                        *
 *---------------------------------------------------------------------------*/

__INLINE__ void _bgp_vcX_get_status( _BGP_TreeFifoStatus *stat, uint32_t vc_base )
{
    stat->status_word = _bgp_In32( (uint32_t *)(vc_base + _BGP_TRx_Sx ) );
}


// Obtain status for a Tree Virtual Channel
__INLINE__ void _bgp_TreeGetStatus( int vc, _BGP_TreeFifoStatus *stat )
{
   if ( vc )
      stat->status_word = _bgp_In32( (uint32_t *)_BGP_TR1_S1 );
   else
      stat->status_word = _bgp_In32( (uint32_t *)_BGP_TR0_S0 );
}

// Obtain status for Tree Virtual Channel 0
__INLINE__ void _bgp_TreeGetStatusVC0( _BGP_TreeFifoStatus *stat )
{
   stat->status_word = _bgp_In32( (uint32_t *)_BGP_TR0_S0 );
}

// Obtain status for Tree Virtual Channel 1
__INLINE__ void _bgp_TreeGetStatusVC1( _BGP_TreeFifoStatus *stat )
{
   stat->status_word = _bgp_In32( (uint32_t *)_BGP_TR1_S1 );
}

// Can VC0 accept a packet?
//   Return: True  = There is room to inject header and payload.
//           False = No room to inject header and/or payload.
__INLINE__ int _bgp_TreeOkToSendVC0()
{
   _BGP_TreeFifoStatus stat;

   _bgp_TreeGetStatusVC0( &stat );

   return( (stat.InjHdrCount  < _BGP_TREE_STATUS_MAX_PKTS)  &&
           (stat.InjPyldCount < ((_BGP_TREE_STATUS_MAX_PKTS - 1) * 16)) );
}

// Can VC1 accept a packet?
//   Return: True  = There is room to inject header and payload.
//           False = No room to inject header and/or payload.
__INLINE__ int _bgp_TreeOkToSendVC1()
{
   _BGP_TreeFifoStatus stat;

   _bgp_TreeGetStatusVC1( &stat );

   return( (stat.InjHdrCount  < _BGP_TREE_STATUS_MAX_PKTS)  &&
           (stat.InjPyldCount < ((_BGP_TREE_STATUS_MAX_PKTS - 1) * 16)) );
}


// Can requested VC accept a packet?
//   Return: True  = There is room to inject header and payload.
//           False = No room to inject header and/or payload.
__INLINE__ int _bgp_TreeOkToSend( int vc )  // Virtual Channel (0 or 1)
{
   _BGP_TreeFifoStatus stat;

   if ( vc )
      _bgp_TreeGetStatusVC1( &stat );
   else
      _bgp_TreeGetStatusVC0( &stat );

   return( (stat.InjHdrCount  < _BGP_TREE_STATUS_MAX_PKTS)  &&
           (stat.InjPyldCount < ((_BGP_TREE_STATUS_MAX_PKTS - 1) * 16)) );
}


// Does VC0 contain a header and payload?
//   Return: True  = There is a packet to be received.
//           False = The reception FIFO is empty.
__INLINE__ int _bgp_TreeReadyToReceiveVC0()
{
   _BGP_TreeFifoStatus stat;

   _bgp_TreeGetStatusVC0( &stat );

   return( (stat.RecHdrCount  >   0) &&
           (stat.RecPyldCount >= 16)    );
}

// Does VC1 contain a header and payload?
//   Return: True  = There is a packet to be received.
//           False = The reception FIFO is empty.
__INLINE__ int _bgp_TreeReadyToReceiveVC1()
{
   _BGP_TreeFifoStatus stat;

   _bgp_TreeGetStatusVC1( &stat );

   return( (stat.RecHdrCount  >   0) &&
           (stat.RecPyldCount >= 16)    );
}

// Does requested VC contain a header and payload?
//   Return: True  = There is a packet to be received.
//           False = The reception FIFO is empty.
__INLINE__ int _bgp_TreeReadyToReceive( int vc ) // Virtual Channel (0 or 1)
{
   _BGP_TreeFifoStatus stat;

   if ( vc )
      _bgp_TreeGetStatusVC1( &stat );
   else
      _bgp_TreeGetStatusVC0( &stat );

   return( (stat.RecHdrCount  >   0) &&
           (stat.RecPyldCount >= 16)    );
}


/*---------------------------------------------------------------------------*
 *  Collective Network Header and Payload Functions                          *
 *---------------------------------------------------------------------------*/

// Because Tree Headers are expensive to create, the interface provides a choice
// of creating the headers "on the fly" or pre-creating the headers.  When you
// use regular and repeated communication patterns (a.k.a persistent messages),
// pre-creating headers during the setup phase can provide significant performance
// benefits.

// Create a COLLECTIVE H/W tree header.  This function allows full control of header creation.
// QUESTION: SHOULD THIS RETURN AN ERROR CODE, RATHER THAN THE POINTER?
__INLINE__ _BGP_TreeHwHdr *_bgp_TreeMakeCollectiveHdr(
                                _BGP_TreeHwHdr *hdr,     // Filled in on return
                                int vt,                  // class (virtual tree) user space calls should use vt=1
                                int irq,                 // irq.  If combine, must be the same for all callers.
                                int opcode,              // ALU opcode (000 for ordinary packets)
                                int opsize,              // ALU operand size
                                int tag,                 // software tag. If combine, must be the same for all callers.
                                int csumMode )           // injection checksum mode
{
   hdr->CtvHdr.Class    = vt;        // Class number (a.k.a virtual tree number)
   hdr->CtvHdr.Ptp      = 0;         // Must be 0 for collective header format.
   hdr->CtvHdr.Irq      = irq;       // 1=request interrupt when received, 0=not
   hdr->CtvHdr.OpCode   = opcode;    // 000=ordinary routed packet, else ALU opcode
   hdr->CtvHdr.OpSize   = opsize;    // Operand size
   hdr->CtvHdr.Tag      = tag;       // Software tag
   hdr->CtvHdr.CsumMode = csumMode;  // Injection checksum mode

   return( hdr );
}

// Create a POINT-TO-POINT H/W tree header.  This function allows full control of header construction.
__INLINE__ _BGP_TreeHwHdr *_bgp_TreeMakePtpHdr(
                                _BGP_TreeHwHdr *hdr,     // Filled in on return
                                int vt,                  // class (virtual tree) user space calls should use vt=1 
                                int irq,                 // interrupt request
                                int PtpTarget,           // point-to-point target (node address to accept)
                                int csumMode )           // injection checksum mode
{
   hdr->PtpHdr.Class     = vt;          // Class number (a.k.a virtual tree number)
   hdr->PtpHdr.Ptp       = 1;           // Must be 1 for PTP header format.
   hdr->PtpHdr.Irq       = irq;         // 1=request interrupt when received, 0=not
   hdr->PtpHdr.PtpTarget = PtpTarget;   // Ptp packet target (matched to node address)
   hdr->PtpHdr.CsumMode  = csumMode;    // Injection checksum mode

   return( hdr );
}

// Create a point-to-point header for the common case.  In this header:
// (1) The irq bit is always set
// (2) The injection checksum mode is set to 1 (include H/W header, exclude first qword of payload)
__INLINE__ _BGP_TreeHwHdr *_bgp_TreeMakeSendHdr(
                                _BGP_TreeHwHdr *hdr,     // Filled in on return
                                int vt,                  // class (virtual tree)  user space calls should use vt=1
                                int PtpTarget )          // point-to-point target (node address to accept)
{
   hdr->PtpHdr.Class     = vt;                  // Class number (a.k.a virtual tree number)
   hdr->PtpHdr.Ptp       = 1;                   // Must be 1 for PTP header format.
   hdr->PtpHdr.Irq       = 1;                   // 1=request interrupt when received, 0=not (usually Masked in BIC)
   hdr->PtpHdr.PtpTarget = PtpTarget;           // Ptp packet target (matched to node address)
   hdr->PtpHdr.CsumMode  = _BGP_TREE_CSUM_SOME; // Injection checksum mode (All but 1st Quad)

   return( hdr );
}

// Create a broadcast header for the common case.  In this header:
// (1) the irq bit is always cleared
// (2) the opcode is set to NONE (so operand size is irrelevant).
// (3) the injection checksum mode is set to 3 (include everything)
__INLINE__ _BGP_TreeHwHdr *_bgp_TreeMakeBcastHdr(
                                _BGP_TreeHwHdr *hdr,     // Filled in on return
                                int vt,                  // class (virtual tree)  user space calls should use vt=1
                                int tag )                // software tag.  If combine, must be the same for all callers.
{
   hdr->CtvHdr.Class    = vt;                    // Class number (a.k.a virtual tree number)  user space calls should use vt=1
   hdr->CtvHdr.Ptp      = 0;                     // Must be 0 for collective header format.
   hdr->CtvHdr.Irq      = 0;                     // 1=request interrupt when received, 0=not
   hdr->CtvHdr.OpCode   = _BGP_TREE_OPCODE_NONE; // 000=ordinary routed packet, else ALU opcode
   hdr->CtvHdr.OpSize   = 0;                     // Operand size
   hdr->CtvHdr.Tag      = tag;                   // Software tag
   hdr->CtvHdr.CsumMode = _BGP_TREE_CSUM_ALL;    // Injection checksum mode

   return( hdr );
}

// Create a collective header for the common ALU case.  In this header:
// (1) the irq bit is always cleared
// (2) the tag is undefined
// (3) the injection checksum mode is set to 3 (include everything)
__INLINE__ _BGP_TreeHwHdr *_bgp_TreeMakeAluHdr(
                                _BGP_TreeHwHdr *hdr,     // Filled in on return
                                int vt,                  // class (virtual tree)  user space calls should use vt=1
                                int opcode,              // ALU opcode
                                int opsize )             // ALU operand size
{
   hdr->CtvHdr.Class    = vt;                    // Class number (a.k.a virtual tree number)  user space calls should use vt=1
   hdr->CtvHdr.Ptp      = 0;                     // Must be 0 for collective header format.
   hdr->CtvHdr.Irq      = 0;                     // 1=request interrupt when received, 0=not
   hdr->CtvHdr.OpCode   = opcode;                // 000=ordinary routed packet, else ALU opcode
   hdr->CtvHdr.OpSize   = opsize;                // Operand size
   hdr->CtvHdr.Tag      = 0;                     // Software tag
   hdr->CtvHdr.CsumMode = _BGP_TREE_CSUM_ALL;    // Injection checksum mode

   return( hdr );
}


// Enable local loopback on a channel (0, 1, or 2).
__INLINE__ void _bgp_TreeWrapChannel( int channel )
{
  unsigned long dcr = _bgp_mfdcrx( _BGP_DCR_TR_ARB_RCFG );
  _bgp_mtdcrx( _BGP_DCR_TR_ARB_RCFG, dcr | (_TR_ARB_RCFG_LB0 << channel) );
}

// Disable local loopback on a channel (0, 1, or 2).
__INLINE__ void _bgp_TreeUnwrapChannel( int channel )
{
  unsigned long dcr = _bgp_mfdcrx( _BGP_DCR_TR_ARB_RCFG );
  _bgp_mtdcrx( _BGP_DCR_TR_ARB_RCFG, dcr & ~(_TR_ARB_RCFG_LB0 << channel) );
}

// Enable header class MSB inversion on a send channel (0, 1, or 2).  Note that inversion occurs
// before the header enters the send fifo!
__INLINE__ void _bgp_TreeInvertClassMsb( int channel )
{
  unsigned long dcr = _bgp_mfdcrx( _BGP_DCR_TR_CH0_SCTRL + 0x8*channel );
  _bgp_mtdcrx( _BGP_DCR_TR_CH0_SCTRL + 0x8*channel, dcr | _TR_SCTRL_INVMSB );
}

// Disable header class MSB inversion on a send channel (0, 1, or 2).
__INLINE__ void _bgp_TreeUninvertClassMsb( int channel )
{
  unsigned long dcr = _bgp_mfdcrx( _BGP_DCR_TR_CH0_SCTRL + 0x8*channel );
  _bgp_mtdcrx( _BGP_DCR_TR_CH0_SCTRL + 0x8*channel, dcr & ~_TR_SCTRL_INVMSB );
}

// Set point-to-point receive address.
__INLINE__ void _bgp_TreeSetPtpAddress( int addr )
{
  _bgp_mtdcrx( _BGP_DCR_TR_GLOB_NADDR, addr );
}

// Get point-to-point receive address.
__INLINE__ int _bgp_TreeGetPtpAddress( void )
{
  return( _bgp_mfdcrx( _BGP_DCR_TR_GLOB_NADDR ) );
}


/*---------------------------------------------------------------------------*
 *  Low-Level Collective Packet Injection and Reception                      *
 *---------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
// _bgp_vcX_pkt_inject: Inject a tree packet (header and full payload) to a VC.
//
//  Do not use this macro directly, there are wrapper functions below.
//
//  Assumes payload source address is quad aligned.
//
//  Destroyed: q0-q9
////////////////////////////////////////////////////////////////////////////////
__INLINE__ void _bgp_vcX_pkt_inject( uint32_t *header, void *payload, uint32_t vc_base )
{
   uint32_t hdr_reg,         // temps
            bytes_per_quad,
            payload_fifo;

   asm volatile( "lwz     %0,0(%5);"       // hdr_reg(%0) = *header(%5)
                 "li      %2,16;"          // bytes_per_quad(%2) = 16
                 "lfpdx   0,0,%3;"         // F0=Q0 load *payload(%3)
                 "stw     %0,%c6(%4);"     // store header to HI offset from vc_base
                 "la      %1,%c7(%4);"     // payload_fifo(%1) = data injection address
                 "lfpdux  1,%3,%2;"        // F1=Q1 load from (%3 += 16)
                 "lfpdux  2,%3,%2;"        // F2=Q2 load
                 "lfpdux  3,%3,%2;"        // F3=Q3 load
                 "stfpdx  0,0,%1;"         // Q0 store to *(vc_base + TRx_DI)
                 "lfpdux  4,%3,%2;"        // F4=Q4 load
                 "lfpdux  5,%3,%2;"        // F5=Q5 load
                 "lfpdux  6,%3,%2;"        // F6=Q6 load
                 "stfpdx  1,0,%1;"         // Q1 store
                 "stfpdx  2,0,%1;"         // Q2 store
                 "stfpdx  3,0,%1;"         // Q3 store
                 "lfpdux  7,%3,%2;"        // F7=Q7 load
                 "lfpdux  8,%3,%2;"        // F8=Q8 load
                 "lfpdux  9,%3,%2;"        // F9=Q9 load
                 "stfpdx  4,0,%1;"         // Q4 store
                 "stfpdx  5,0,%1;"         // Q5 store
                 "stfpdx  6,0,%1;"         // Q6 store
                 "lfpdux  0,%3,%2;"        // F0=Q10 load
                 "lfpdux  1,%3,%2;"        // F1=Q11 load
                 "lfpdux  2,%3,%2;"        // F2=Q12 load
                 "stfpdx  7,0,%1;"         // Q7 store
                 "stfpdx  8,0,%1;"         // Q8 store
                 "stfpdx  9,0,%1;"         // Q9 store
                 "lfpdux  3,%3,%2;"        // F3=Q13 load
                 "lfpdux  4,%3,%2;"        // F4=Q14 load
                 "lfpdux  5,%3,%2;"        // F5=Q15 load
                 "stfpdx  0,0,%1;"         // Q10 store
                 "stfpdx  1,0,%1;"         // Q11 store
                 "stfpdx  2,0,%1;"         // Q12 store
                 "stfpdx  3,0,%1;"         // Q13 store
                 "stfpdx  4,0,%1;"         // Q14 store
                 "stfpdx  5,0,%1;"         // Q15 store
                 : "=&b" (hdr_reg),        // 0 Outputs
                   "=&b" (payload_fifo),   // 1
                   "=&b" (bytes_per_quad), // 2
                   "=b"  (payload)         // 3
                 : "b"   (vc_base),        // 4 Inputs
                   "b"   (header),         // 5
                   "i"   (_BGP_TRx_HI),    // 6
                   "i"   (_BGP_TRx_DI),    // 7
                   "3"   (payload)         // "payload" is input and output
                 : "fr0", "fr1", "fr2",    // Clobbers
                   "fr3", "fr4", "fr5",
                   "fr6", "fr7", "fr8",
                   "fr9", "memory" );
}

////////////////////////////////////////////////////////////////////////////////
// _bgp_vcX_pkt_inject: Inject a tree packet (header and full payload) to a VC.
//
//  Do not use this macro directly, there are wrapper functions below.
//  
//  Assumes payload source address is quad aligned.
//  This injects only the first quad multiple times.
//  Destroyed: q0-q9
////////////////////////////////////////////////////////////////////////////////
__INLINE__ void _bgp_vcX_pkt_inject0( uint32_t *header,uint32_t vc_base )
{
   uint32_t hdr_reg,         // temps
            bytes_per_quad,
            payload_fifo;
   _QuadWord_t zeroes[4];
   zeroes[0].ull[0]=0;
   zeroes[0].ull[1]=0;
   zeroes[1].ull[0]=0;
   zeroes[1].ull[1]=0;
   zeroes[2].ull[0]=0;
   zeroes[2].ull[1]=0;
   zeroes[3].ull[0]=0;
   zeroes[3].ull[1]=0;
   void *payload = zeroes;
   asm volatile( "lwz     %0,0(%5);"       // hdr_reg(%0) = *header(%5)
                 "li      %2,16;"          // bytes_per_quad(%2) = 16
                 "lfpdx   0,0,%3;"         // F0=Q0 load *payload(%3)
                 "stw     %0,%c6(%4);"     // store header to HI offset from vc_base
                 "la      %1,%c7(%4);"     // payload_fifo(%1) = data injection address
                 "lfpdux  1,%3,%2;"        // F1=Q1 load from (%3 += 16)
                 "lfpdux  2,%3,%2;"        // F2=Q2 load
                 "lfpdux  3,%3,%2;"        // F3=Q3 load
                 "lfpdux  4,%3,%2;"        // F4=Q4 load
                 "stfpdx  0,0,%1;"         // Q0 store to *(vc_base + TRx_DI)
                 "stfpdx  1,0,%1;"         // Q1 store
                 "stfpdx  2,0,%1;"         // Q2 store
                 "stfpdx  3,0,%1;"         // Q3 store
                 "stfpdx  0,0,%1;"         // Q4 store to *(vc_base + TRx_DI)
                 "stfpdx  1,0,%1;"         // Q5 store
                 "stfpdx  2,0,%1;"         // Q6 store
                 "stfpdx  3,0,%1;"         // Q7 store
                 "stfpdx  0,0,%1;"         // Q8 store to *(vc_base + TRx_DI)
                 "stfpdx  1,0,%1;"         // Q9 store
                 "stfpdx  2,0,%1;"         // Q10 store
                 "stfpdx  3,0,%1;"         // Q11 store
                 "stfpdx  0,0,%1;"         // Q12 store to *(vc_base + TRx_DI)
                 "stfpdx  1,0,%1;"         // Q13 store
                 "stfpdx  2,0,%1;"         // Q14 store
                 "stfpdx  3,0,%1;"         // Q15 store
                 : "=&b" (hdr_reg),        // 0 Outputs
                   "=&b" (payload_fifo),   // 1
                   "=&b" (bytes_per_quad), // 2
                   "=b"  (payload)         // 3
                 : "b"   (vc_base),        // 4 Inputs
                   "b"   (header),         // 5
                   "i"   (_BGP_TRx_HI),    // 6
                   "i"   (_BGP_TRx_DI),    // 7
                   "3"   (payload)         // "payload" is input and output
                 : "fr0", "fr1", "fr2",    // Clobbers
                   "fr3", "fr4", "fr5",
                   "fr6", "fr7", "fr8",
                   "fr9", "memory" );
}

__INLINE__ void _bgp_vcX_pkt_inject_sh( uint32_t *header, struct _BGPTreePacketSoftHeader_tag* softheader, void *payload, uint32_t vc_base )
{
  uint32_t hdr_reg,         // temps
    bytes_per_quad,
    payload_fifo;

  asm volatile( "lwz     %0,0(%6);"       // hdr_reg(%0) = *header(%5)
		"li      %2,16;"          // bytes_per_quad(%2) = 16
		"lfpdx   0,0,%4;"         // F0=Q0 load from softheader
		"stw     %0,%c7(%5);"     // store header to HI offset from vc_base
		"la      %1,%c8(%5);"     // payload_fifo(%1) = data injection address
		"lfpdx   1,0,%3;"         // F1=Q1 load from payload(%3)
		"lfpdux  2,%3,%2;"        // F2=Q2 load from (%3 += 16)
		"lfpdux  3,%3,%2;"        // F3=Q3 load
		"stfpdx  0,0,%1;"         // Q0 store to *(vc_base + TRx_DI)
		"lfpdux  4,%3,%2;"        // F4=Q4 load
		"lfpdux  5,%3,%2;"        // F5=Q5 load
		"lfpdux  6,%3,%2;"        // F6=Q6 load
		"stfpdx  1,0,%1;"         // Q1 store
		"stfpdx  2,0,%1;"         // Q2 store
		"stfpdx  3,0,%1;"         // Q3 store
		"lfpdux  7,%3,%2;"        // F7=Q7 load
		"lfpdux  8,%3,%2;"        // F8=Q8 load
		"lfpdux  9,%3,%2;"        // F9=Q9 load
		"stfpdx  4,0,%1;"         // Q4 store
		"stfpdx  5,0,%1;"         // Q5 store
		"stfpdx  6,0,%1;"         // Q6 store
		"lfpdux  0,%3,%2;"        // F0=Q10 load
		"lfpdux  1,%3,%2;"        // F1=Q11 load
		"lfpdux  2,%3,%2;"        // F2=Q12 load
		"stfpdx  7,0,%1;"         // Q7 store
		"stfpdx  8,0,%1;"         // Q8 store
		"stfpdx  9,0,%1;"         // Q9 store
		"lfpdux  3,%3,%2;"        // F3=Q13 load
		"lfpdux  4,%3,%2;"        // F4=Q14 load
		"lfpdux  5,%3,%2;"        // F5=Q15 load
		"stfpdx  0,0,%1;"         // Q10 store
		"stfpdx  1,0,%1;"         // Q11 store
		"stfpdx  2,0,%1;"         // Q12 store
		"stfpdx  3,0,%1;"         // Q13 store
		"stfpdx  4,0,%1;"         // Q14 store
		"stfpdx  5,0,%1;"         // Q15 store
		: "=&b" (hdr_reg),        // 0 Outputs
		"=&b" (payload_fifo),   // 1
		"=&b" (bytes_per_quad), // 2
		"=b"  (payload),        // 3
		"=b"  (softheader)      // 4
		: "b"   (vc_base),        // 5 Inputs
		"b"   (header),         // 6
		"i"   (_BGP_TRx_HI),    // 7
		"i"   (_BGP_TRx_DI),    // 8
		"3"   (payload),        // 9 "payload" is input and output
		"4"   (softheader)      // 10 softheader
		: "fr0", "fr1", "fr2",    // Clobbers
		"fr3", "fr4", "fr5",
		"fr6", "fr7", "fr8",
		"fr9", "memory" );
}

////////////////////////////////////////////////////////////////////////////////
// _bgp_vcX_pkt_receive: Receive a tree packet (header and full payload) from a VC.
//
//  Do not use this macro directly, there are wrapper functions below.
//
//  Assumes payload destination address is quad aligned.
//
//  Destroyed: q0-q9
////////////////////////////////////////////////////////////////////////////////
__INLINE__ void _bgp_vcX_pkt_receive( uint32_t *header, void *payload, uint32_t vc_base )
{
   uint32_t hdr_reg,        // temps
            payload_fifo,
            bytes_per_quad;

   asm volatile( "lwz     %0,%c6(%4);"      // load Hdr from *(vc_base + hdr) into hdr_reg
                 "la      %1,%c7(%4);"      // %1 = data reception fifo address
                 "li      %2,16;"           // %2 = 16 bytes per quad
                 "lfpdx   0,0,%1;"          // F0=Q0  load
                 "lfpdx   1,0,%1;"          // F1=Q1  load
                 "lfpdx   2,0,%1;"          // F2=Q2  load
                 "stw     %0,0(%5);"        // Store header to caller's header buffer
                 "lfpdx   3,0,%1;"          // F3=Q3  load
                 "lfpdx   4,0,%1;"          // F4=Q4  load
                 "lfpdx   5,0,%1;"          // F5=Q5  load
                 "stfpdx  0,0,%3;"          // Q0  store to caller's payload buffer
                 "stfpdux 1,%3,%2;"         // Q1  store to (%5 += 16)
                 "stfpdux 2,%3,%2;"         // Q2  store
                 "lfpdx   6,0,%1;"          // F6=Q6  load
                 "lfpdx   7,0,%1;"          // F7=Q7  load
                 "lfpdx   8,0,%1;"          // F8=Q8  load
                 "stfpdux 3,%3,%2;"         // Q3  store
                 "stfpdux 4,%3,%2;"         // Q4  store
                 "stfpdux 5,%3,%2;"         // Q5  store
                 "lfpdx   9,0,%1;"          // F9=Q9  load
                 "lfpdx   0,0,%1;"          // F0=Q10 load
                 "lfpdx   1,0,%1;"          // F1=Q11 load
                 "stfpdux 6,%3,%2;"         // Q6  store
                 "stfpdux 7,%3,%2;"         // Q7  store
                 "stfpdux 8,%3,%2;"         // Q8  store
                 "lfpdx   2,0,%1;"          // F2=Q12 load
                 "lfpdx   3,0,%1;"          // F3=Q13 load
                 "lfpdx   4,0,%1;"          // F4=Q14 load
                 "stfpdux 9,%3,%2;"         // Q9  store
                 "stfpdux 0,%3,%2;"         // Q10 store
                 "stfpdux 1,%3,%2;"         // Q11 store
                 "lfpdx   5,0,%1;"          // F5=Q15 load
                 "stfpdux 2,%3,%2;"         // Q12 store
                 "stfpdux 3,%3,%2;"         // Q13 store
                 "stfpdux 4,%3,%2;"         // Q14 store
                 "stfpdux 5,%3,%2;"         // Q15 store
                 : "=&b"  (hdr_reg),        // 0 Outputs
                   "=&b"  (payload_fifo),   // 1
                   "=&b"  (bytes_per_quad), // 2
                   "=b"   (payload)         // 3
                 : "b"    (vc_base),        // 4 Inputs
                   "b"    (header),         // 5
                   "i"    (_BGP_TRx_HR),    // 6
                   "i"    (_BGP_TRx_DR),    // 7
                   "3"    (payload)         // "payload" is input and output
                 : "fr0", "fr1", "fr2",     // Clobbers
                   "fr3", "fr4", "fr5",
                   "fr6", "fr7", "fr8",
                   "fr9", "memory" );
}
////////////////////////////////////////////////////////////////////////////////
// _bgp_vcX_pkt_receive: Receive a tree packet (header and full payload) from a VC.
//
//  Do not use this macro directly, there are wrapper functions below.
//
//  Assumes payload destination address is quad aligned.
//
//  Destroyed: q0-q9
////////////////////////////////////////////////////////////////////////////////
__INLINE__ void _bgp_vcX_pkt_receiveNoHdr( uint32_t *header, void *payload, uint32_t vc_base )
{
   uint32_t hdr_reg,        // temps
            payload_fifo,
            bytes_per_quad;

   asm volatile( "la      %1,%c7(%4);"      // %1 = data reception fifo address
                 "li      %2,16;"           // %2 = 16 bytes per quad
                 "lfpdx   0,0,%1;"          // F0=Q0  load
                 "lfpdx   1,0,%1;"          // F1=Q1  load
                 "lfpdx   2,0,%1;"          // F2=Q2  load
                 "lfpdx   3,0,%1;"          // F3=Q3  load
                 "lfpdx   4,0,%1;"          // F4=Q4  load
                 "lfpdx   5,0,%1;"          // F5=Q5  load
                 "stfpdx  0,0,%3;"          // Q0  store to caller's payload buffer
                 "stfpdux 1,%3,%2;"         // Q1  store to (%5 += 16)
                 "stfpdux 2,%3,%2;"         // Q2  store
                 "lfpdx   6,0,%1;"          // F6=Q6  load
                 "lfpdx   7,0,%1;"          // F7=Q7  load
                 "lfpdx   8,0,%1;"          // F8=Q8  load
                 "stfpdux 3,%3,%2;"         // Q3  store
                 "stfpdux 4,%3,%2;"         // Q4  store
                 "stfpdux 5,%3,%2;"         // Q5  store
                 "lfpdx   9,0,%1;"          // F9=Q9  load
                 "lfpdx   0,0,%1;"          // F0=Q10 load
                 "lfpdx   1,0,%1;"          // F1=Q11 load
                 "stfpdux 6,%3,%2;"         // Q6  store
                 "stfpdux 7,%3,%2;"         // Q7  store
                 "stfpdux 8,%3,%2;"         // Q8  store
                 "lfpdx   2,0,%1;"          // F2=Q12 load
                 "lfpdx   3,0,%1;"          // F3=Q13 load
                 "lfpdx   4,0,%1;"          // F4=Q14 load
                 "stfpdux 9,%3,%2;"         // Q9  store
                 "stfpdux 0,%3,%2;"         // Q10 store
                 "stfpdux 1,%3,%2;"         // Q11 store
                 "lfpdx   5,0,%1;"          // F5=Q15 load
                 "stfpdux 2,%3,%2;"         // Q12 store
                 "stfpdux 3,%3,%2;"         // Q13 store
                 "stfpdux 4,%3,%2;"         // Q14 store
                 "stfpdux 5,%3,%2;"         // Q15 store
                 : "=&b"  (hdr_reg),        // 0 Outputs
                   "=&b"  (payload_fifo),   // 1
                   "=&b"  (bytes_per_quad), // 2
                   "=b"   (payload)         // 3
                 : "b"    (vc_base),        // 4 Inputs
                   "b"    (header),         // 5
                   "i"    (_BGP_TRx_HR),    // 6
                   "i"    (_BGP_TRx_DR),    // 7
                   "3"    (payload)         // "payload" is input and output
                 : "fr0", "fr1", "fr2",     // Clobbers
                   "fr3", "fr4", "fr5",
                   "fr6", "fr7", "fr8",
                   "fr9", "memory" );
}

////////////////////////////////////////////////////////////////////////////////
// _bgp_vcX_pkt_receive: Receive a tree packet (header and full payload) from a VC.
//
//  Do not use this macro directly, there are wrapper functions below.
//
//  Assumes payload destination address is quad aligned.
//
//  Destroyed: q0-q9
////////////////////////////////////////////////////////////////////////////////
__INLINE__ void _bgp_vcX_pkt_receiveNoHdrNoStore( uint32_t *header, uint32_t vc_base )
{
   uint32_t hdr_reg,        // temps
            payload_fifo,
            bytes_per_quad;
   void *payload=0;           // This pointer is never stored to
   asm volatile( "la      %1,%c7(%4);"      // %1 = data reception fifo address
                 "li      %2,16;"           // %2 = 16 bytes per quad
                 "lfpdx   0,0,%1;"          // F0=Q0  load
                 "lfpdx   1,0,%1;"          // F1=Q1  load
                 "lfpdx   2,0,%1;"          // F2=Q2  load
                 "lfpdx   3,0,%1;"          // F3=Q3  load
                 "lfpdx   4,0,%1;"          // F4=Q4  load
                 "lfpdx   5,0,%1;"          // F5=Q5  load
                 "lfpdx   6,0,%1;"          // F6=Q6  load
                 "lfpdx   7,0,%1;"          // F7=Q7  load
                 "lfpdx   8,0,%1;"          // F8=Q8  load
                 "lfpdx   9,0,%1;"          // F9=Q9  load
                 "lfpdx   0,0,%1;"          // F0=Q10 load
                 "lfpdx   1,0,%1;"          // F1=Q11 load
                 "lfpdx   2,0,%1;"          // F2=Q12 load
                 "lfpdx   3,0,%1;"          // F3=Q13 load
                 "lfpdx   4,0,%1;"          // F4=Q14 load
                 "lfpdx   5,0,%1;"          // F5=Q15 load
                 : "=&b"  (hdr_reg),        // 0 Outputs
                   "=&b"  (payload_fifo),   // 1
                   "=&b"  (bytes_per_quad), // 2
                   "=b"   (payload)         // 3
                 : "b"    (vc_base),        // 4 Inputs
                   "b"    (header),         // 5
                   "i"    (_BGP_TRx_HR),    // 6
                   "i"    (_BGP_TRx_DR),    // 7
                   "3"    (payload)         // "payload" is input and output
                 : "fr0", "fr1", "fr2",     // Clobbers
                   "fr3", "fr4", "fr5",
                   "fr6", "fr7", "fr8",
                   "fr9", "memory" );
}
__INLINE__ void _bgp_vcX_pkt_receive_sh( uint32_t *header, struct _BGPTreePacketSoftHeader_tag * softheader, void *payload, uint32_t vc_base )
   {
     uint32_t hdr_reg,        // temps
       payload_fifo,
       bytes_per_quad;

     asm volatile( "lwz     %0,%c7(%5);"      // load Hdr from *(vc_base + hdr) into hdr_reg
		   "la      %1,%c8(%5);"      // %1 = data reception fifo address
		   "li      %2,16;"           // %2 = 16 bytes per quad
		   "lfpdx   0,0,%1;"          // F0=Q0  load
		   "lfpdx   1,0,%1;"          // F1=Q1  load
		   "lfpdx   2,0,%1;"          // F2=Q2  load
		   "stw     %0,0(%6);"        // Store header to caller's header buffer
		   "lfpdx   3,0,%1;"          // F3=Q3  load
		   "lfpdx   4,0,%1;"          // F4=Q4  load
		   "lfpdx   5,0,%1;"          // F5=Q5  load
		   "stfpdx  0,0,%4;"          // Q0  store to caller's softheader buffer
		   "stfpdx  1,0,%3;"          // Q1  store to caller's payload buffer
		   "stfpdux 2,%3,%2;"         // Q2  store to (%5 += 16)
		   "lfpdx   6,0,%1;"          // F6=Q6  load
		   "lfpdx   7,0,%1;"          // F7=Q7  load
		   "lfpdx   8,0,%1;"          // F8=Q8  load
		   "stfpdux 3,%3,%2;"         // Q3  store
		   "stfpdux 4,%3,%2;"         // Q4  store
		   "stfpdux 5,%3,%2;"         // Q5  store
		   "lfpdx   9,0,%1;"          // F9=Q9  load
		   "lfpdx   0,0,%1;"          // F0=Q10 load
		   "lfpdx   1,0,%1;"          // F1=Q11 load
		   "stfpdux 6,%3,%2;"         // Q6  store
		   "stfpdux 7,%3,%2;"         // Q7  store
		   "stfpdux 8,%3,%2;"         // Q8  store
		   "lfpdx   2,0,%1;"          // F2=Q12 load
		   "lfpdx   3,0,%1;"          // F3=Q13 load
		   "lfpdx   4,0,%1;"          // F4=Q14 load
		   "stfpdux 9,%3,%2;"         // Q9  store
		   "stfpdux 0,%3,%2;"         // Q10 store
		   "stfpdux 1,%3,%2;"         // Q11 store
		   "lfpdx   5,0,%1;"          // F5=Q15 load
		   "stfpdux 2,%3,%2;"         // Q12 store
		   "stfpdux 3,%3,%2;"         // Q13 store
		   "stfpdux 4,%3,%2;"         // Q14 store
		   "stfpdux 5,%3,%2;"         // Q15 store
		   : "=&b"  (hdr_reg),        // 0 Outputs
                   "=&b"  (payload_fifo),   // 1
                   "=&b"  (bytes_per_quad), // 2
                   "=b"   (payload),        // 3
		   "=b"   (softheader)      // 4
		   : "b"    (vc_base),        // 5 Inputs
                   "b"    (header),         // 6
                   "i"    (_BGP_TRx_HR),    // 7
                   "i"    (_BGP_TRx_DR),    // 8
                   "3"    (payload),        // 9 "payload" is input and output
		   "4"    (softheader)
		   : "fr0", "fr1", "fr2",     // Clobbers
                   "fr3", "fr4", "fr5",
                   "fr6", "fr7", "fr8",
                   "fr9", "memory" );
   }


__INLINE__ void _bgp_vcX_pkt_receive_func( uint32_t *header, struct _BGPTreePacketSoftHeader_tag* softheader, BGPTreePacketPlaceFunction fcn, void* arg, uint32_t vc_base )
{
  uint32_t hdr_reg,        // temps
    payload_fifo,
    bytes_per_quad;
  void* payload;

  asm volatile( "lwz     %0,%c6(%4);"      // load Hdr from *(vc_base + hdr) into hdr_reg
		"la      %1,%c7(%4);"      // %1 = data reception fifo address
		"li      %2,16;"           // %2 = 16 bytes per quad
		"lfpdx   0,0,%1;"          // F0=Q0  load
		"stw     %0,0(%5);"        // Store header to caller's header buffer
		"stfpdx  0,0,%3;"          // Q0  store to caller's softheader buffer
		: "=&b"  (hdr_reg),        // 0 Outputs
                "=&b"  (payload_fifo),   // 1
                "=&b"  (bytes_per_quad), // 2
                "=b"   (softheader)      // 3
                : "b"    (vc_base),        // 4 Inputs
                "b"    (header),         // 5
                "i"    (_BGP_TRx_HR),    // 6
                "i"    (_BGP_TRx_DR),    // 7
		"3"    (softheader)
                : "fr0", "memory" );

  payload = (*fcn) (arg, softheader);

  asm volatile(
		"la      %1,%c8(%5);"      // %1 = data reception fifo address
		"li      %2,16;"           // %2 = 16 bytes per quad
		"lfpdx   1,0,%1;"          // F1=Q1  load
                "lfpdx   2,0,%1;"          // F2=Q2  load
                "lfpdx   3,0,%1;"          // F3=Q3  load
                "lfpdx   4,0,%1;"          // F4=Q4  load
                "lfpdx   5,0,%1;"          // F5=Q5  load

		"stfpdx  1,0,%3;"          // Q1  store to caller's payload buffer
		"stfpdux 2,%3,%2;"         // Q2  store to (%5 += 16)
		"lfpdx   6,0,%1;"          // F6=Q6  load
		"lfpdx   7,0,%1;"          // F7=Q7  load
		"lfpdx   8,0,%1;"          // F8=Q8  load
		"stfpdux 3,%3,%2;"         // Q3  store
		"stfpdux 4,%3,%2;"         // Q4  store
		"stfpdux 5,%3,%2;"         // Q5  store
		"lfpdx   9,0,%1;"          // F9=Q9  load
		"lfpdx   0,0,%1;"          // F0=Q10 load
		"lfpdx   1,0,%1;"          // F1=Q11 load
		"stfpdux 6,%3,%2;"         // Q6  store
		"stfpdux 7,%3,%2;"         // Q7  store
		"stfpdux 8,%3,%2;"         // Q8  store
		"lfpdx   2,0,%1;"          // F2=Q12 load
		"lfpdx   3,0,%1;"          // F3=Q13 load
		"lfpdx   4,0,%1;"          // F4=Q14 load
		"stfpdux 9,%3,%2;"         // Q9  store
		"stfpdux 0,%3,%2;"         // Q10 store
		"stfpdux 1,%3,%2;"         // Q11 store
		"lfpdx   5,0,%1;"          // F5=Q15 load
		"stfpdux 2,%3,%2;"         // Q12 store
		"stfpdux 3,%3,%2;"         // Q13 store
		"stfpdux 4,%3,%2;"         // Q14 store
		"stfpdux 5,%3,%2;"         // Q15 store
		: "=&b"  (hdr_reg),        // 0 Outputs
		"=&b"  (payload_fifo),   // 1
		"=&b"  (bytes_per_quad), // 2
		"=b"   (payload),        // 3
		"=b"   (softheader)      // 4
		: "b"    (vc_base),        // 5 Inputs
		"b"    (header),         // 6
		"i"    (_BGP_TRx_HR),    // 7
		"i"    (_BGP_TRx_DR),    // 8
		"3"    (payload)         // 9 "payload" is input and output
		: "fr0", "fr1", "fr2",     // Clobbers
		"fr3", "fr4", "fr5",
		"fr6", "fr7", "fr8",
		"fr9", "memory" );
}


////////////////////////////////////////////////////////////////////////////////
// _bgp_vcX_hdr_inject: Inject a tree packet header to a VC.
//
//  Do not use this macro directly, there are wrapper functions below.
////////////////////////////////////////////////////////////////////////////////
__INLINE__ void _bgp_vcX_hdr_inject( uint32_t *header, uint32_t vc_base )
{
   uint32_t hdr_reg; // temp

   asm volatile( "lwz    %0,0(%2);"     // Load header from caller's buffer
                 "stw    %0,%c3(%1);"   // Store header to HI offset from VC base
                 : "=&r" (hdr_reg)      // 0 Outputs
                 : "b"   (vc_base),     // 1 Inputs
                   "b"   (header),      // 2
                   "i"   (_BGP_TRx_HI)  // 3
                 : "memory" );          // Clobbers
}


////////////////////////////////////////////////////////////////////////////////
// _bgp_vcX_hdr_receive: Receive a tree packet header to from a VC.
//
//  Do not use this macro directly, there are wrapper functions below.
////////////////////////////////////////////////////////////////////////////////
__INLINE__ void _bgp_vcX_hdr_receive( uint32_t *header, uint32_t vc_base )
{
   uint32_t hdr_reg; // temp

   asm volatile( "lwz    %0,%c3(%1);"   // Load header from HI offset from VC base
                 "stw    %0,0(%2);"     // Store header to caller's buffer
                 : "=&r" (hdr_reg)      // 0 Outputs
                 : "b"   (vc_base),     // 1 Inputs
                   "b"   (header),      // 2
                   "i"   (_BGP_TRx_HR)  // 3
                 : "memory" );          // Clobbers
}


////////////////////////////////////////////////////////////////////////////////
// Use these "raw" routines *only* for aligned payload of 16 quadwords (256 Bytes).
////////////////////////////////////////////////////////////////////////////////
// Inject a single header
__INLINE__ void _bgp_TreeRawSendHeader(
                    int            vc,       // Virtual channel (0 or 1)
                    _BGP_TreeHwHdr *hdrHW )  // Previously created hardware header (any type)
{
   if ( vc )
      _bgp_vcX_hdr_inject( (uint32_t *)hdrHW, _BGP_VA_TREE1 );
   else
      _bgp_vcX_hdr_inject( (uint32_t *)hdrHW, _BGP_VA_TREE0 );
}


__INLINE__ void _bgp_TreeRawSendPacketVC0(
                    _BGP_TreeHwHdr *hdrHW,  // Previously created hardware header (any type)
                    void           *pyld )  // Source address of payload (256 bytes 16-byte aligned)
{
   _bgp_vcX_pkt_inject( (uint32_t *)hdrHW, pyld, _BGP_VA_TREE0 );
}


__INLINE__ void _bgp_TreeRawSendPacketVC0_sh(
						_BGP_TreeHwHdr *hdrHW,  // Previously created hardware header (any type)
						struct _BGPTreePacketSoftHeader_tag* softheader,
						void           *pyld )  // Source address of payload (256 bytes 16-byte aligned)
{
  _bgp_vcX_pkt_inject_sh( (uint32_t *)hdrHW, softheader, pyld, _BGP_VA_TREE0 );
}


__INLINE__ void _bgp_TreeRawSendPacketVC1(
                    _BGP_TreeHwHdr *hdrHW,  // Previously created hardware header (any type)
                    void           *pyld )  // Source address of payload (256 bytes 16-byte aligned)
{
   _bgp_vcX_pkt_inject( (uint32_t *)hdrHW, pyld, _BGP_VA_TREE1 );
}


__INLINE__ void _bgp_TreeRawSendPacketVC1_sh(
						_BGP_TreeHwHdr *hdrHW,  // Previously created hardware header (any type)
						struct _BGPTreePacketSoftHeader_tag* softheader,
						void           *pyld )  // Source address of payload (256 bytes 16-byte aligned)
{
  _bgp_vcX_pkt_inject_sh( (uint32_t *)hdrHW, softheader, pyld, _BGP_VA_TREE1 );
}


__INLINE__ void _bgp_TreeRawSendPacket(
                    int vc,                 // Virtual Channel to use (0 or 1)
                    _BGP_TreeHwHdr *hdrHW,  // Previously created hardware header (any type)
                    void           *pyld )  // Source address of payload (256 bytes 16-byte aligned)
{
   if ( vc )
      _bgp_vcX_pkt_inject( (uint32_t *)hdrHW, pyld, _BGP_VA_TREE1 );
   else
      _bgp_vcX_pkt_inject( (uint32_t *)hdrHW, pyld, _BGP_VA_TREE0 );
}
__INLINE__ void _bgp_TreeRawSendPacket0(
                    int vc,                 // Virtual Channel to use (0 or 1)
                    _BGP_TreeHwHdr *hdrHW)  // Previously created hardware header (any type)
{
   if ( vc )
      _bgp_vcX_pkt_inject0( (uint32_t *)hdrHW, _BGP_VA_TREE1 );
   else
      _bgp_vcX_pkt_inject0( (uint32_t *)hdrHW, _BGP_VA_TREE0 );
}

__INLINE__ void _bgp_TreeRawReceiveHeader(
                     int            vc,       // Virtual channel (0 or 1)
                     _BGP_TreeHwHdr *hdrHW )  // Hardware header buffer
{
   if ( vc )
      _bgp_vcX_hdr_receive( (uint32_t *)hdrHW, _BGP_VA_TREE1 );
   else
      _bgp_vcX_hdr_receive( (uint32_t *)hdrHW, _BGP_VA_TREE0 );
}


__INLINE__ void _bgp_TreeRawReceivePacketVC0(
                     _BGP_TreeHwHdr *hdrHW,   // Hardware header buffer
                     void           *pyld )   // Payload buffer (must be 256 bytes, qword-aligned)
{
   _bgp_vcX_pkt_receive( (uint32_t *)hdrHW, pyld, _BGP_VA_TREE0 );
}


__INLINE__ void _bgp_TreeRawReceivePacketVC0_sh(
						_BGP_TreeHwHdr *hdrHW,   // Hardware header buffer
						struct _BGPTreePacketSoftHeader_tag           *softheader,
						void           *pyld )   // Payload buffer (must be 256 bytes, qword-aligned)
{
  _bgp_vcX_pkt_receive_sh( (uint32_t *)hdrHW, softheader, pyld, _BGP_VA_TREE0 );
}


__INLINE__ void _bgp_TreeRawReceivePacketVC0_func(
						_BGP_TreeHwHdr *hdrHW,   // Hardware header buffer
						struct _BGPTreePacketSoftHeader_tag* softheader,
						BGPTreePacketPlaceFunction fcn,
						void* arg)
{
  _bgp_vcX_pkt_receive_func( (uint32_t *)hdrHW, softheader, fcn, arg, _BGP_VA_TREE0 );
}


__INLINE__ void _bgp_TreeRawReceivePacketVC1(
                     _BGP_TreeHwHdr *hdrHW,   // Hardware header buffer
                     void           *pyld )   // Payload buffer (must be 256 bytes, qword-aligned)
{
   _bgp_vcX_pkt_receive( (uint32_t *)hdrHW, pyld, _BGP_VA_TREE1 );
}


__INLINE__ void _bgp_TreeRawReceivePacketVC1_sh(
						_BGP_TreeHwHdr *hdrHW,   // Hardware header buffer
						struct _BGPTreePacketSoftHeader_tag           *softheader,
						void           *pyld )   // Payload buffer (must be 256 bytes, qword-aligned)
{
  _bgp_vcX_pkt_receive_sh( (uint32_t *)hdrHW, softheader, pyld, _BGP_VA_TREE1 );
}


__INLINE__ void _bgp_TreeRawReceivePacketVC1_func(
						_BGP_TreeHwHdr *hdrHW,   // Hardware header buffer
						struct _BGPTreePacketSoftHeader_tag* softheader,
						BGPTreePacketPlaceFunction fcn,
						void* arg)
{
  _bgp_vcX_pkt_receive_func( (uint32_t *)hdrHW, softheader, fcn, arg, _BGP_VA_TREE1 );
}


__INLINE__ void _bgp_TreeRawReceivePacket(
                     int            vc,       // Virtual channel (0 or 1)
                     _BGP_TreeHwHdr *hdrHW,   // Hardware header buffer
                     void           *pyld )   // Payload buffer (must be 256 bytes, qword-aligned)
{
   if ( vc )
      _bgp_vcX_pkt_receive( (uint32_t *)hdrHW, pyld, _BGP_VA_TREE1 );
   else
      _bgp_vcX_pkt_receive( (uint32_t *)hdrHW, pyld, _BGP_VA_TREE0 );
}

__INLINE__ void _bgp_TreeRawReceivePacketNoHdr(
                     int            vc,       // Virtual channel (0 or 1)
                     void           *pyld )   // Payload buffer (must be 256 bytes, qword-aligned)
{
   if ( vc )
      _bgp_vcX_pkt_receiveNoHdr( (uint32_t *)0, pyld, _BGP_VA_TREE1 );
   else
      _bgp_vcX_pkt_receiveNoHdr( (uint32_t *)0, pyld, _BGP_VA_TREE0 );
}

__INLINE__ void _bgp_TreeRawReceivePacketNoHdrNoStore(
                     int            vc)       // Virtual channel (0 or 1)

{
   if ( vc )
      _bgp_vcX_pkt_receiveNoHdrNoStore( (uint32_t *)0,_BGP_VA_TREE1 );
   else
      _bgp_vcX_pkt_receiveNoHdrNoStore( (uint32_t *)0,_BGP_VA_TREE0 );
}


/*---------------------------------------------------------------------------*
 *  Kernel Initialization and Interrupt Service Routine Interface            *
 *---------------------------------------------------------------------------*/
extern int _bgp_Collective_Init( _BGP_Personality_t *p );
extern int _bgp_Collective_Release( _BGP_Personality_t *p );
extern int _bgp_Collective_HardReset( _BGP_Personality_t *p );

extern int _bgp_Collective_ConfigureClass( uint32_t vt,          // 0..15
                                           uint16_t specifier ); // bitwise OR of _BGP_TREE_RDR_*

extern int _bgp_Collective_ConfigureClass_Internal( uint32_t vt,
                                                    uint16_t specifier );

extern uint32_t _bgp_Collective_GetClass( uint32_t vt );    // Virtual tree (class) number

// Interrupt Service Routines
void _bgp_ISR_Collective_RouterTimeOut( int group, uint32_t status );
void _bgp_ISR_Collective_NoTargetTrap( int group, uint32_t status );
void _bgp_ISR_Collective_ALUOverFlow( int group, uint32_t status );
void _bgp_ISR_Collective_LocalInjection( int group, uint32_t status );
void _bgp_ISR_Collective_LocalReception( int group, uint32_t status );

void _bgp_ISR_Collective_WriteToFullChannel0( int group, uint32_t status );
void _bgp_ISR_Collective_ECCExceptionChannel0Send( int group, uint32_t status );
void _bgp_ISR_Collective_LinkCRCExceptionChannel0Send( int group, uint32_t status );

void _bgp_ISR_Collective_WriteToFullChannel1( int group, uint32_t status );
void _bgp_ISR_Collective_ECCExceptionChannel1Send( int group, uint32_t status );
void _bgp_ISR_Collective_LinkCRCExceptionChannel1Send( int group, uint32_t status );

void _bgp_ISR_Collective_WriteToFullChannel2( int group, uint32_t status );
void _bgp_ISR_Collective_ECCExceptionChannel2Send( int group, uint32_t status );
void _bgp_ISR_Collective_LinkCRCExceptionChannel2Send( int group, uint32_t status );

void _bgp_ISR_Collective_ECCExceptionChannel0Receive( int group, uint32_t status );
void _bgp_ISR_Collective_LinkCRCExceptionChannel0Receive( int group, uint32_t status );
void _bgp_ISR_Collective_ECCExceptionChannel1Receive( int group, uint32_t status );
void _bgp_ISR_Collective_LinkCRCExceptionChannel1Receive( int group, uint32_t status );
void _bgp_ISR_Collective_ECCExceptionChannel2Receive( int group, uint32_t status );
void _bgp_ISR_Collective_LinkCRCExceptionChannel2Receive( int group, uint32_t status );

void _bgp_ISR_Collective_InjectionException( int group, uint32_t status );
void _bgp_ISR_Collective_ReceptionException( int group, uint32_t status );

void _bgp_ISR_Collective_VC0PacketInterrupt( int group, uint32_t status );
void _bgp_ISR_Collective_VC1PacketInterrupt( int group, uint32_t status );

// Correctable Error Thresholds Exceeded
void _bgp_ISR_Collective_Recv0_SEC( int group, uint32_t status );
void _bgp_ISR_Collective_Recv0_CRC( int group, uint32_t status );
void _bgp_ISR_Collective_Send0_SEC( int group, uint32_t status );
void _bgp_ISR_Collective_Send0_Retry( int group, uint32_t status );
void _bgp_ISR_Collective_Recv1_SEC( int group, uint32_t status );
void _bgp_ISR_Collective_Recv1_CRC( int group, uint32_t status );
void _bgp_ISR_Collective_Send1_SEC( int group, uint32_t status );
void _bgp_ISR_Collective_Send1_Retry( int group, uint32_t status );
void _bgp_ISR_Collective_Recv2_SEC( int group, uint32_t status );
void _bgp_ISR_Collective_Recv2_CRC( int group, uint32_t status );
void _bgp_ISR_Collective_Send2_SEC( int group, uint32_t status );
void _bgp_ISR_Collective_Send2_Retry( int group, uint32_t status );
void _bgp_ISR_Collective_INJ_SEC( int group, uint32_t status );
void _bgp_ISR_Collective_REC_SEC( int group, uint32_t status );

__END_DECLS



#endif // Add nothing below this line

