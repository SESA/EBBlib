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
 * \file bpcore/bgp_dcrmap.h
 */

#ifndef _BGP_DCRMAP_H_  // Prevent multiple inclusion.
#define _BGP_DCRMAP_H_




#define _BGP_DCR_BIC        (0x000)                     // 0x000-0x1ff: BIC (includes MCCU functionality)
#define _BGP_DCR_BIC_END    (_BGP_DCR_BIC + 0x1FF)      // 0x1ff: BIC (includes MCCU functionality)

#define _BGP_DCR_SERDES     (0x200)                     // 0x200-0x3ff: Serdes Config
#define _BGP_DCR_SERDES_END (_BGP_DCR_SERDES + 0x1FF)   // 0x3ff: Serdes Config End

#define _BGP_DCR_TEST       (0x400)                     // 0x400-0x47f: Test Interface
#define _BGP_DCR_TEST_END   (_BGP_DCR_TEST + 0x07F)     // 0x400-0x47f: Test Interface End

#define _BGP_DCR_L30        (0x500)                     // 0x500-0x53f: L3-Cache 0
#define _BGP_DCR_L30_END    (_BGP_DCR_L30 + 0x03F)      // 0x53f: L3-Cache 0 End

#define _BGP_DCR_L31        (0x540)                     // 0x540-0x57f: L3-Cache 1
#define _BGP_DCR_L31_END    (_BGP_DCR_L31 + 0x03F)      // 0x57f: L3-Cache 1 End

#define _BGP_DCR_XAUI       (0x580)                     // 0x580-0x5bf: XAUI config
#define _BGP_DCR_XAUI_END   (_BGP_DCR_XAUI + 0x03F)     // 0x5bf: XAUI config End

#define _BGP_DCR_SRAM       (0x610)                     // 0x610-0x61f: SRAM unit (Includes Lockbox functionality)
#define _BGP_DCR_SRAM_END   (_BGP_DCR_SRAM + 0x00F)     // 0x61f: SRAM unit (Includes Lockbox functionality)

#define _BGP_DCR_DEVBUS     (0x620)                     // 0x620-0x62f: DevBus Arbiter
#define _BGP_DCR_DEVBUS_END (_BGP_DCR_DEVBUS + 0x00F)   // 0x62f: DevBus Arbiter End

#define _BGP_DCR_NETBUS     (0x630)                     // 0x630-0x63f: NetBus Arbiter
#define _BGP_DCR_NETBUS_END (_BGP_DCR_NETBUS + 0x00F)   // 0x63f: NetBus Arbiter End

#define _BGP_DCR_DMAARB     (0x640)                     // 0x640-0x64f: DMA arbiter (former PLB slave)
#define _BGP_DCR_DMAARB_END (_BGP_DCR_DMAARB + 0x00F)   // 0x64f: DMA arbiter (former PLB slave) End

#define _BGP_DCR_DCRARB     (0x650)                     // 0x650-0x65f: DCR arbiter
#define _BGP_DCR_DCRARB_END (_BGP_DCR_DCRARB + 0x00F)   // 0x65f: DCR arbiter End

#define _BGP_DCR_GLOBINT     (0x660)                    // 0x660-0x66F: Global Interrupts
#define _BGP_DCR_GLOBINT_END (_BGP_DCR_GLOBINT + 0x00F) // 0x66F: Global Interrupts End

#define _BGP_DCR_CLOCKSTOP     (0x670)                      // 0x670-0x67F: Clock Stop
#define _BGP_DCR_CLOCKSTOP_END (_BGP_DCR_CLOCKSTOP + 0x00F) // 0x67F: Clock Stop End

#define _BGP_DCR_ENVMON      (0x680)                    // 0x670-0x67F: Environmental Monitor
#define _BGP_DCR_ENVMON_END  (_BGP_DCR_ENVMON + 0x00F)  // 0x67F: Env Mon End

#define _BGP_DCR_FPU        (0x700)                     // 0x700-0x77f: Hummer3 00/01/10/11
#define _BGP_DCR_FPU_END    (_BGP_DCR_FPU + 0x07F)      // 0x77f: Hummer3 00/01/10/11 End

#define _BGP_DCR_L2         (0x780)                     // 0x780-0x7ff: L2-Cache 00/01/10/11
#define _BGP_DCR_L2_END     (_BGP_DCR_L2 + 0x07F)       // 0x7ff: L2-Cache 00/01/10/11 End

#define _BGP_DCR_SNOOP      (0x800)                     // 0x800-0xbff: Snoop 00/01/10/11
#define _BGP_DCR_SNOOP0     (0x800)                     // 0x800-0x8ff: Snoop 00
#define _BGP_DCR_SNOOP1     (0x900)                     // 0x900-0x9ff: Snoop 01
#define _BGP_DCR_SNOOP2     (0xA00)                     // 0xa00-0xaff: Snoop 10
#define _BGP_DCR_SNOOP3     (0xB00)                     // 0xb00-0xbff: Snoop 11
#define _BGP_DCR_SNOOP_END  (_BGP_DCR_SNOOP + 0x3FF)    // 0xbff: Snoop 00/01/10/11 End

#define _BGP_DCR_TREE       (0xc00)                     // 0xc00-0xc7f: Tree
#define _BGP_DCR_TREE_END   (_BGP_DCR_TREE + 0x07F)     // 0xc7f: Tree End

#define _BGP_DCR_TORUS      (0xc80)                     // 0xc80-0xcff: Torus
#define _BGP_DCR_TORUS_END  (_BGP_DCR_TORUS + 0x07F)    // 0xcff: Torus End

#define _BGP_DCR_DMA        (0xd00)                     // 0xd00-0xdff: DMA
#define _BGP_DCR_DMA_END    (_BGP_DCR_DMA + 0x0FF)      // 0xdff: DMA End

#define _BGP_DCR_DDR0       (0xe00)                     // 0xe00-0xeff: DDR controller 0
#define _BGP_DCR_DDR0_END   (_BGP_DCR_DDR0 + 0x0FF)     // 0xeff: DDR controller 0 End

#define _BGP_DCR_DDR1       (0xf00)                     // 0xf00-0xfff: DDR controller 1
#define _BGP_DCR_DDR1_END   (_BGP_DCR_DDR1 + 0x0FF)     // 0xfff: DDR controller 1 End




#endif // defined(_BGP_DCRMAP_H_)
