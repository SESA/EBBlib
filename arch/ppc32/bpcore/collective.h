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
 * \file bpcore/collective.h
 */

#ifndef	_COLLECTIVE_H_ // Prevent multiple inclusion
#define	_COLLECTIVE_H_



#include <common/namespace.h>

__BEGIN_DECLS

#ifdef _AIX
#include <inttypes.h>
#else
#include <stdint.h>
#endif

#include <common/linkage.h>
#include <common/bgp_bitnumbers.h>
#include <bpcore/bgp_types.h>
#include <bpcore/bgp_dcrmap.h>


#define _BGP_TRx_DI      (0x00)    // Offset from Tree VCx for Data   Injection   (WO,Quad)
#define _BGP_TRx_HI      (0x10)    // Offset from Tree VCx for Header Injection   (WO,Word)
#define _BGP_TRx_DR      (0x20)    // Offset from Tree VCx for Data   Reception   (RO,Quad)
#define _BGP_TRx_HR      (0x30)    // Offset from Tree VCx for Header Reception   (RO,Word)
#define _BGP_TRx_Sx      (0x40)    // Offset from Tree VCx for Status             (RO,Word)
#define _BGP_TRx_SO      (0x50)    // Offset from Tree VCx for Status of Other VC (RO,Word)

// Virtual Addresses for Tree VC0
#define _BGP_TR0_DI    (_BGP_VA_TREE0 | _BGP_TRx_DI)
#define _BGP_TR0_HI    (_BGP_VA_TREE0 | _BGP_TRx_HI)
#define _BGP_TR0_DR    (_BGP_VA_TREE0 | _BGP_TRx_DR)
#define _BGP_TR0_HR    (_BGP_VA_TREE0 | _BGP_TRx_HR)
#define _BGP_TR0_S0    (_BGP_VA_TREE0 | _BGP_TRx_Sx)
#define _BGP_TR0_S1    (_BGP_VA_TREE0 | _BGP_TRx_SO)

// Virtual Addresses for Tree VC1
#define _BGP_TR1_DI    (_BGP_VA_TREE1 | _BGP_TRx_DI)
#define _BGP_TR1_HI    (_BGP_VA_TREE1 | _BGP_TRx_HI)
#define _BGP_TR1_DR    (_BGP_VA_TREE1 | _BGP_TRx_DR)
#define _BGP_TR1_HR    (_BGP_VA_TREE1 | _BGP_TRx_HR)
#define _BGP_TR1_S1    (_BGP_VA_TREE1 | _BGP_TRx_Sx)
#define _BGP_TR1_S0    (_BGP_VA_TREE1 | _BGP_TRx_SO)

// Packet Payload: fixed size for all Tree packets
#define _BGP_TREE_PKT_MAX_BYTES    (256)       // bytes in a tree packet
#define _BGP_TREE_PKT_MAX_SHORT    (128)
#define _BGP_TREE_PKT_MAX_LONG      (64)
#define _BGP_TREE_PKT_MAX_LONGLONG  (32)
#define _BGP_TREE_PKT_MAX_QUADS     (16)       // quads in a tree packet


// Packet header
#define  _BGP_TR_HDR_CLASS(x)           _B4( 3,x)      //  Packet class (virtual tree)
#define  _BGP_TR_HDR_P2P                _BN( 4)        //  Point-to-point enable
#define  _BGP_TR_HDR_IRQ                _BN( 5)        //  Interrupt request (at receiver) enable
#define  _BGP_TR_HDR_OPCODE(x)          _B3( 8,x)      //  ALU opcode
#define    _BGP_TR_OP_NONE                0x0          //    No operand.  Use for ordinary routed packets.
#define    _BGP_TR_OP_OR                  0x1          //    Bitwise logical OR.
#define    _BGP_TR_OP_AND                 0x2          //    Bitwise logical AND.
#define    _BGP_TR_OP_XOR                 0x3          //    Bitwise logical XOR.
#define    _BGP_TR_OP_MAX                 0x5          //    Unsigned integer maximum.
#define    _BGP_TR_OP_ADD                 0x6          //    Unsigned integer addition.
#define  _BGP_TR_HDR_OPSIZE(x)          _B7(15,x)      //  Operand size (# of 16-bit words minus 1)
#define  _BGP_TR_HDR_TAG(x)             _B14(29,x)     //  User-specified tag (for ordinary routed packets only)
#define  _BGP_TR_HDR_NADDR(x)           _B24(29,x)     //  Target address (for P2P packets only)
#define  _BGP_TR_HDR_CSUM(x)            _B2(31,x)      //  Injection checksum mode
#define    _BGP_TR_CSUM_NONE              0x0          //    Do not include packet in checksums.
#define    _BGP_TR_CSUM_SOME              0x1          //    Include header in header checksum.  Include all but
                                                       //     first quadword in payload checksum.
#define    _BGP_TR_CSUM_CFG               0x2          //    Include header in header checksum.  Include all but
                                                       //     specified number of 16-bit words in payload checksum.
#define    _BGP_TR_CSUM_ALL               0x3          //    Include entire packet in checksums.

// Packet status
#define  _BGP_TR_STAT_IPY_CNT(x)        _B8( 7,x)      //  Injection payload qword count
#define  _BGP_TR_STAT_IHD_CNT(x)        _B4(15,x)      //  Injection header word count
#define  _BGP_TR_STAT_RPY_CNT(x)        _B8(23,x)      //  Reception payload qword count
#define  _BGP_TR_STAT_IRQ               _BN(27)        //  One or more reception headers with IRQ bit set
#define  _BGP_TR_STAT_RHD_CNT(x)        _B4(31,x)      //  Reception header word count

// Tree Map of DCR Groupings
#define _BGP_DCR_TR_CLASS  (_BGP_DCR_TREE + 0x00)      // Class Definition Registers (R/W)
#define _BGP_DCR_TR_DMA    (_BGP_DCR_TREE + 0x0C)      // Network Port Diagnostic Memory Access Registers (R/W)
#define _BGP_DCR_TR_ARB    (_BGP_DCR_TREE + 0x10)      // Arbiter Control Registers (R/W)
#define _BGP_DCR_TR_CH0    (_BGP_DCR_TREE + 0x20)      // Channel 0 Control Registers (R/W)
#define _BGP_DCR_TR_CH1    (_BGP_DCR_TREE + 0x28)      // Channel 1 Control Registers (R/W)
#define _BGP_DCR_TR_CH2    (_BGP_DCR_TREE + 0x30)      // Channel 2 Control Registers (R/W)
#define _BGP_DCR_TR_GLOB   (_BGP_DCR_TREE + 0x40)      // Global Registers (R/W)
#define _BGP_DCR_TR_REC    (_BGP_DCR_TREE + 0x44)      // Processor Reception Registers (R/W)
#define _BGP_DCR_TR_INJ    (_BGP_DCR_TREE + 0x48)      // Processor Injection Registers (R/W)
#define _BGP_DCR_TR_LCRC   (_BGP_DCR_TREE + 0x50)      // Link CRC's
#define _BGP_DCR_TR_ERR    (_BGP_DCR_TREE + 0x60)      // Internal error counters


// Tree Class Registers
// Note: each route descriptor register contains two class descriptors.  "LO" will refer to the lower-numbered
//       of the two and "HI" will refer to the higher numbered.
#define _BGP_DCR_TR_CLASS_RDR0     (_BGP_DCR_TR_CLASS + 0x00)  // CLASS: Route Descriptor Register for classes 0,  1
#define _BGP_DCR_TR_CLASS_RDR1     (_BGP_DCR_TR_CLASS + 0x01)  // CLASS: Route Descriptor Register for classes 2,  3
#define _BGP_DCR_TR_CLASS_RDR2     (_BGP_DCR_TR_CLASS + 0x02)  // CLASS: Route Descriptor Register for classes 4,  5
#define _BGP_DCR_TR_CLASS_RDR3     (_BGP_DCR_TR_CLASS + 0x03)  // CLASS: Route Descriptor Register for classes 6,  7
#define _BGP_DCR_TR_CLASS_RDR4     (_BGP_DCR_TR_CLASS + 0x04)  // CLASS: Route Descriptor Register for classes 8,  9
#define _BGP_DCR_TR_CLASS_RDR5     (_BGP_DCR_TR_CLASS + 0x05)  // CLASS: Route Descriptor Register for classes 10, 11
#define _BGP_DCR_TR_CLASS_RDR6     (_BGP_DCR_TR_CLASS + 0x06)  // CLASS: Route Descriptor Register for classes 12, 13
#define _BGP_DCR_TR_CLASS_RDR7     (_BGP_DCR_TR_CLASS + 0x07)  // CLASS: Route Descriptor Register for classes 14, 15
#define  _TR_CLASS_RDR_LO_SRC2      _BN( 1)                    //  Class low,  source channel 2
#define  _TR_CLASS_RDR_LO_SRC1      _BN( 2)                    //  Class low,  source channel 1
#define  _TR_CLASS_RDR_LO_SRC0      _BN( 3)                    //  Class low,  source channel 0
#define  _TR_CLASS_RDR_LO_TGT2      _BN( 5)                    //  Class low,  target channel 2
#define  _TR_CLASS_RDR_LO_TGT1      _BN( 6)                    //  Class low,  target channel 1
#define  _TR_CLASS_RDR_LO_TGT0      _BN( 7)                    //  Class low,  target channel 0
#define  _TR_CLASS_RDR_LO_SRCL      _BN(14)                    //  Class low,  source local client (injection)
#define  _TR_CLASS_RDR_LO_TGTL      _BN(15)                    //  Class low,  target local client (reception)
#define  _TR_CLASS_RDR_HI_SRC2      _BN(17)                    //  Class high, source channel 2
#define  _TR_CLASS_RDR_HI_SRC1      _BN(18)                    //  Class high, source channel 1
#define  _TR_CLASS_RDR_HI_SRC0      _BN(19)                    //  Class high, source channel 0
#define  _TR_CLASS_RDR_HI_TGT2      _BN(21)                    //  Class high, target channel 2
#define  _TR_CLASS_RDR_HI_TGT1      _BN(22)                    //  Class high, target channel 1
#define  _TR_CLASS_RDR_HI_TGT0      _BN(23)                    //  Class high, target channel 0
#define  _TR_CLASS_RDR_HI_SRCL      _BN(30)                    //  Class high, source local client (injection)
#define  _TR_CLASS_RDR_HI_TGTL      _BN(31)                    //  Class high, target local client (reception)
#define _BGP_DCR_TR_CLASS_ISRA     (_BGP_DCR_TR_CLASS + 0x08)  // CLASS: Bits 0-31 of 64-bit idle pattern
#define _BGP_DCR_TR_CLASS_ISRB     (_BGP_DCR_TR_CLASS + 0x09)  // CLASS: Bits 32-63 of 64-bit idle pattern

// Tree Network Port Diagnostic Memory Access Registers
// Note: Diagnostic access to processor injection and reception fifos is through TR_REC and TR_INJ registers.
#define _BGP_DCR_TR_DMA_DMAA       (_BGP_DCR_TR_DMA + 0x00)   // DMA: Diagnostic SRAM address
#define  _TR_DMA_DMAA_TGT(x)        _B3(21,x)                 //  Target
#define   _TR_DMAA_TGT_RCV0           0x0                     //   Channel 0 receiver
#define   _TR_DMAA_TGT_RCV1           0x1                     //   Channel 1 receiver
#define   _TR_DMAA_TGT_RCV2           0x2                     //   Channel 2 receiver
#define   _TR_DMAA_TGT_SND0           0x4                     //   Channel 0 sender
#define   _TR_DMAA_TGT_SND1           0x5                     //   Channel 1 sender
#define   _TR_DMAA_TGT_SND2           0x6                     //   Channel 2 sender
#define  _TR_DMA_DMAA_VC(x)         _B1(22,x)                 //  Virtual channel
#define  _TR_DMA_DMAA_PCKT(x)       _B2(24,x)                 //  Packet number
#define  _TR_DMA_DMAA_WORD(x)       _B7(31,x)                 //  Word offset within packet
#define _BGP_DCR_TR_DMA_DMAD       (_BGP_DCR_TR_DMA + 0x01)   // DMA: Diagnostic SRAM data
#define _BGP_DCR_TR_DMA_DMADI      (_BGP_DCR_TR_DMA + 0x02)   // DMA: Diagnostic SRAM data with address increment
#define  _TR_DMA_DMAD_ECC(x)        _B6(15,x)                 //  ECC
#define  _TR_DMA_DMAD_DATA(x)       _B16(31,x)                //  Data
#define _BGP_DCR_TR_DMA_DMAH       (_BGP_DCR_TR_DMA + 0x03)   // DMA: Diagnostic header access

// Tree Arbiter Control Registers
#define _BGP_DCR_TR_ARB_RCFG       (_BGP_DCR_TR_ARB + 0x00)   // ARB: General router configuration
#define  _TR_ARB_RCFG_SRC00         _BN( 0)                   //  Disable source channel 0, VC0
#define  _TR_ARB_RCFG_SRC01         _BN( 1)                   //  Disable source channel 0, VC1
#define  _TR_ARB_RCFG_TGT00         _BN( 2)                   //  Disable target channel 0, VC0
#define  _TR_ARB_RCFG_TGT01         _BN( 3)                   //  Disable target channel 0, VC1
#define  _TR_ARB_RCFG_SRC10         _BN( 4)                   //  Disable source channel 1, VC0
#define  _TR_ARB_RCFG_SRC11         _BN( 5)                   //  Disable source channel 1, VC1
#define  _TR_ARB_RCFG_TGT10         _BN( 6)                   //  Disable target channel 1, VC0
#define  _TR_ARB_RCFG_TGT11         _BN( 7)                   //  Disable target channel 1, VC1
#define  _TR_ARB_RCFG_SRC20         _BN( 8)                   //  Disable source channel 2, VC0
#define  _TR_ARB_RCFG_SRC21         _BN( 9)                   //  Disable source channel 2, VC1
#define  _TR_ARB_RCFG_TGT20         _BN(10)                   //  Disable target channel 2, VC0
#define  _TR_ARB_RCFG_TGT21         _BN(11)                   //  Disable target channel 2, VC1
#define  _TR_ARB_RCFG_LB2           _BN(25)                   //  Channel 2 loopback enable
#define  _TR_ARB_RCFG_LB1           _BN(26)                   //  Channel 1 loopback enable
#define  _TR_ARB_RCFG_LB0           _BN(27)                   //  Channel 0 loopback enable
#define  _TR_ARB_RCFG_TOM(x)        _B2(29,x)                 //  Timeout mode
#define   _TR_RCFG_TOM_NONE           0x0                     //   Disable.
#define   _TR_RCFG_TOM_NRML           0x1                     //   Normal mode, irq enabled.
#define   _TR_RCFG_TOM_WD             0x2                     //   Watchdog mode, irq enabled.
#define  _TR_ARB_RCFG_MAN           _BN(30)                   //  Manual mode (router is disabled).
#define  _TR_ARB_RCFG_RST           _BN(31)                   //  Full arbiter reset.
#define _BGP_DCR_TR_ARB_RTO        (_BGP_DCR_TR_ARB + 0x01)   // ARB: 32 MSBs of router timeout value
#define _BGP_DCR_TR_ARB_RTIME      (_BGP_DCR_TR_ARB + 0x02)   // ARB: Value of router timeout counter
#define _BGP_DCR_TR_ARB_RSTAT      (_BGP_DCR_TR_ARB + 0x03)   // ARB: General router status
#define  _TR_ARB_RSTAT_REQ20        _BN( 0)                   //  Packet available in channel 2, VC0
#define  _TR_ARB_RSTAT_REQ10        _BN( 1)                   //  Packet available in channel 1, VC0
#define  _TR_ARB_RSTAT_REQ00        _BN( 2)                   //  Packet available in channel 0, VC0
#define  _TR_ARB_RSTAT_REQP0        _BN( 3)                   //  Packet available in local client, VC0
#define  _TR_ARB_RSTAT_REQ21        _BN( 4)                   //  Packet available in channel 2, VC1
#define  _TR_ARB_RSTAT_REQ11        _BN( 5)                   //  Packet available in channel 1, VC1
#define  _TR_ARB_RSTAT_REQ01        _BN( 6)                   //  Packet available in channel 0, VC1
#define  _TR_ARB_RSTAT_REQP1        _BN( 7)                   //  Packet available in local client, VC1
#define  _TR_ARB_RSTAT_FUL20        _BN( 8)                   //  Channel 2, VC0 is full
#define  _TR_ARB_RSTAT_FUL10        _BN( 9)                   //  Channel 1, VC0 is full
#define  _TR_ARB_RSTAT_FUL00        _BN(10)                   //  Channel 0, VC0 is full
#define  _TR_ARB_RSTAT_FULP0        _BN(11)                   //  Local client, VC0 is full
#define  _TR_ARB_RSTAT_FUL21        _BN(12)                   //  Channel 2, VC1 is full
#define  _TR_ARB_RSTAT_FUL11        _BN(13)                   //  Channel 1, VC1 is full
#define  _TR_ARB_RSTAT_FUL01        _BN(14)                   //  Channel 0, VC1 is full
#define  _TR_ARB_RSTAT_FULP1        _BN(15)                   //  Local client, VC1 is full
#define  _TR_ARB_RSTAT_MAT20        _BN(16)                   //  Channel 2, VC0 is mature
#define  _TR_ARB_RSTAT_MAT10        _BN(17)                   //  Channel 1, VC0 is mature
#define  _TR_ARB_RSTAT_MAT00        _BN(18)                   //  Channel 0, VC0 is mature
#define  _TR_ARB_RSTAT_MATP0        _BN(19)                   //  Local client, VC0 is mature
#define  _TR_ARB_RSTAT_MAT21        _BN(20)                   //  Channel 2, VC1 is mature
#define  _TR_ARB_RSTAT_MAT11        _BN(21)                   //  Channel 1, VC1 is mature
#define  _TR_ARB_RSTAT_MAT01        _BN(22)                   //  Channel 0, VC1 is mature
#define  _TR_ARB_RSTAT_MATP1        _BN(23)                   //  Local client, VC1 is mature
#define  _TR_ARB_RSTAT_BSY20        _BN(24)                   //  Channel 2, VC0 is busy
#define  _TR_ARB_RSTAT_BSY10        _BN(25)                   //  Channel 1, VC0 is busy
#define  _TR_ARB_RSTAT_BSY00        _BN(26)                   //  Channel 0, VC0 is busy
#define  _TR_ARB_RSTAT_BSYP0        _BN(27)                   //  Local client, VC0 is busy
#define  _TR_ARB_RSTAT_BSY21        _BN(28)                   //  Channel 2, VC1 is busy
#define  _TR_ARB_RSTAT_BSY11        _BN(29)                   //  Channel 1, VC1 is busy
#define  _TR_ARB_RSTAT_BSY01        _BN(30)                   //  Channel 0, VC1 is busy
#define  _TR_ARB_RSTAT_BSYP1        _BN(31)                   //  Local client, VC1 is busy
#define _BGP_DCR_TR_ARB_HD00       (_BGP_DCR_TR_ARB + 0x04)   // ARB: Next header, channel 0, VC0
#define _BGP_DCR_TR_ARB_HD01       (_BGP_DCR_TR_ARB + 0x05)   // ARB: Next header, channel 0, VC1
#define _BGP_DCR_TR_ARB_HD10       (_BGP_DCR_TR_ARB + 0x06)   // ARB: Next header, channel 1, VC0
#define _BGP_DCR_TR_ARB_HD11       (_BGP_DCR_TR_ARB + 0x07)   // ARB: Next header, channel 1, VC1
#define _BGP_DCR_TR_ARB_HD20       (_BGP_DCR_TR_ARB + 0x08)   // ARB: Next header, channel 2, VC0
#define _BGP_DCR_TR_ARB_HD21       (_BGP_DCR_TR_ARB + 0x09)   // ARB: Next header, channel 2, VC1
#define _BGP_DCR_TR_ARB_HDI0       (_BGP_DCR_TR_ARB + 0x0A)   // ARB: Next header, injection, VC0
#define _BGP_DCR_TR_ARB_HDI1       (_BGP_DCR_TR_ARB + 0x0B)   // ARB: Next header, injection, VC1
#define _BGP_DCR_TR_ARB_FORCEC     (_BGP_DCR_TR_ARB + 0x0C)   // ARB: Force control for manual mode
#define  _TR_ARB_FORCEC_CH0         _BN( 0)                   //  Channel 0 is a target
#define  _TR_ARB_FORCEC_CH1         _BN( 1)                   //  Channel 1 is a target
#define  _TR_ARB_FORCEC_CH2         _BN( 2)                   //  Channel 2 is a target
#define  _TR_ARB_FORCEC_P           _BN( 3)                   //  Local client is a target
#define  _TR_ARB_FORCEC_ALU         _BN( 4)                   //  ALU is a target
#define  _TR_ARB_FORCEC_RT          _BN( 5)                   //  Force route immediately
#define  _TR_ARB_FORCEC_STK         _BN( 6)                   //  Sticky route: always force route
#define _BGP_DCR_TR_ARB_FORCER     (_BGP_DCR_TR_ARB + 0x0D)   // ARB: Forced route for manual mode
#define  _TR_ARB_FORCER_CH20        _BN( 0)                   //  Channel 2 is a source for channel 0
#define  _TR_ARB_FORCER_CH10        _BN( 1)                   //  Channel 1 is a source for channel 0
#define  _TR_ARB_FORCER_CH00        _BN( 2)                   //  Channel 0 is a source for channel 0
#define  _TR_ARB_FORCER_CHP0        _BN( 3)                   //  Local client is a source for channel 0
#define  _TR_ARB_FORCER_CHA0        _BN( 4)                   //  ALU is a source for channel 0
#define  _TR_ARB_FORCER_VC0         _BN( 5)                   //  VC that is source for channel 0
#define  _TR_ARB_FORCER_CH21        _BN( 6)                   //  Channel 2 is a source for channel 1
#define  _TR_ARB_FORCER_CH11        _BN( 7)                   //  Channel 1 is a source for channel 1
#define  _TR_ARB_FORCER_CH01        _BN( 8)                   //  Channel 0 is a source for channel 1
#define  _TR_ARB_FORCER_CHP1        _BN( 9)                   //  Local client is a source for channel 1
#define  _TR_ARB_FORCER_CHA1        _BN(10)                   //  ALU is a source for channel 1
#define  _TR_ARB_FORCER_VC1         _BN(11)                   //  VC that is source for channel 1
#define  _TR_ARB_FORCER_CH22        _BN(12)                   //  Channel 2 is a source for channel 2
#define  _TR_ARB_FORCER_CH12        _BN(13)                   //  Channel 1 is a source for channel 2
#define  _TR_ARB_FORCER_CH02        _BN(14)                   //  Channel 0 is a source for channel 2
#define  _TR_ARB_FORCER_CHP2        _BN(15)                   //  Local client is a source for channel 2
#define  _TR_ARB_FORCER_CHA2        _BN(16)                   //  ALU is a source for channel 2
#define  _TR_ARB_FORCER_VC2         _BN(17)                   //  VC that is source for channel 2
#define  _TR_ARB_FORCER_CH2P        _BN(18)                   //  Channel 2 is a source for local client
#define  _TR_ARB_FORCER_CH1P        _BN(19)                   //  Channel 1 is a source for local client
#define  _TR_ARB_FORCER_CH0P        _BN(20)                   //  Channel 0 is a source for local client
#define  _TR_ARB_FORCER_CHPP        _BN(21)                   //  Local client is a source for local client
#define  _TR_ARB_FORCER_CHAP        _BN(22)                   //  ALU is a source for local client
#define  _TR_ARB_FORCER_VCP         _BN(23)                   //  VC that is source for local client
#define  _TR_ARB_FORCER_CH2A        _BN(24)                   //  Channel 2 is a source for ALU
#define  _TR_ARB_FORCER_CH1A        _BN(25)                   //  Channel 1 is a source for ALU
#define  _TR_ARB_FORCER_CH0A        _BN(26)                   //  Channel 0 is a source for ALU
#define  _TR_ARB_FORCER_CHPA        _BN(27)                   //  Local client is a source for ALU
#define  _TR_ARB_FORCER_CHAA        _BN(28)                   //  ALU is a source for ALU
#define  _TR_ARB_FORCER_VCA         _BN(29)                   //  VC that is source for ALU
#define _BGP_DCR_TR_ARB_FORCEH     (_BGP_DCR_TR_ARB + 0x0E)   // ARB: Forced header for manual mode
#define _BGP_DCR_TR_ARB_XSTAT      (_BGP_DCR_TR_ARB + 0x0F)   // ARB: Extended router status
#define  _TR_ARB_XSTAT_BLK20        _BN( 0)                   //  Request from channel 2, VC0 is blocked
#define  _TR_ARB_XSTAT_BLK10        _BN( 1)                   //  Request from channel 1, VC0 is blocked
#define  _TR_ARB_XSTAT_BLK00        _BN( 2)                   //  Request from channel 0, VC0 is blocked
#define  _TR_ARB_XSTAT_BLKP0        _BN( 3)                   //  Request from local client, VC0 is blocked
#define  _TR_ARB_XSTAT_BLK21        _BN( 4)                   //  Request from channel 2, VC1 is blocked
#define  _TR_ARB_XSTAT_BLK11        _BN( 5)                   //  Request from channel 1, VC1 is blocked
#define  _TR_ARB_XSTAT_BLK01        _BN( 6)                   //  Request from channel 0, VC1 is blocked
#define  _TR_ARB_XSTAT_BLKP1        _BN( 7)                   //  Request from local client, VC1 is blocked
#define  _TR_ARB_XSTAT_BSYR2        _BN( 8)                   //  Channel 2 receiver is busy
#define  _TR_ARB_XSTAT_BSYR1        _BN( 9)                   //  Channel 1 receiver is busy
#define  _TR_ARB_XSTAT_BSYR0        _BN(10)                   //  Channel 0 receiver is busy
#define  _TR_ARB_XSTAT_BSYPI        _BN(11)                   //  Local client injection is busy
#define  _TR_ARB_XSTAT_BSYA         _BN(12)                   //  ALU is busy
#define  _TR_ARB_XSTAT_BSYS2        _BN(13)                   //  Channel 2 sender is busy
#define  _TR_ARB_XSTAT_BSYS1        _BN(14)                   //  Channel 1 sender is busy
#define  _TR_ARB_XSTAT_BSYS0        _BN(15)                   //  Channel 0 sender is busy
#define  _TR_ARB_XSTAT_BSYPR        _BN(16)                   //  Local client reception is busy
#define  _TR_ARB_XSTAT_ARB_TO(x)    _B15(31,x)                //  Greedy-Arbitration timeout

// Tree Channel 0 Control Registers
#define _BGP_DCR_TR_CH0_RSTAT      (_BGP_DCR_TR_CH0 + 0x00)   // CH0: Receiver status
#define  _TR_RSTAT_RCVERR           _BN( 0)                   //  Receiver error
#define  _TR_RSTAT_LHEXP            _BN( 1)                   //  Expect link header
#define  _TR_RSTAT_PH0EXP           _BN( 2)                   //  Expect packet header 0
#define  _TR_RSTAT_PH1EXP           _BN( 3)                   //  Expect packet header 1
#define  _TR_RSTAT_PDRCV            _BN( 4)                   //  Receive packet data
#define  _TR_RSTAT_CWEXP            _BN( 5)                   //  Expect packet control word
#define  _TR_RSTAT_CSEXP            _BN( 6)                   //  Expect packet checksum
#define  _TR_RSTAT_SCRBRD0          _B8(14,0xff)              //  VC0 fifo scoreboard
#define  _TR_RSTAT_SCRBRD1          _B8(22,0xff)              //  VC1 fifo scoreboard
#define  _TR_RSTAT_RMTSTAT          _B9(31,0x1ff)             //  Remote status
#define _BGP_DCR_TR_CH0_RCTRL      (_BGP_DCR_TR_CH0 + 0x01)   // CH0: Receiver control
#define  _TR_RCTRL_FERR             _BN( 0)                   //  Force receiver into error state
#define  _TR_RCTRL_RST              _BN( 1)                   //  Reset all internal pointers
#define  _TR_RCTRL_FRZ0             _BN( 2)                   //  Freeze VC0
#define  _TR_RCTRL_FRZ1             _BN( 3)                   //  Freeze VC1
#define  _TR_RCTRL_RCVALL           _BN( 4)                   //  Disable receiver CRC check and accept all packets
#define _BGP_DCR_TR_CH0_SSTAT      (_BGP_DCR_TR_CH0 + 0x02)   // CH0: Sender status
#define  _TR_SSTAT_SYNC             _BN( 0)                   //  Phase of sender
#define  _TR_SSTAT_ARB              _BN( 1)                   //  Arbitrating
#define  _TR_SSTAT_PH0SND           _BN( 2)                   //  Sending packet header 0
#define  _TR_SSTAT_PH1SND           _BN( 3)                   //  Sending packet header 1
#define  _TR_SSTAT_PDSND            _BN( 4)                   //  Sending packet payload
#define  _TR_SSTAT_CWSND            _BN( 5)                   //  Sending packet control word
#define  _TR_SSTAT_CSSND            _BN( 6)                   //  Sending packet checksum
#define  _TR_SSTAT_IDLSND           _BN( 7)                   //  Sending idle packet
#define  _TR_SSTAT_RPTR0            _B3(10,0x7)               //  VC0 read pointer
#define  _TR_SSTAT_WPTR0            _B3(13,0x7)               //  VC0 write pointer
#define  _TR_SSTAT_RPTR1            _B3(16,0x7)               //  VC1 read pointer
#define  _TR_SSTAT_WPTR1            _B3(19,0x7)               //  VC1 write pointer
#define _BGP_DCR_TR_CH0_SCTRL      (_BGP_DCR_TR_CH0 + 0x03)   // CH0: Sender control
#define  _TR_SCTRL_SYNC             _BN( 0)                   //  Force sender to send SYNC
#define  _TR_SCTRL_IDLE             _BN( 1)                   //  Force sender to send IDLE
#define  _TR_SCTRL_RST              _BN( 2)                   //  Reset all internal pointers
#define  _TR_SCTRL_INVMSB           _BN( 3)                   //  Invert MSB of class for loopback packets
#define  _TR_SCTRL_OFF              _BN( 4)                   //  Disable (black hole) the sender
#define _BGP_DCR_TR_CH0_TNACK      (_BGP_DCR_TR_CH0 + 0x04)   // CH0: Tolerated dalay from NACK to ACK status
#define _BGP_DCR_TR_CH0_CNACK      (_BGP_DCR_TR_CH0 + 0x05)   // CH0: Time since last NACK received
#define _BGP_DCR_TR_CH0_TIDLE      (_BGP_DCR_TR_CH0 + 0x06)   // CH0: Frequency to send IDLE packets
#define _BGP_DCR_TR_CH0_CIDLE      (_BGP_DCR_TR_CH0 + 0x07)   // CH0: Time since last IDLE sent

// Tree Channel 1 Control Registers
// Note: Register definitions are the same as those of channel 0.
#define _BGP_DCR_TR_CH1_RSTAT      (_BGP_DCR_TR_CH1 + 0x00)   // CH1: Receiver status
#define _BGP_DCR_TR_CH1_RCTRL      (_BGP_DCR_TR_CH1 + 0x01)   // CH1: Receiver control
#define _BGP_DCR_TR_CH1_SSTAT      (_BGP_DCR_TR_CH1 + 0x02)   // CH1: Sender status
#define _BGP_DCR_TR_CH1_SCTRL      (_BGP_DCR_TR_CH1 + 0x03)   // CH1: Sender control
#define _BGP_DCR_TR_CH1_TNACK      (_BGP_DCR_TR_CH1 + 0x04)   // CH1: Tolerated dalay from NACK to ACK status
#define _BGP_DCR_TR_CH1_CNACK      (_BGP_DCR_TR_CH1 + 0x05)   // CH1: Time since last NACK received
#define _BGP_DCR_TR_CH1_TIDLE      (_BGP_DCR_TR_CH1 + 0x06)   // CH1: Frequency to send IDLE packets
#define _BGP_DCR_TR_CH1_CIDLE      (_BGP_DCR_TR_CH1 + 0x07)   // CH1: Time since last IDLE sent

// Tree Channel 2 Control Registers
// Note: Register definitions are the same as those of channel 0.
#define _BGP_DCR_TR_CH2_RSTAT      (_BGP_DCR_TR_CH2 + 0x00)   // CH2: Receiver status
#define _BGP_DCR_TR_CH2_RCTRL      (_BGP_DCR_TR_CH2 + 0x01)   // CH2: Receiver control
#define _BGP_DCR_TR_CH2_SSTAT      (_BGP_DCR_TR_CH2 + 0x02)   // CH2: Sender status
#define _BGP_DCR_TR_CH2_SCTRL      (_BGP_DCR_TR_CH2 + 0x03)   // CH2: Sender control
#define _BGP_DCR_TR_CH2_TNACK      (_BGP_DCR_TR_CH2 + 0x04)   // CH2: Tolerated dalay from NACK to ACK status
#define _BGP_DCR_TR_CH2_CNACK      (_BGP_DCR_TR_CH2 + 0x05)   // CH2: Time since last NACK received
#define _BGP_DCR_TR_CH2_TIDLE      (_BGP_DCR_TR_CH2 + 0x06)   // CH2: Frequency to send IDLE packets
#define _BGP_DCR_TR_CH2_CIDLE      (_BGP_DCR_TR_CH2 + 0x07)   // CH2: Time since last IDLE sent

// Tree Global Registers
#define _BGP_DCR_TR_GLOB_FPTR      (_BGP_DCR_TR_GLOB + 0x00)  // GLOB: Fifo Pointer Register
#define  _TR_GLOB_FPTR_IPY0(x)      _B3( 3,x)                 //  VC0 injection payload FIFO packet write pointer
#define  _TR_GLOB_FPTR_IHD0(x)      _B3( 7,x)                 //  VC0 injection header  FIFO packet write pointer
#define  _TR_GLOB_FPTR_IPY1(x)      _B3(11,x)                 //  VC1 injection payload FIFO packet write pointer
#define  _TR_GLOB_FPTR_IHD1(x)      _B3(15,x)                 //  VC1 injection header  FIFO packet write pointer
#define  _TR_GLOB_FPTR_RPY0(x)      _B3(19,x)                 //  VC0 reception payload FIFO packet read  pointer
#define  _TR_GLOB_FPTR_RHD0(x)      _B3(23,x)                 //  VC0 reception header  FIFO packet read  pointer
#define  _TR_GLOB_FPTR_RPY1(x)      _B3(27,x)                 //  VC1 reception payload FIFO packet read  pointer
#define  _TR_GLOB_FPTR_RHD1(x)      _B3(31,x)                 //  VC1 reception header  FIFO packet read  pointer
#define _BGP_DCR_TR_GLOB_NADDR     (_BGP_DCR_TR_GLOB + 0x01)  // GLOB: Node Address Register
#define  _TR_GLOB_NADDR(x)          _B24(31,x)                //  Node address
#define _BGP_DCR_TR_GLOB_VCFG0     (_BGP_DCR_TR_GLOB + 0x02)  // GLOB: VC0 Configuration Register (use macros below)
#define _BGP_DCR_TR_GLOB_VCFG1     (_BGP_DCR_TR_GLOB + 0x03)  // GLOB: VC1 Configuration Register
#define  _TR_GLOB_VCFG_RCVALL       _BN( 0)                   //  Disable P2P reception filering
#define  _TR_GLOB_VCFG_CSUMX(x)     _B8(15,x)                 //  Injection checksum mode 2 exclusion
#define  _TR_GLOB_VCFG_RWM(x)       _B3(23,x)                 //  Payload reception FIFO watermark
#define  _TR_GLOB_VCFG_IWM(x)       _B3(31,x)                 //  Payload injection FIFO watermark

// Tree Processor Reception Registers
#define _BGP_DCR_TR_REC_PRXF       (_BGP_DCR_TR_REC + 0x00)   // REC: Receive Exception Flag Register
#define _BGP_DCR_TR_REC_PRXEN      (_BGP_DCR_TR_REC + 0x01)   // REC: Receive Exception Enable Register
#define  _TR_REC_PRX_APAR0          _BN( 8)                   //  P0 address parity error
#define  _TR_REC_PRX_APAR1          _BN( 9)                   //  P1 address parity error
#define  _TR_REC_PRX_ALIGN0         _BN(10)                   //  P0 address alignment error
#define  _TR_REC_PRX_ALIGN1         _BN(11)                   //  P1 address alignment error
#define  _TR_REC_PRX_ADDR0          _BN(12)                   //  P0 bad (unrecognized) address error
#define  _TR_REC_PRX_ADDR1          _BN(13)                   //  P1 bad (unrecognized) address error
#define  _TR_REC_PRX_COLL           _BN(14)                   //  FIFO read collision error
#define  _TR_REC_PRX_UE             _BN(15)                   //  Uncorrectable SRAM ECC error
#define  _TR_REC_PRX_PFU0           _BN(26)                   //  VC0 payload FIFO under-run error
#define  _TR_REC_PRX_PFU1           _BN(27)                   //  VC1 payload FIFO under-run error
#define  _TR_REC_PRX_HFU0           _BN(28)                   //  VC0 header FIFO under-run error
#define  _TR_REC_PRX_HFU1           _BN(29)                   //  VC1 header FIFO under-run error
#define  _TR_REC_PRX_WM0            _BN(30)                   //  VC0 payload FIFO above watermark
#define  _TR_REC_PRX_WM1            _BN(31)                   //  VC1 payload FIFO above watermark
#define _BGP_DCR_TR_REC_PRDA       (_BGP_DCR_TR_REC + 0x02)   // REC: Receive Diagnostic Address Register
#define  _TR_PRDA_VC(x)             _B1(21,x)                 //  Select VC to access
#define  _TR_PRDA_MAC(x)            _B1(22,x)                 //  Select SRAM macro to access
#define  _TR_PRDA_LINE(x)           _B7(29,x)                 //  Select line in SRAM or RA
#define  _TR_PRDA_TGT(x)            _B2(31,x)                 //  Select target sub-line or RA
#define   _TR_PRDA_TGT_LO             0x0                     //   Least significant word of SRAM
#define   _TR_PRDA_TGT_HI             0x1                     //   Most significant word of SRAM
#define   _TR_PRDA_TGT_ECC            0x2                     //   ECC syndrome of SRAM
#define   _TR_PRDA_TGT_HDR            0x3                     //   Header fifo
#define _BGP_DCR_TR_REC_PRDD       (_BGP_DCR_TR_REC + 0x03)   // REC: Receive Diagnostic Data Register
#define  _TR_PRDD_ECC(x)            _B8(31,x)                 //  ECC
#define  _TR_PRDD_DATA(x)           (x)                       //  Data

// Tree Processor Injection Registers
#define _BGP_DCR_TR_INJ_PIXF       (_BGP_DCR_TR_INJ + 0x00)   // INJ: Injection Exception Flag Register
#define _BGP_DCR_TR_INJ_PIXEN      (_BGP_DCR_TR_INJ + 0x01)   // INJ: Injection Exception Enable Register
#define  _TR_INJ_PIX_APAR0          _BN( 6)                   //  P0 address parity error
#define  _TR_INJ_PIX_APAR1          _BN( 7)                   //  P1 address parity error
#define  _TR_INJ_PIX_ALIGN0         _BN( 8)                   //  P0 address alignment error
#define  _TR_INJ_PIX_ALIGN1         _BN( 9)                   //  P1 address alignment error
#define  _TR_INJ_PIX_ADDR0          _BN(10)                   //  P0 bad (unrecognized) address error
#define  _TR_INJ_PIX_ADDR1          _BN(11)                   //  P1 bad (unrecognized) address error
#define  _TR_INJ_PIX_DPAR0          _BN(12)                   //  P0 data parity error
#define  _TR_INJ_PIX_DPAR1          _BN(13)                   //  P1 data parity error
#define  _TR_INJ_PIX_COLL           _BN(14)                   //  FIFO write collision error
#define  _TR_INJ_PIX_UE             _BN(15)                   //  Uncorrectable SRAM ECC error
#define  _TR_INJ_PIX_PFO0           _BN(25)                   //  VC0 payload FIFO overflow error
#define  _TR_INJ_PIX_PFO1           _BN(26)                   //  VC1 payload FIFO overflow error
#define  _TR_INJ_PIX_HFO0           _BN(27)                   //  VC0 header FIFO overflow error
#define  _TR_INJ_PIX_HFO1           _BN(28)                   //  VC1 header FIFO overflow error
#define  _TR_INJ_PIX_WM0            _BN(29)                   //  VC0 payload FIFO at or below watermark
#define  _TR_INJ_PIX_WM1            _BN(30)                   //  VC1 payload FIFO at or below watermark
#define  _TR_INJ_PIX_ENABLE         _BN(31)                   //  Injection interface enable (if enabled in PIXEN)
#define _BGP_DCR_TR_INJ_PIDA       (_BGP_DCR_TR_INJ + 0x02)   // INJ: Injection Diagnostic Address Register
//        Use _TR_PRDA_* defined above.
#define _BGP_DCR_TR_INJ_PIDD       (_BGP_DCR_TR_INJ + 0x03)   // INJ: Injection Diagnostic Data Register
//        Use _TR_PRDD_* defined above.
#define _BGP_DCR_TR_INJ_CSPY0      (_BGP_DCR_TR_INJ + 0x04)   // INJ: VC0 payload checksum
#define _BGP_DCR_TR_INJ_CSHD0      (_BGP_DCR_TR_INJ + 0x05)   // INJ: VC0 header checksum
#define _BGP_DCR_TR_INJ_CSPY1      (_BGP_DCR_TR_INJ + 0x06)   // INJ: VC1 payload checksum
#define _BGP_DCR_TR_INJ_CSHD1      (_BGP_DCR_TR_INJ + 0x07)   // INJ: VC1 header checksum


// Link CRC's for the receivers 0..2 (vc0,1)
#define _BGP_DCR_TR_LCRC_R00  (_BGP_DCR_TR_LCRC + 0)
#define _BGP_DCR_TR_LCRC_R01  (_BGP_DCR_TR_LCRC + 1)
#define _BGP_DCR_TR_LCRC_R10  (_BGP_DCR_TR_LCRC + 2)
#define _BGP_DCR_TR_LCRC_R11  (_BGP_DCR_TR_LCRC + 3)
#define _BGP_DCR_TR_LCRC_R20  (_BGP_DCR_TR_LCRC + 4)
#define _BGP_DCR_TR_LCRC_R21  (_BGP_DCR_TR_LCRC + 5)

// Link CRC'c for the senders 0..2 (vc0,1)
#define _BGP_DCR_TR_LCRC_S00  (_BGP_DCR_TR_LCRC + 8)
#define _BGP_DCR_TR_LCRC_S01  (_BGP_DCR_TR_LCRC + 9)
#define _BGP_DCR_TR_LCRC_S10  (_BGP_DCR_TR_LCRC + 10)
#define _BGP_DCR_TR_LCRC_S11  (_BGP_DCR_TR_LCRC + 11)
#define _BGP_DCR_TR_LCRC_S20  (_BGP_DCR_TR_LCRC + 12)
#define _BGP_DCR_TR_LCRC_S21  (_BGP_DCR_TR_LCRC + 13)

// Internal error counters and thresholds
#define _BGP_DCR_TR_ERR_R0_CRC   (_BGP_DCR_TR_ERR + 0x00)    // CH0: Receiver link CRC errors detected
#define _BGP_DCR_TR_ERR_R0_CE    (_BGP_DCR_TR_ERR + 0x01)    // CH0: Receiver SRAM errors corrected
#define _BGP_DCR_TR_ERR_S0_RETRY (_BGP_DCR_TR_ERR + 0x02)    // CH0: Sender link retransmissions
#define _BGP_DCR_TR_ERR_S0_CE    (_BGP_DCR_TR_ERR + 0x03)    // CH0: Sender SRAM errors corrected
#define _BGP_DCR_TR_ERR_R1_CRC   (_BGP_DCR_TR_ERR + 0x04)    // CH1: Receiver link CRC errors detected
#define _BGP_DCR_TR_ERR_R1_CE    (_BGP_DCR_TR_ERR + 0x05)    // CH1: Receiver SRAM errors corrected
#define _BGP_DCR_TR_ERR_S1_RETRY (_BGP_DCR_TR_ERR + 0x06)    // CH1: Sender link retransmissions
#define _BGP_DCR_TR_ERR_S1_CE    (_BGP_DCR_TR_ERR + 0x07)    // CH1: Sender SRAM errors corrected
#define _BGP_DCR_TR_ERR_R2_CRC   (_BGP_DCR_TR_ERR + 0x08)    // CH2: Receiver link CRC errors detected
#define _BGP_DCR_TR_ERR_R2_CE    (_BGP_DCR_TR_ERR + 0x09)    // CH2: Receiver SRAM errors corrected
#define _BGP_DCR_TR_ERR_S2_RETRY (_BGP_DCR_TR_ERR + 0x0A)    // CH2: Sender link retransmissions
#define _BGP_DCR_TR_ERR_S2_CE    (_BGP_DCR_TR_ERR + 0x0B)    // CH2: Sender SRAM errors corrected
#define _BGP_DCR_TR_ERR_INJ_SE   (_BGP_DCR_TR_ERR + 0x0C)    // INJ: SRAM errors (correctable and uncorrectable)
#define _BGP_DCR_TR_ERR_REC_SE   (_BGP_DCR_TR_ERR + 0x0D)    // REC: SRAM errors (correctable and uncorrectable)

#define _BGP_DCR_TR_ERR_R0_CRC_T   (_BGP_DCR_TR_ERR + 0x10)  // Interrupt thresholds for corresponding error
#define _BGP_DCR_TR_ERR_R0_CE_T    (_BGP_DCR_TR_ERR + 0x11)  // counters.
#define _BGP_DCR_TR_ERR_S0_RETRY_T (_BGP_DCR_TR_ERR + 0x12)
#define _BGP_DCR_TR_ERR_S0_CE_T    (_BGP_DCR_TR_ERR + 0x13)
#define _BGP_DCR_TR_ERR_R1_CRC_T   (_BGP_DCR_TR_ERR + 0x14)
#define _BGP_DCR_TR_ERR_R1_CE_T    (_BGP_DCR_TR_ERR + 0x15)
#define _BGP_DCR_TR_ERR_S1_RETRY_T (_BGP_DCR_TR_ERR + 0x16)
#define _BGP_DCR_TR_ERR_S1_CE_T    (_BGP_DCR_TR_ERR + 0x17)
#define _BGP_DCR_TR_ERR_R2_CRC_T   (_BGP_DCR_TR_ERR + 0x18)
#define _BGP_DCR_TR_ERR_R2_CE_T    (_BGP_DCR_TR_ERR + 0x19)
#define _BGP_DCR_TR_ERR_S2_RETRY_T (_BGP_DCR_TR_ERR + 0x1A)
#define _BGP_DCR_TR_ERR_S2_CE_T    (_BGP_DCR_TR_ERR + 0x1B)
#define _BGP_DCR_TR_ERR_INJ_SE_T   (_BGP_DCR_TR_ERR + 0x1C)
#define _BGP_DCR_TR_ERR_REC_SE_T   (_BGP_DCR_TR_ERR + 0x1D)

// For _bgp_tree_configure_class
#define _BGP_TREE_RDR_NUM      (16)  // classes are 0..15

// The following interface allows for fine-grain control of the RDR register
// contents.  Use bit-wize OR'd together to create a route specification.
#define _BGP_TREE_RDR_SRC0    (0x1000)  // Bit Number  3 (MSb is bit number 0)
#define _BGP_TREE_RDR_SRC1    (0x2000)  // Bit Number  2
#define _BGP_TREE_RDR_SRC2    (0x4000)  // Bit Number  1
#define _BGP_TREE_RDR_SRCL    (0x0002)  // Bit Number 14
#define _BGP_TREE_RDR_TGT0    (0x0100)  // Bit Number  7
#define _BGP_TREE_RDR_TGT1    (0x0200)  // Bit Number  6
#define _BGP_TREE_RDR_TGT2    (0x0400)  // Bit Number  5
#define _BGP_TREE_RDR_TGTL    (0x0001)  // Bit Number 15

// OR of all valid Source and Target bits for SrtTgtEnable validation.
#define _BGP_TREE_RDR_ACCEPT (0x7703)

typedef struct _BGPTreePacketSoftHeader_tag
{
  unsigned int arg0;
  unsigned int arg1;
  unsigned int arg2;
  unsigned int arg3;
} _BGPTreePacketSoftHeader;

typedef _QuadWord_t *(*BGPTreePacketPlaceFunction)(void *arg,
						   struct _BGPTreePacketSoftHeader_tag *);

#if !defined(__ASSEMBLY__) && !defined(__BGP_HOST_COMPILED__) && !defined(__MAMBO__)

/*---------------------------------------------------------------------------*
 *  Tree Fifo Status                                                         *
 *---------------------------------------------------------------------------*/
//
// Tree Fifo Status:
//   Each 32-bit status word contains all of the fifo status for a virtual channel
//   and the reception fifo interrupt request status.  Note that in reality, only
//   the low-order bit of the RecIrq field is relevant.  Similarly, InjHdrCount and
//   RecHdrCount are really 4-bit values, so only the 4 low-order bits are relevant.
//   In both cases, the hardware returns 0 for undefined bits.
#define _BGP_TREE_STATUS_MAX_PKTS (8)

typedef union  T_BGP_TreeFifoStatus
                {
                uint32_t status_word;
                struct {
                       unsigned InjPyldCount  : 8; // Injection payload fifo entry (qword) count
                       unsigned _notdefined1  : 4;
                       unsigned InjHdrCount   : 4; // Injection header fifo entry count
                       unsigned RecPyldCount  : 8; // Reception payload fifo entry (qword) count
                       unsigned _notdefined2  : 3;
                       unsigned RecIrq        : 1; // If non-zero, reception fifo contains an IRQ packet
                       unsigned RecHdrCount   : 4; // Reception header fifo entry count
                       };
                }
                _BGP_TreeFifoStatus;


/*---------------------------------------------------------------------------*
 *  Collective Network Header and Payload Definition and Routines            *
 *---------------------------------------------------------------------------*/
// ALU opcodes for collectives.
#define _BGP_TREE_OPCODE_NONE    (0x0)
#define _BGP_TREE_OPCODE_OR      (0x1)
#define _BGP_TREE_OPCODE_AND     (0x2)
#define _BGP_TREE_OPCODE_XOR     (0x3)
#define _BGP_TREE_OPCODE_MAX     (0x5)
#define _BGP_TREE_OPCODE_ADD     (0x6)

// Common operand sizes.
#define _BGP_TREE_OPSIZE_BIT16   (0x0)
#define _BGP_TREE_OPSIZE_BIT32   (0x1)
#define _BGP_TREE_OPSIZE_BIT64   (0x3)
#define _BGP_TREE_OPSIZE_BIT128  (0x7)
#define _BGP_TREE_OPSIZE_SHORT   (0x0)
#define _BGP_TREE_OPSIZE_INT     (0x1)
#define _BGP_TREE_OPSIZE_LONG    (0x3)
#define _BGP_TREE_OPSIZE_QUAD    (0x7)

// Software checksum counters.  Use these when calling BGPTreeInjectionCsumGet.
#define _BGP_TREE_CSUM_VC0_HEADER   (0)
#define _BGP_TREE_CSUM_VC0_PAYLOAD  (1)
#define _BGP_TREE_CSUM_VC1_HEADER   (2)
#define _BGP_TREE_CSUM_VC1_PAYLOAD  (3)

// Checksum mode.
#define _BGP_TREE_CSUM_NONE      (0x0)   // Do not include packet in checksums.
#define _BGP_TREE_CSUM_SOME      (0x1)   // Include header in header checksum.  Include all but
                                         //   first quadword in payload checksum.
#define _BGP_TREE_CSUM_CFG       (0x2)   // Include header in header checksum.  Include all but
                                         //   specified number of 16-bit words in payload checksum.
#define _BGP_TREE_CSUM_ALL       (0x3)   // Include entire packet in checksums.

// The Tree Hardware Packet Headers
//  There are two hardware header formats: one for point-to-point packets and the
//  other for collective (basically all other) packets.
//
//  These structures should be considered "opaque".  They are supplied for informational
//  purposes only.  Functions are provided to construct/fillout these headers.
typedef union T_BGP_TreePtpHdr
               {
               uint32_t word;
               struct {
                      unsigned Class      : 4; // Class number (a.k.a virtual tree number)
                      unsigned Ptp        : 1; // Must be 1 for PTP header format.
                      unsigned Irq        : 1; // 1=request interrupt when received, 0=not
                      unsigned PtpTarget  :24; // Ptp packet target (matched to node address)
                      unsigned CsumMode   : 2; // Injection checksum mode
                      };
               }
               _BGP_TreePtpHdr;

typedef union T_BGP_TreeCollectiveHdr
               {
               uint32_t word;
               struct {
                      unsigned Class      : 4; // Class number (a.k.a virtual tree number)
                      unsigned Ptp        : 1; // Must be 0 for collective header format.
                      unsigned Irq        : 1; // 1=request interrupt when received, 0=not
                      unsigned OpCode     : 3; // 000=ordinary routed packet, else ALU opcode
                      unsigned OpSize     : 7; // Operand size
                      unsigned Tag        :14; // Software tag
                      unsigned CsumMode   : 2; // Injection checksum mode
                      };
               }
               _BGP_TreeCollectiveHdr;

// Convenient union of the two hardware header types
typedef union T_BGP_TreeHwHdr
               {
               _BGP_TreePtpHdr         PtpHdr;
               _BGP_TreeCollectiveHdr  CtvHdr;
               }
               _BGP_TreeHwHdr;

// Collective Packet Payload
typedef union T_BGP_TreePayload
               {
               uint8_t       u8[  _BGP_TREE_PKT_MAX_BYTES                       ];
               uint16_t      u16[ _BGP_TREE_PKT_MAX_BYTES / sizeof(uint16_t)    ];
               uint32_t      u32[ _BGP_TREE_PKT_MAX_BYTES / sizeof(uint32_t)    ];
               uint32_t      u64[ _BGP_TREE_PKT_MAX_BYTES / sizeof(uint32_t)    ];
               float         f[   _BGP_TREE_PKT_MAX_BYTES / sizeof(float)       ];
               double        d[   _BGP_TREE_PKT_MAX_BYTES / sizeof(double)      ];
               _QuadWord_t   q[   _BGP_TREE_PKT_MAX_BYTES / sizeof(_QuadWord_t) ];
               }
               ALIGN_QUADWORD _BGP_TreePayload;


/*!\brief Tree Reception Register DCR
 *
 * _BGP_DCR_TR_REC_PRXF: Tree Reception Exception Flag Register
 * <p>
 * Reading this register will implicitly clear the flags.
 * </p>
 * <pre>
 *  Address: 0xC44
 *  Offset R/W   Bits  Function
 *  0x00   R/W    8    P0 address parity error
 *                9    P1 address parity error
 *               10    P0 address alignment error
 *               11    P1 address alignment error
 *               12    P0 bad (unrecognized) address error
 *               13    P1 bad (unrecognized) address error
 *               14    FIFO read collision error
 *               15    Uncorrectable SRAM ECC error
 *               26    VC0 payload FIFO under-run error
 *               27    VC1 payload FIFO under-run error
 *               28    VC0 header FIFO under-run error
 *               29    VC1 header FIFO under-run error
 *               30    VC0 payload FIFO above watermark
 *               31    VC1 payload FIFO above watermark
 * </pre>
*/
typedef struct {
   uint32_t dcr_TreeReceptionExceptionFlag;          
} DCR_TreeReceptionExceptoinFlag_t;


/*!\brief Tree Reception Register DCR
 *
 * _BGP_DCR_TR_REC_PRXEN: Tree Reception Exception Enable Register
 * <p>
 * A value of '1' in the indicated bit position of the register will enable the indicated condition
 * </p>
 * <pre>
 *  Address: 0xC45
 *  Offset R/W   Bits  Function
 *  0x00   R/W    8    P0 address parity error
 *                9    P1 address parity error
 *               10    P0 address alignment error
 *               11    P1 address alignment error
 *               12    P0 bad (unrecognized) address error
 *               13    P1 bad (unrecognized) address error
 *               14    FIFO read collision error
 *               15    Uncorrectable SRAM ECC error
 *               26    VC0 payload FIFO under-run error
 *               27    VC1 payload FIFO under-run error
 *               28    VC0 header FIFO under-run error
 *               29    VC1 header FIFO under-run error
 *               30    VC0 payload FIFO above watermark
 *               31    VC1 payload FIFO above watermark
 * </pre>
*/
typedef struct { 
   uint32_t dcr_TreeReceptionExceptionEnable;          
} DCR_TreeReceptionExceptionEnable_t;


/*!\brief Tree Injection Exception Flag Register
 *
 * _BGP_DCR_TR_INJ_PIXF: Tree Injection Exception Flag Register.  
 * <p>
 * Reading this register will implicitly clear the flags.  
 * </p>
 * <pre>
 *  Address: 0xC48
 *  Offset R/W   Bits  Function
 *  0x00    R     6    P0 address parity error
 *          R     7    P1 address parity error
 *          R     8    P0 address alignment error
 *          R     9    P1 address alignment error
 *          R     10   P0 bad (unrecognized) address error
 *          R     11   P1 bad (unrecognized) address error
 *          R     12   P0 data parity error
 *          R     13   P1 data parity error
 *          R     14   FIFO write collision error
 *          R     15   Uncorrectable SRAM ECC error
 *          R     25   VC0 payload FIFO overflow error
 *          R     26   VC1 payload FIFO overflow error
 *          R     27   VC0 header FIFO overflow error
 *          R     28   VC1 header FIFO overflow error
 *          R     29   VC0 payload FIFO at or below watermark
 *          R     30   VC1 payload FIFO at or below watermark
 *          W     31   Injection interface enable (if enabled in PIXEN)
 * </pre>
*/
typedef struct {
   uint32_t dcr_TreeInjectionExceptionFlag;          
} DCR_TreeInjectionExceptionFlag_t;


/*!\brief Tree Injection Exception Enable Register
 *
 * _BGP_DCR_TR_INJ_PIXEN: Tree Injection Exception Enable Register.
 * <p> 
 A value of '1' in the indicated bit position of the register will enable the indicated condition
 * </p>
 * <pre>
 *  Address: 0xC49
 *  Offset R/W   Bits  Function
 *  0x00   R/W    6    P0 address parity error
 *                7    P1 address parity error
 *                8    P0 address alignment error
 *                9    P1 address alignment error
 *                10   P0 bad (unrecognized) address error
 *                11   P1 bad (unrecognized) address error
 *                12   P0 data parity error
 *                13   P1 data parity error
 *                14   FIFO write collision error
 *                15   Uncorrectable SRAM ECC error
 *                25   VC0 payload FIFO overflow error
 *                26   VC1 payload FIFO overflow error
 *                27   VC0 header FIFO overflow error
 *                28   VC1 header FIFO overflow error
 *                29   VC0 payload FIFO at or below watermark
 *                30   VC1 payload FIFO at or below watermark
 *                31   Injection interface enable 
 * </pre>
*/
typedef struct {
   uint32_t dcr_TreeInjectionExceptionEnable;          
} DCR_TreeInjectionExceptionEnable_t;


/*!\brief Tree class route descriptor register DCR for classes 2 and 3
 *
 * _BGP_DCR_TR_CLASS_RDR1: Tree class route descriptor register for classes 2 and 3
 * <pre>
 *  Address: 0xC01
 *  Offset R/W   Bits  Function
 *  0x00   R/W    1     Class low,  source channel 2
 *                2     Class low,  source channel 1
 *                3     Class low,  source channel 0  
 *                5     Class low,  target channel 2  
 *                6     Class low,  target channel 1  
 *                7     Class low,  target channel 0  
 *               14     Class low,  source local client (injection)  
 *               15     Class low,  target local client (reception)  
 *               17     Class high, source channel 2  
 *               18     Class high, source channel 1  
 *               19     Class high, source channel 0
 *               21     Class high, target channel 2  
 *               22     Class high, target channel 1  
 *               23     Class high, target channel 0
 *               30     Class high, source local client (injection)  
 *               31     Class high, target local client (reception)
 * </pre>
*/
typedef struct {
   uint32_t dcr_TreeClassRoute;          
} DCR_TreeClassRoute_t;


#if defined(__BL__) || defined(__CNK__) || defined(__CNA__)
#include <bpcore/bgp_collective_inlines.h>
#endif

#endif // Not __ASSEMBLY__ and Not __BGP_HOST_COMPILED__ and Not __MAMBO__

__END_DECLS



#endif // Add nothing below this line
