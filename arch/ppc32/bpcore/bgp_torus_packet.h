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
#ifndef	_BGP_TORUS_PACKET_H_ // Prevent multiple inclusion
#define	_BGP_TORUS_PACKET_H_



#include <common/namespace.h>

__BEGIN_DECLS

#include <common/alignment.h>

// Constants for Torus Packet Header:
//
// Torus Packet Header Hint Bits (for use in structure bit-fields)
#define _BGP_TORUS_PKT_HINT_XP  (0x20)
#define _BGP_TORUS_PKT_HINT_XM  (0x10)
#define _BGP_TORUS_PKT_HINT_YP  (0x08)
#define _BGP_TORUS_PKT_HINT_YM  (0x04)
#define _BGP_TORUS_PKT_HINT_ZP  (0x02)
#define _BGP_TORUS_PKT_HINT_ZM  (0x01)

// Torus Packet Header Virtual Circuits
#define _BGP_TORUS_PKT_VC_D0 (0)
#define _BGP_TORUS_PKT_VC_D1 (1)
#define _BGP_TORUS_PKT_VC_BN (2)
#define _BGP_TORUS_PKT_VC_BP (3)

// Torus Packet Header Dynamic Routing
#define _BGP_TORUS_PKT_DETERMINSTIC (0)
#define _BGP_TORUS_PKT_DYNAMIC      (1)

// Torus Packet Header Routing
#define _BGP_TORUS_PKT_POINT2POINT  (0)
#define _BGP_TORUS_PKT_CLASS        (1)



// The Torus Hardware Packet Header
typedef struct _BGP_TorusHdrHW0_t    // first 32bit word of a Torus Hardware Header
{
    union {
        unsigned word;                      // access fields as 32-bit word
        struct {
             unsigned CSum_Skip     : 7;    // Number of shorts (2B) to skip in CheckSum.
             unsigned Sk            : 1;    // 0=use CSum_Skip, 1=Skip entire pkt.

             unsigned Hint          : 6;    // Hint Bits
             unsigned Dp            : 1;    // Deposit Bit for Class Routed MultiCast
             unsigned Pid0          : 1;    // Destination Fifo Group MSb

             unsigned Chunks        : 3;    // Size in Chunks of 32B (0 for 1 chunk, ... , 7 for 8 chunks)
             unsigned Pid1          : 1;    // Destination Fifo Group LSb
             unsigned Dm            : 1;    // 1=DMA Mode, 0=Fifo Mode
             unsigned Dynamic       : 1;    // 1=Dynamic Routing, 0=Deterministic Routing.
             unsigned VC            : 2;    // Virtual Channel (0=D0,1=D1,2=BN,3=BP)

             unsigned X             : 8;    // Destination X Physical Coordinate
        };
    };

} _BGP_TorusHdrHW0_t;


typedef struct _BGP_TorusHdrHW1_t           // second 32bit word of a Torus Hardware Header
{
    union {
        unsigned word;                      // access fields as 32-bit word
        struct {
             unsigned Y             : 8;    // Destination Y Physical Coordinate
             unsigned Z             : 8;    // Destination Z Physical Coordinate
             unsigned Resvd0        : 8;    // Reserved (pkt crc)
             unsigned Resvd1        : 8;    // Reserved (pkt crc)
        };
    };

} _BGP_TorusHdrHW1_t;



typedef union _BGP_TorusHdrHW2_t           // third 32bit word of a Torus Hardware Header (DMA Mode)
{
    unsigned word;
    unsigned Put_Offset;

} _BGP_TorusHdrHW2_t;



typedef struct _BGP_TorusHdrHW3_t           // fourth 32bit word of a Torus Hardware Header (DMA Mode)
{
    union {
        unsigned word;                      // access fields as 32-bit word
        struct {
             unsigned rDMA_Counter  : 8;
             unsigned Payload_Bytes : 8;
             unsigned Flags         : 8;    // Flags[6]=Pacing, Flags[7]=Remote-Get
             unsigned iDMA_Fifo_ID  : 8;
        };
    };

} _BGP_TorusHdrHW3_t;



// DMA Injection Descriptor
typedef struct _BGP_DMA_InjDescriptor_t
{
    union {
        unsigned word1;                     // access fields as 32-bit word
        struct {
             unsigned rsvd0          : 24;  // 3 bytes: unused
             unsigned rsvd1          :  6;  // bits 0-5: unused flags
             unsigned prefetch_only  :  1;  // bit 6: prefetch only on local memcopy
             unsigned local_memcopy  :  1;  // bit 7: local memory copy bit
        };
    };

    union {
        unsigned word2;                     // access fields as 32-bit word
        struct {
             unsigned rsvd2          : 24;  // 3 bytes: unused
             unsigned idma_counterId :  8;  // 1 byte: Injection Counter Id
        };
    };

    unsigned base_offset             : 32;  // 4 bytes: pointer to base address of message payload

    unsigned msg_length              : 32;  // 4 bytes: message length (in bytes)

    _BGP_TorusHdrHW0_t hwHdr0;              // Torus Hardware Header
    _BGP_TorusHdrHW1_t hwHdr1;

    _BGP_TorusHdrHW2_t hwHdr2;              // Torus DMA Header
    _BGP_TorusHdrHW3_t hwHdr3;

}  ALIGN_QUADWORD _BGP_DMA_InjDescriptor_t;


// Non-DMA Torus Packet Header
typedef struct _BGP_TorusPacketHeader_t
                 {
                 _BGP_TorusHdrHW0_t hwh0;
                 _BGP_TorusHdrHW1_t hwh1;
                 _BGP_TorusHdrHW2_t hwh2;
                 _BGP_TorusHdrHW3_t hwh3;
                 }
                 ALIGN_QUADWORD _BGP_TorusPacketHeader_t;


__END_DECLS



#endif // Add nothing below this line

