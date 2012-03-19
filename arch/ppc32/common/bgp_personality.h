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
 * \file common/bgp_personality.h
 */

#ifndef	_BGP_PERSONALITY_H_ // Prevent multiple inclusion
#define	_BGP_PERSONALITY_H_



#include <common/namespace.h>

__BEGIN_DECLS

#include <common/bgp_chipversion.h>
#include <common/alignment.h>
#include <common/bgp_bitnumbers.h>
#include <bpcore/bgp_types.h>

//
// I/O Node Linux currently hard-codes the personality address.
//
#define _BGP_HARDCODED_PERSONALITY_SRAM_ADDRESS (0xFFFFF800)

#define _BGP_PERSONALITY_VERSION (0x0A)

#define _BGP_DEFAULT_FREQ (850)  // Match the current DD2 hardware

#define _BGP_PERS_Unused_DEFAULT 0

#define _BGP_PERS_PROCESSCONFIG_DIAGS      (0xFF000000) // Diagnostic Mode: All Cores Enabled and Privileged in Process 0
#define _BGP_PERS_PROCESSCONFIG_SMP        (0x0F000000) // All Cores Enabled User-Space in Process 0
#define _BGP_PERS_PROCESSCONFIG_VNM        (0x08040201) // 4 Single-Core Processes (a.k.a. Virtual Nodes)
#define _BGP_PERS_PROCESSCONFIG_2x2        (0x0C030000) // 2 Processes of 2 Cores each in same DP unit
#define _BGP_PERS_PROCESSCONFIG_DEFAULT    (_BGP_PERS_PROCESSCONFIG_DIAGS)
#define _BGP_PERS_PROCESSCONFIG_PRIV_MSK   (0xF0F0F0F0) // Mask to isolate privileged core flags


// Personality.Kernel_Config.RASPolicy
#define _BGP_PERS_RASPOLICY_VERBOSITY(x)   _B2( 1,x)  // Verbosity as shown below
#define _BGP_PERS_RASPOLICY_MINIMAL          _BGP_PERS_RASPOLICY_VERBOSITY(0) // Benchmarking Level of Capture and Reporting
#define _BGP_PERS_RASPOLICY_NORMAL           _BGP_PERS_RASPOLICY_VERBOSITY(1) // Normal Production Level of Capture and Reporting
#define _BGP_PERS_RASPOLICY_VERBOSE          _BGP_PERS_RASPOLICY_VERBOSITY(2) // Manufacturing Test and Diagnostics
#define _BGP_PERS_RASPOLICY_EXTREME          _BGP_PERS_RASPOLICY_VERBOSITY(3) // Report Every Event Immediately - Thresholds set to 1
#define _BGP_PERS_RASPOLICY_FATALEXIT      _BN( 2)   // Fatal is Fatal, so exit.

#define _BGP_PERS_RASPOLICY_DEFAULT        (_BGP_PERS_RASPOLICY_VERBOSE | _BGP_PERS_RASPOLICY_FATALEXIT)


#define _BGP_PERSONALITY_LEN_NFSDIR (32) // 32bytes

#define _BGP_PERSONALITY_LEN_SECKEY (32) // 32bytes

// Personality.NodeConfig Driver Enables and Configurations
#define _BGP_PERS_ENABLE_Simulation      _BN( 0)  // Running on VHDL Simulation
#define _BGP_PERS_ENABLE_LockBox         _BN( 1)
#define _BGP_PERS_ENABLE_BIC             _BN( 2)
#define _BGP_PERS_ENABLE_DDR             _BN( 3)  // DDR Controllers (not Fusion DDR model)
#define _BGP_PERS_ENABLE_LoopBack        _BN( 4)  // LoopBack: Internal TS/TR or SerDes Loopback
#define _BGP_PERS_ENABLE_GlobalInts      _BN( 5)
#define _BGP_PERS_ENABLE_Collective      _BN( 6)  // Enable Collective Network
#define _BGP_PERS_ENABLE_Torus           _BN( 7)
#define _BGP_PERS_ENABLE_TorusMeshX      _BN( 8)  // Torus is a Mesh in the X-dimension
#define _BGP_PERS_ENABLE_TorusMeshY      _BN( 9)  // Torus is a Mesh in the Y-dimension
#define _BGP_PERS_ENABLE_TorusMeshZ      _BN(10)  // Torus is a Mesh in the Z-dimension
#define _BGP_PERS_ENABLE_TreeA           _BN(11)  // Enable Collective Network A-link
#define _BGP_PERS_ENABLE_TreeB           _BN(12)  // Enable Collective Network B-link
#define _BGP_PERS_ENABLE_TreeC           _BN(13)  // Enable Collective Network C-link
#define _BGP_PERS_ENABLE_DMA             _BN(14)
#define _BGP_PERS_ENABLE_SerDes          _BN(15)
#define _BGP_PERS_ENABLE_UPC             _BN(16)
#define _BGP_PERS_ENABLE_EnvMon          _BN(17)
#define _BGP_PERS_ENABLE_Ethernet        _BN(18)
#define _BGP_PERS_ENABLE_JTagLoader      _BN(19)  // Converse with JTag Host to load kernel
#define _BGP_PERS_ENABLE_MailBoxReceive  _BGP_PERS_ENABLE_JTagLoader
#define _BGP_PERS_ENABLE_PowerSave       _BN(20)  // Turn off unused devices (Eth on CN, TS on ION)
#define _BGP_PERS_ENABLE_FPU             _BN(21)  // Enable Double-Hummers (not supported in EventSim)
#define _BGP_PERS_ENABLE_StandAlone      _BN(22)  // Disable "CIOD" interface, Requires Collective!
#define _BGP_PERS_ENABLE_TLBMisses       _BN(23)  // TLB Misses vs Wasting Memory (see bgp_AppSetup.c)
#define _BGP_PERS_ENABLE_Mambo           _BN(24)  // Running under Mambo? Used by Linux
#define _BGP_PERS_ENABLE_TreeBlast       _BN(25)  // Enable Tree "Blast" mode
#define _BGP_PERS_ENABLE_BlindStacks     _BN(26)  // For "XB" Tests, Lock 16K Stacks in Blind Device
#define _BGP_PERS_ENABLE_CNK_Malloc      _BN(27)  // Enable Malloc Support in CNK.
#define _BGP_PERS_ENABLE_Reproducibility _BN(28)  // Enable Cycle Reproducibility
#define _BGP_PERS_ENABLE_HighThroughput  _BN(29)  // Enable high throughput computing mode
#define _BGP_PERS_ENABLE_DiagnosticsMode _BN(30)  // Enable diagnostics mode

// Configure L1+L2 into BG/L Mode (s/w managed L1 coherence, write-back)
//  This overrides most L1, L2, and Snoop settings. Carefull!!
#define _BGP_PERS_ENABLE_BGLMODE      _BN(31)  // (not yet fully implemented)

// Default Setup for Simulation: Torus Meshes, DMA, SerDes, Ethernet, JTagLoader, PowerSave

#define _BGP_PERS_NODECONFIG_DEFAULT (_BGP_PERS_ENABLE_Simulation  |\
                                      _BGP_PERS_ENABLE_LockBox     |\
                                      _BGP_PERS_ENABLE_BIC         |\
                                      _BGP_PERS_ENABLE_DDR         |\
                                      _BGP_PERS_ENABLE_LoopBack    |\
                                      _BGP_PERS_ENABLE_GlobalInts  |\
                                      _BGP_PERS_ENABLE_Collective  |\
                                      _BGP_PERS_ENABLE_Torus       |\
                                      _BGP_PERS_ENABLE_UPC         |\
                                      _BGP_PERS_ENABLE_EnvMon      |\
                                      _BGP_PERS_ENABLE_FPU         |\
                                      _BGP_PERS_ENABLE_TLBMisses   |\
                                      _BGP_PERS_ENABLE_StandAlone)

// Default Setup for Hardware:
//     Supports Stand-Alone CNA Applications.
//     Bootloader-Extensions and XB's must turn-off JTagLoader
#define _BGP_PERS_NODECONFIG_DEFAULT_FOR_HARDWARE (_BGP_PERS_ENABLE_JTagLoader  |\
                                                   _BGP_PERS_ENABLE_LockBox     |\
                                                   _BGP_PERS_ENABLE_BIC         |\
                                                   _BGP_PERS_ENABLE_DDR         |\
                                                   _BGP_PERS_ENABLE_GlobalInts  |\
                                                   _BGP_PERS_ENABLE_Collective  |\
                                                   _BGP_PERS_ENABLE_SerDes      |\
                                                   _BGP_PERS_ENABLE_UPC         |\
                                                   _BGP_PERS_ENABLE_EnvMon      |\
                                                   _BGP_PERS_ENABLE_FPU         |\
                                                   _BGP_PERS_ENABLE_TLBMisses   |\
                                                   _BGP_PERS_ENABLE_StandAlone)



// these fields are defined by the control system depending on compute/io node
//                                                   _BGP_PERS_ENABLE_Torus       |
//                                                   _BGP_PERS_ENABLE_TorusMeshX  |
//                                                   _BGP_PERS_ENABLE_TorusMeshY  |
//                                                   _BGP_PERS_ENABLE_TorusMeshZ  |



// Personality.L1Config: Controls and Settings for L1 Cache
#define _BGP_PERS_L1CONFIG_L1I          _BN( 0)    // L1 Enabled for Instructions
#define _BGP_PERS_L1CONFIG_L1D          _BN( 1)    // L1 Enabled for Data
#define _BGP_PERS_L1CONFIG_L1SWOA       _BN( 2)    // L1 Store WithOut Allocate
#define _BGP_PERS_L1CONFIG_L1Recovery   _BN( 3)    // L1 Full Recovery Mode
#define _BGP_PERS_L1CONFIG_L1WriteThru  _BN( 4)    // L1 Write-Thru (not svc_host changeable (yet?))
#define _BGP_PERS_L1CONFIG_DO_L1ITrans  _BN( 5)    // Enable L1 Instructions Transient?
#define _BGP_PERS_L1CONFIG_DO_L1DTrans  _BN( 6)    // Enable L1 Data         Transient?
                                                   // unused 9bits: 7..15
#define _BGP_PERS_L1CONFIG_L1ITrans(x)  _B8(23,x)  // L1 Transient for Instructions in Groups of 16 Lines
#define _BGP_PERS_L1CONFIG_L1DTrans(x)  _B8(31,x)  // L1 Transient for Data         in Groups of 16 Lines

#define _BGP_PERS_L1CONFIG_DEFAULT (_BGP_PERS_L1CONFIG_L1I         |\
                                    _BGP_PERS_L1CONFIG_L1D         |\
                                    _BGP_PERS_L1CONFIG_L1SWOA      |\
				    _BGP_PERS_L1CONFIG_L1Recovery  |\
                                    _BGP_PERS_L1CONFIG_L1WriteThru)

typedef union T_BGP_Pers_L1Cfg
               {
               uint32_t l1cfg;
               struct {
                      unsigned l1i         :  1;
                      unsigned l1d         :  1;
                      unsigned l1swoa      :  1;
                      unsigned l1recovery  :  1;
                      unsigned l1writethru :  1;
                      unsigned do_l1itrans :  1;
                      unsigned do_l1dtrans :  1;
                      unsigned l1rsvd      :  9;
                      unsigned l1itrans    :  8;
                      unsigned l1dtrans    :  8;
                      };
               }
               _BGP_Pers_L1Cfg;

// Personality.L2Config: Controls and Settings for L2 and Snoop
#define _BGP_PERS_L2CONFIG_L2I                _BN( 0)  // L2 Instruction Caching Enabled
#define _BGP_PERS_L2CONFIG_L2D                _BN( 1)  // L2 Data        Caching Enabled
#define _BGP_PERS_L2CONFIG_L2PF               _BN( 2)  // L2 Automatic Prefetching Enabled
#define _BGP_PERS_L2CONFIG_L2PFO              _BN( 3)  // L2 Optimistic Prefetching Enabled
#define _BGP_PERS_L2CONFIG_L2PFA              _BN( 4)  // L2 Aggressive Prefetching Enabled (fewer deeper streams)
#define _BGP_PERS_L2CONFIG_L2PFS              _BN( 5)  // L2 Aggressive Many-Stream Prefetching Enabled (deeper only when available buffers)
#define _BGP_PERS_L2CONFIG_Snoop              _BN( 6)  // Just NULL Snoop Filter
#define _BGP_PERS_L2CONFIG_SnoopCache         _BN( 7)  // Snoop Caches
#define _BGP_PERS_L2CONFIG_SnoopStream        _BN( 8)  // Snoop Stream Registers (Disable for BG/P Rit 1.0 due to PPC450 errata)
#define _BGP_PERS_L2CONFIG_SnoopRange         _BN( 9)  // Snoop Range Filter when possible
#define _BGP_PERS_L2CONFIG_BUG824LUMPY        _BN(10)  // BPC_BUGS 824: Fix with Lumpy Performance
#define _BGP_PERS_L2CONFIG_BUG824SMOOTH       _BN(11)  // BPC_BUGS 824: Fix with Smooth Performance, but -12% Memory
#define _BGP_PERS_L2CONFIG_NONCOHERENT_STACKS _BN(12)  // Special for Snoop diagnostics. See bgp_vmm.c
                                              // additional bits may be used for Snoop setting tweaks

// Default L2 Configuration:
//   L2 Enabled with Multi-Stream Aggressive Prefetching
//   Snoop Enabled with all filters except Range
#define _BGP_PERS_L2CONFIG_DEFAULT   (_BGP_PERS_L2CONFIG_L2I        |\
                                      _BGP_PERS_L2CONFIG_L2D        |\
                                      _BGP_PERS_L2CONFIG_L2PF       |\
                                      _BGP_PERS_L2CONFIG_L2PFO      |\
                                      _BGP_PERS_L2CONFIG_L2PFS      |\
                                      _BGP_PERS_L2CONFIG_Snoop      |\
                                      _BGP_PERS_L2CONFIG_SnoopCache |\
                                      _BGP_PERS_L2CONFIG_SnoopStream)

// Personality.L3Config: Controls and Settings for L3
//   Note: Most bits match _BGP_L3x_CTRL DCRs.
//         See arch/include/bpcore/bgl_l3_dcr.h
#define _BGP_PERS_L3CONFIG_L3I        _BN( 0)    // L3 Enabled for Instructions
#define _BGP_PERS_L3CONFIG_L3D        _BN( 1)    // L3 Enabled for Data
#define _BGP_PERS_L3CONFIG_L3PFI      _BN( 2)    // Inhibit L3 Prefetch from DDR
#define _BGP_PERS_L3CONFIG_DO_Scratch _BN( 3)    // Set up Scratch?
#define _BGP_PERS_L3CONFIG_DO_PFD0    _BN( 4)    // Adjust PFD0?
#define _BGP_PERS_L3CONFIG_DO_PFD1    _BN( 5)    // Adjust PFD1?
#define _BGP_PERS_L3CONFIG_DO_PFDMA   _BN( 6)    // Adjust PFDMA?
#define _BGP_PERS_L3CONFIG_DO_PFQD    _BN( 7)    // Adjust PFQD?
                                      // 8..15 unused/available
#define _BGP_PERS_L3CONFIG_Scratch(x) _B4(19,x)  // Scratch 8ths: 0..8
#define _BGP_PERS_L3CONFIG_PFD0(x)    _B3(22,x)  // Prefetch Depth for DP0
#define _BGP_PERS_L3CONFIG_PFD1(x)    _B3(25,x)  // Prefetch Depth for DP1
#define _BGP_PERS_L3CONFIG_PFDMA(x)   _B3(28,x)  // Prefetch Depth for DMA
#define _BGP_PERS_L3CONFIG_PFQD(x)    _B3(31,x)  // Prefetch Queue Depth

// General L3 Configuration
typedef union T_BGP_Pers_L3Cfg
               {
               uint32_t l3cfg;
               struct {
                      unsigned l3i        :  1;
                      unsigned l3d        :  1;
                      unsigned l3pfi      :  1;
                      unsigned do_scratch :  1;
                      unsigned do_pfd0    :  1;
                      unsigned do_pfd1    :  1;
                      unsigned do_pfdma   :  1;
                      unsigned do_pfqd    :  1;
                      unsigned rsvd       :  8;
                      unsigned scratch    :  4;
                      unsigned pfd0       :  3;
                      unsigned pfd1       :  3;
                      unsigned pfdma      :  3;
                      unsigned pfqd       :  3;
                      };
               }
               _BGP_Pers_L3Cfg;

// Default L3 Configuration:
//   L3 Enabled for Instructions and Data
//   No Prefetch Depth overrides, No Scratch, No Scrambling.
#define _BGP_PERS_L3CONFIG_DEFAULT    (_BGP_PERS_L3CONFIG_L3I |\
                                       _BGP_PERS_L3CONFIG_L3D |\
				       _BGP_PERS_L3CONFIG_DO_PFDMA |\
                                       _BGP_PERS_L3CONFIG_PFDMA(4))


// L3 Cache and Bank Selection, and prefetching tweaks (Recommended for Power-Users)
#define _BGP_PERS_L3SELECT_DO_CacheSel _BN( 0)   // Adjust Cache Select setting?
#define _BGP_PERS_L3SELECT_DO_BankSel  _BN( 1)   // Adjust Bank  Select setting?
#define _BGP_PERS_L3SELECT_Scramble    _BN( 2)   // L3 Scramble
#define _BGP_PERS_L3SELECT_PFby2       _BN( 3)   // Prefetch by 2 if set, else by 1 (default) if clear.
#define _BGP_PERS_L3SELECT_CacheSel(x) _B5( 8,x) // PhysAddr Bit for L3 Selection (0..26)
#define _BGP_PERS_L3SELECT_BankSel(x)  _B5(13,x) // PhysAddr Bit for L3 Bank Selection (0..26) Must be > CacheSel.

typedef union T_BGP_Pers_L3Select
               {
               uint32_t l3select;
               struct {
                      unsigned do_CacheSel :  1;
                      unsigned do_BankSel  :  1;
                      unsigned l3Scramble  :  1;
                      unsigned l3_PF_by2   :  1; // default is PreFetch by 1.
                      unsigned CacheSel    :  5; // Physical Address Bit for L3 Selection (0..26)
                      unsigned BankSel     :  5; // 0..26 Must be strictly greater than CacheSel.
                      unsigned rsvd        : 18;
                      };
               }
               _BGP_Pers_L3Select;

// Default L3 Selection Configuration: Disable overrides, but set h/w default values.
#define _BGP_PERS_L3SELECT_DEFAULT  (_BGP_PERS_L3SELECT_CacheSel(21) |\
                                     _BGP_PERS_L3SELECT_BankSel(26))

// Tracing Masks and default trace configuration
//   See also arch/include/cnk/Trace.h
#define _BGP_TRACE_CONFIG    _BN( 0)   // Display Encoded personality config on startup
#define _BGP_TRACE_ENTRY     _BN( 1)   // Function enter and exit
#define _BGP_TRACE_INTS      _BN( 2)   // Standard Interrupt Dispatch
#define _BGP_TRACE_CINTS     _BN( 3)   // Critical Interrupt Dispatch
#define _BGP_TRACE_MCHK      _BN( 4)   // Machine Check Dispatch
#define _BGP_TRACE_SYSCALL   _BN( 5)   // System Calls
#define _BGP_TRACE_VMM       _BN( 6)   // Virtual Memory Manager
#define _BGP_TRACE_DEBUG     _BN( 7)   // Debug Events (app crashes etc)
#define _BGP_TRACE_TORUS     _BN( 8)   // Torus Init
#define _BGP_TRACE_TREE      _BN( 9)   // Tree  Init
#define _BGP_TRACE_GLOBINT   _BN(10)   // Global Interrupts
#define _BGP_TRACE_DMA       _BN(11)   // DMA Setup
#define _BGP_TRACE_SERDES    _BN(12)   // SerDes Init
#define _BGP_TRACE_TESTINT   _BN(13)   // Test Interface, ECID, Config
#define _BGP_TRACE_ETHTX     _BN(14)   // Ethernet Transmit
#define _BGP_TRACE_ETHRX     _BN(15)   // Ethernet Receive
#define _BGP_TRACE_POWER     _BN(16)   // Power Control
#define _BGP_TRACE_PROCESS   _BN(17)   // Process/Thread Mapping
#define _BGP_TRACE_EXIT_SUM  _BN(18)   // Report Per-Core Interrupt and Error Summary on exit()
#define _BGP_TRACE_SCHED     _BN(19)   // Report Scheduler Information
#define _BGP_TRACE_RAS       _BN(20)   // Report RAS Events (in addition to sending to Host)
#define _BGP_TRACE_ECID      _BN(21)   // Report UCI and ECID on boot
#define _BGP_TRACE_FUTEX     _BN(22)   // Trace Futex operations
#define _BGP_TRACE_MemAlloc  _BN(23)   // Trace MMAP and Shared Memory operations
#define _BGP_TRACE_CONTROL   _BN(24)   // Trace control messages exchanged with I/O node
#define _BGP_TRACE_MSGS      _BN(25)   // Trace messages and packets sent on virtual channel 0
#define _BGP_TRACE_DEBUGGER  _BN(26)   // Trace debugger messages exchanged with I/O node
#define _BGP_TRACE_WARNINGS  _BN(30)   // Trace Warnings
#define _BGP_TRACE_VERBOSE   _BN(31)   // Verbose Tracing Modifier

// Enable tracking of Regression Suite coverage and report UCI+ECID on boot
#define _BGP_PERS_TRACE_DEFAULT 0
//(_BGP_TRACE_CONFIG | _BGP_TRACE_ECID)


typedef struct _BGP_Personality_Kernel_t
                {
                uint32_t  UniversalComponentIdentifier; // see include/common/bgp_ras.h

                uint32_t  FreqMHz;                      // Clock_X1 Frequency in MegaHertz (eg 1000)

                uint32_t  RASPolicy;                    // Verbosity level, and other RAS Reporting Controls

                // Process Config:
                //   Each byte represents a process (1 to 4 processes supported)
                //     No core can be assigned to more than 1 process.
                //     Cores assigned to no process are disabled.
                //     Cores with in a process share the same address space.
                //     Separate processes have distinct address spaces.
                //   Within each process (0 to 4 cores assigned to a process):
                //     Lower nibble is bitmask of which core belongs to that process.
                //     Upper nibble is bitmask whether that thread is privileged or user.
                //     Processes with zero cores do not exist.
                //   E.g., for Diagnostics, we sometimes use 0xFF000000, which means
                //     that all 4 cores run privileged in process 0.
                uint32_t  ProcessConfig;

                uint32_t  TraceConfig;        // Kernel Tracing Enables
                uint32_t  NodeConfig;         // Kernel Driver Enables
                uint32_t  L1Config;           // L1 Config and setup controls
                uint32_t  L2Config;           // L2 and Snoop Config and setup controls
                uint32_t  L3Config;           // L3 Config and setup controls
                uint32_t  L3Select;           // L3 Cache and Bank Selection controls

                uint32_t  SharedMemMB;        // Memory to Reserve for Sharing among Processes

                uint32_t  ClockStop0;        // Upper 11Bits of ClockStop, enabled if Non-zero
                uint32_t  ClockStop1;        // Lower 32Bits of ClockStop, enabled if Non-zero
                }
                _BGP_Personality_Kernel_t;


// Defaults for DDR Config
#define _BGP_PERS_DDR_PBX0_DEFAULT             (0x411D1512)    // PBX DCRs setting (in IBM bit numbering)
#define _BGP_PERS_DDR_PBX1_DEFAULT             (0x40000000)    // PBX DCRs setting (in IBM bit numbering)
#define _BGP_PERS_DDR_MemConfig0_DEFAULT       (0x81fc4080)    // MemConfig
#define _BGP_PERS_DDR_MemConfig1_DEFAULT       (0x0C0ff800)    // MemConfig
#define _BGP_PERS_DDR_ParmCtl0_DEFAULT         (0x3216c008)    // Parm Control
#define _BGP_PERS_DDR_ParmCtl1_DEFAULT         (0x4168c323)    // Parm Control
#define _BGP_PERS_DDR_MiscCtl0_DEFAULT         (0)    // Misc. Control
#define _BGP_PERS_DDR_MiscCtl1_DEFAULT         (0)    // Misc. Control
#define _BGP_PERS_DDR_CmdBufMode0_DEFAULT      (0x00400fdf)    // Command Buffer Mode
#define _BGP_PERS_DDR_CmdBufMode1_DEFAULT      (0xffc80600)    // Command Buffer Mode
#define _BGP_PERS_DDR_RefrInterval0_DEFAULT    (0xD1000002)    // Refresh Interval
#define _BGP_PERS_DDR_RefrInterval1_DEFAULT    (0x04000000)    // Refresh Interval
#define _BGP_PERS_DDR_ODTCtl0_DEFAULT          (0)    // ODT Control
#define _BGP_PERS_DDR_ODTCtl1_DEFAULT          (0)    // ODT Control
#define _BGP_PERS_DDR_TimingTweaks_DEFAULT     (0)    // DRAM timing tweaks to use
#define _BGP_PERS_DDR_DataStrobeCalib1_DEFAULT (0xa514c805)    // Data Strobe Calibration
#define _BGP_PERS_DDR_DQSCtl_DEFAULT           (0x00000168)    // DQS Control
#define _BGP_PERS_DDR_Throttle_DEFAULT         (0)    // DDR Throttle

#define _BGP_PERS_DDR_CAS_DEFAULT              (4)    // CAS Latency (3, 4, or 5)
#define _BGP_PERS_DDR_DDRSizeMB_DEFAULT        (2048) // Total DDR size in MegaBytes (512MB - 16384MB).
#define _BGP_PERS_DDR_Chips_DEFAULT            (0x01) // Type of DDR chips: 512GBx8

#define _BGP_PERS_DDRFLAGS_ENABLE_Scrub        _BN(0) // Enable DDR Slow Scrub when 1

// DDRFLAGS default: Enable Slow Scrub.
#define _BGP_PERS_DDRFLAGS_DEFAULT             (_BGP_PERS_DDRFLAGS_ENABLE_Scrub)

#define _BGP_PERS_SRBS0_DEFAULT                (0xFFFFFFFF)
#define _BGP_PERS_SRBS1_DEFAULT                (0xFFFFFFFF)

typedef struct _BGP_Personality_DDR_t
                {
                uint32_t  DDRFlags;         // Misc. Flags and Settings
                uint32_t  SRBS0;            // Controller 0 SRBS/CK Settings
                uint32_t  SRBS1;            // Controller 1 SRBS/CK Settings
                uint32_t  PBX0;             // PBX DCRs setting (in IBM bit numbering)
                uint32_t  PBX1;             // PBX DCRs setting (in IBM bit numbering)
                uint32_t  MemConfig0;       // MemConfig
                uint32_t  MemConfig1;       // MemConfig
                uint32_t  ParmCtl0;         // Parm Control
                uint32_t  ParmCtl1;         // Parm Control
                uint32_t  MiscCtl0;         // Misc. Control
                uint32_t  MiscCtl1;         // Misc. Control
                uint32_t  CmdBufMode0;      // Command Buffer Mode
                uint32_t  CmdBufMode1;      // Command Buffer Mode
                uint32_t  RefrInterval0;    // Refresh Interval
                uint32_t  RefrInterval1;    // Refresh Interval
                uint32_t  ODTCtl0;          // ODT Control
                uint32_t  ODTCtl1;          // ODT Control
                uint8_t   TimingTweaks;     // DRAM timing tweak type (Size specific)
                uint8_t   Unused0;
                uint8_t   Unused1;
                uint8_t   Unused2;
                uint32_t  DataStrobeCalib1; // Data Strobe Calibration
                uint32_t  DQSCtl;           // DQS Control
                uint32_t  Throttle;         // DDR Throttle
                uint16_t  DDRSizeMB;        // Total DDR size in MegaBytes (512MB - 16384MB).
                uint8_t   Chips;            // Type of DDR chips
                uint8_t   CAS;              // CAS Latency (3, 4, or 5)
                }
                _BGP_Personality_DDR_t;


typedef struct _BGP_Personality_Networks_t
                {
                uint32_t  BlockID;         // a.k.a. PartitionID

                uint8_t   Xnodes,
                          Ynodes,
                          Znodes,
                          Xcoord,
                          Ycoord,
                          Zcoord;

                // PSet Support
                uint16_t  PSetNum;
                uint32_t  PSetSize;
                uint32_t  RankInPSet;

                uint32_t  IOnodes;
                uint32_t  Rank;               // Rank in Block (or Partition)
                uint32_t  IOnodeRank;         // Rank (and therefore P2P Addr) of my I/O Node
                uint16_t  TreeRoutes[ 16 ];
                }
                _BGP_Personality_Networks_t;


typedef struct _BGP_IP_Addr_t
                {
                // IPv6 Addresses are 16 bytes, where the
                //  lower 4 (indices 12-15) can be used for IPv4 address.
                uint8_t octet[ 16 ];
                }
                _BGP_IP_Addr_t;


typedef struct _BGP_Personality_Ethernet_t
                {
                uint16_t       MTU;            // Initial emac MTU size
                uint8_t        EmacID[6];      // MAC address for emac
                _BGP_IP_Addr_t IPAddress;      // IPv6/IPv4 address of this node
                _BGP_IP_Addr_t IPNetmask;      // IPv6/IPv4 netmask
                _BGP_IP_Addr_t IPBroadcast;    // IPv6/IPv4 broadcast address
                _BGP_IP_Addr_t IPGateway;      // IPv6/IPv4 initial gateway (zero if none)
                _BGP_IP_Addr_t NFSServer;      // IPv6/IPv4 NFS system software server address
                _BGP_IP_Addr_t serviceNode;    // IPv6/IPv4 address of service node

                // NFS mount info
                char      NFSExportDir[_BGP_PERSONALITY_LEN_NFSDIR];
                char      NFSMountDir[ _BGP_PERSONALITY_LEN_NFSDIR];

                // Security Key for Service Node authentication
                uint8_t   SecurityKey[ _BGP_PERSONALITY_LEN_SECKEY ];
                }
                _BGP_Personality_Ethernet_t;


#define BGP_PERS_BLKCFG_IPOverCollective        _BN(31)
#define BGP_PERS_BLKCFG_IPOverTorus             _BN(30)
#define BGP_PERS_BLKCFG_IPOverCollectiveVC      _BN(29)
#define BGP_PERS_BLKCFG_CIOModeSel(x)           _B2(28,x)
#define BGP_PERS_BLKCFG_bgsysFSSel(x)           _B3(26,x)
#define BGP_PERS_BLKCFG_CIOMode_Full            0
#define BGP_PERS_BLKCFG_CIOMode_MuxOnly         1
#define BGP_PERS_BLKCFG_CIOMode_None            2
#define BGP_PERS_BLKCFG_bgsys_NFSv3             0
#define BGP_PERS_BLKCFG_bgsys_NFSv4             1
#define BGP_PERS_BLKCFG_DEFAULT (BGP_PERS_BLKCFG_CIOModeSel(BGP_PERS_BLKCFG_CIOMode_Full) | \
                                 BGP_PERS_BLKCFG_bgsysFSSel(BGP_PERS_BLKCFG_bgsys_NFSv3))


typedef struct T_BGP_Personality_t
                {
                uint16_t  CRC;
                uint8_t   Version;
                uint8_t   PersonalitySizeWords;

                _BGP_Personality_Kernel_t   Kernel_Config;

                _BGP_Personality_DDR_t      DDR_Config;

                _BGP_Personality_Networks_t Network_Config;

                _BGP_Personality_Ethernet_t Ethernet_Config;

		uint8_t Block_Config;
                uint8_t padd[7]; // Pad size to multiple of 16 bytes (== width of DEVBUS_DATA tdr)
                                  // to simplify jtag operations. See issue #140.
                }
                _BGP_Personality_t;

#define Network_Config_treeInfo0 DDR_Config.ODTCtl0
#define Network_Config_treeInfo1 DDR_Config.ODTCtl1
#define Network_Config_treeInfo2 DDR_Config.CmdBufMode0

/* _BGP_PersonalityTreeInfo provides information about one of the tree
 * ports (A,B or C) on this node.  It is a 32-bit value.
 * See accessor methods below which interpret these fields with this layout:
 *
 *  .-.-.--.--.--.------------------------.
 *  |V|R|LT|CW|DP|   destP2Paddr          |
 *  `-'-'--'--'--'------------------------'
 *   1 1 2  2  2  24	<- bits in field
 *
 * V	Valid bit.  Use is deprecated.  Was used for forward compatibility
 * R	Wire is redundant
 * LT	Link type (2 bit).  0->no wire, 1->compute node, 2->I/O node, 3->reserved
 * CW   CommWorld wire interpret (2 bit): 0->unused wire, 1->child, 2->parent
 * DP	Destination Port on other end of wire (2 bit) 0,1,2 -> A,B,C
 * destP2Paddr (24 bit) Tree address of node on other end of the wire
 */

#define _BGP_PERS_TREEINFO_VALID             0x80000000
#define _BGP_PERS_TREEINFO_REDUNDANT         0x40000000
#define _BGP_PERS_TREEINFO_LINKTYPE_MASK     0x30000000
#define _BGP_PERS_TREEINFO_LINKTYPE_SHIFT    28
#define _BGP_PERS_TREEINFO_COMMWORLD_MASK    0x0c000000
#define _BGP_PERS_TREEINFO_COMMWORLD_SHIFT   26
#define _BGP_PERS_TREEINFO_DESTPORT_MASK     0x03000000
#define _BGP_PERS_TREEINFO_DESTPORT_SHIFT    24
#define _BGP_PERS_TREEINFO_DESTP2P_MASK      0x00ffffff

#define _BGP_PERS_TREEINFO_LINKTYPE_NOWIRE       0
#define _BGP_PERS_TREEINFO_LINKTYPE_COMPUTE      1
#define _BGP_PERS_TREEINFO_LINKTYPE_IO           2

#define _BGP_PERS_TREEINFO_COMMWORLD_UNUSED      0 /* unused wire */
#define _BGP_PERS_TREEINFO_COMMWORLD_CHILD       1
#define _BGP_PERS_TREEINFO_COMMWORLD_PARENT      2

#define _BGP_PERS_TREE_PORT_0    0
#define _BGP_PERS_TREE_PORT_1    1
#define _BGP_PERS_TREE_PORT_2    2

/* This struct is the layout on big endian architectures (ppc) */
typedef struct {
    unsigned valid:1;  /* 1 -> this info is valid */
    unsigned redundant:1; /* 1 -> redundant wire */
    unsigned linkType:2; /* 0 -> no wire, 1 -> compute node, 2 -> I/O */
    unsigned commWorld:2; /* 1 -> child port, 2 -> parent port on comm_world tree */
    unsigned destPort:2; /* dest port 0,1,2 -> A,B,C */
    unsigned destP2Paddr:24; /* destination tree addr on this port */
} _BGP_PersonalityTreeInfo_t;



// Define a static initializer for default configuration. (DEFAULTS FOR SIMULATION)
//  This is used in bootloader:bgp_Personality.c and svc_host:svc_main.c
#define _BGP_PERSONALITY_DEFAULT_STATIC_INITIALIZER { \
           0,                                              /* CRC                  */ \
           _BGP_PERSONALITY_VERSION,                       /* Version              */ \
           (sizeof(_BGP_Personality_t)/sizeof(uint32_t)),  /* PersonalitySizeWords */ \
           {  /* _BGP_Personality_Kernel_t: */ \
              0,                                   /* MachineLocation        */ \
              _BGP_DEFAULT_FREQ,                   /* FreqMHz       */ \
              _BGP_PERS_RASPOLICY_DEFAULT,         /* RASPolicy     */ \
              _BGP_PERS_PROCESSCONFIG_DEFAULT,     /* ProcessConfig */ \
              _BGP_PERS_TRACE_DEFAULT,             /* TraceConfig   */ \
              _BGP_PERS_NODECONFIG_DEFAULT,        /* NodeConfig    */ \
              _BGP_PERS_L1CONFIG_DEFAULT,          /* L1Config      */ \
              _BGP_PERS_L2CONFIG_DEFAULT,          /* L2Config      */ \
              _BGP_PERS_L3CONFIG_DEFAULT,          /* L3Config      */ \
              _BGP_PERS_L3SELECT_DEFAULT,          /* L3Select      */ \
              0,                                   /* SharedMemMB   */ \
              0,                                   /* ClockStop0    */ \
              0                                    /* ClockStop1    */ \
              }, \
           {  /* _BGP_Personality_DDR_t: */ \
              _BGP_PERS_DDRFLAGS_DEFAULT,             /* DDRFlags         */ \
              _BGP_PERS_SRBS0_DEFAULT,                /* SRBS0            */ \
              _BGP_PERS_SRBS1_DEFAULT,                /* SRBS1            */ \
              _BGP_PERS_DDR_PBX0_DEFAULT,             /* PBX0             */ \
              _BGP_PERS_DDR_PBX1_DEFAULT,             /* PBX1             */ \
              _BGP_PERS_DDR_MemConfig0_DEFAULT,       /* MemConfig0       */ \
              _BGP_PERS_DDR_MemConfig1_DEFAULT,       /* MemConfig1       */ \
              _BGP_PERS_DDR_ParmCtl0_DEFAULT,         /* ParmCtl0         */ \
              _BGP_PERS_DDR_ParmCtl1_DEFAULT,         /* ParmCtl1         */ \
              _BGP_PERS_DDR_MiscCtl0_DEFAULT,         /* MiscCtl0         */ \
              _BGP_PERS_DDR_MiscCtl1_DEFAULT,         /* MiscCtl1         */ \
              _BGP_PERS_DDR_CmdBufMode0_DEFAULT,      /* CmdBufMode0      */ \
              _BGP_PERS_DDR_CmdBufMode1_DEFAULT,      /* CmdBufMode1      */ \
              _BGP_PERS_DDR_RefrInterval0_DEFAULT,    /* RefrInterval0    */ \
              _BGP_PERS_DDR_RefrInterval1_DEFAULT,    /* RefrInterval1    */ \
              _BGP_PERS_DDR_ODTCtl0_DEFAULT,          /* ODTCtl0          */ \
              _BGP_PERS_DDR_ODTCtl1_DEFAULT,          /* ODTCtl1          */ \
              _BGP_PERS_DDR_TimingTweaks_DEFAULT,     /* TimingTweaks     */ \
              _BGP_PERS_Unused_DEFAULT,               /* Unused0          */ \
              _BGP_PERS_Unused_DEFAULT,               /* Unused1          */ \
              _BGP_PERS_Unused_DEFAULT,               /* Unused2          */ \
              _BGP_PERS_DDR_DataStrobeCalib1_DEFAULT, /* DataStrobeCalib1 */ \
              _BGP_PERS_DDR_DQSCtl_DEFAULT,           /* DQSCtl           */ \
              _BGP_PERS_DDR_Throttle_DEFAULT,         /* Throttle         */ \
              _BGP_PERS_DDR_DDRSizeMB_DEFAULT,        /* DDRSizeMB        */ \
              _BGP_PERS_DDR_Chips_DEFAULT,            /* Chips            */ \
              _BGP_PERS_DDR_CAS_DEFAULT               /* CAS              */ \
              }, \
           {  /* _BGP_Personality_Networks_t: */ \
              0,                                   /* BlockID                */ \
              1, 1, 1,                             /* Xnodes, Ynodes, Znodes */ \
              0, 0, 0,                             /* Xcoord, Ycoord, Zcoord */ \
              0,                                   /* PSetNum                */ \
              0,                                   /* PSetSize               */ \
              0,                                   /* RankInPSet             */ \
              0,                                   /* IOnodes                */ \
              0,                                   /* Rank                   */ \
              0,                                   /* IOnodeRank             */ \
              { 0, }                               /* TreeRoutes[ 16 ]       */ \
              }, \
           {  /* _BGP_Personality_Ethernet_t: */ \
              1536,                                /* mtu              */ \
              { 0, },                              /* EmacID[6]        */ \
              { { 0x00,0x00,0x00,0x00,             /* IPAddress        */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* IPNetmask        */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0xFF,0xFF,0xFF,0x70  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* IPBroadcast      */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* IPGateway        */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* NFSServer        */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* serviceNode      */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              "",                                  /* NFSExportDir[32] */ \
              "",                                  /* NFSMountDir[32]  */ \
              { 0x00, }                            /* SecurityKey[32]  */ \
              }, \
	     0,                                    /* Block_Config */ \
           { 0, }                                  /* padd[7]          */ \
           }


// Define a static initializer for default configuration. (DEFAULTS FOR HARDWARE)
//  This is used in bootloader:bgp_Personality.c and svc_host:svc_main.c
#define _BGP_PERSONALITY_DEFAULT_STATIC_INITIALIZER_FOR_HARDWARE { \
           0,                                             /* CRC                  */ \
           _BGP_PERSONALITY_VERSION,                      /* Version              */ \
           (sizeof(_BGP_Personality_t)/sizeof(uint32_t)), /* PersonalitySizeWords */ \
           {  /* _BGP_Personality_Kernel_t: */ \
              0,                                          /* MachineLocation      */ \
              _BGP_DEFAULT_FREQ,                          /* FreqMHz       */ \
              _BGP_PERS_RASPOLICY_DEFAULT,                /* RASPolicy     */ \
              _BGP_PERS_PROCESSCONFIG_SMP,                /* ProcessConfig */ \
              _BGP_PERS_TRACE_DEFAULT,                    /* TraceConfig   */ \
              _BGP_PERS_NODECONFIG_DEFAULT_FOR_HARDWARE,  /* NodeConfig    */ \
              _BGP_PERS_L1CONFIG_DEFAULT,                 /* L1Config      */ \
              _BGP_PERS_L2CONFIG_DEFAULT,                 /* L2Config      */ \
              _BGP_PERS_L3CONFIG_DEFAULT,                 /* L3Config      */ \
              _BGP_PERS_L3SELECT_DEFAULT,                 /* L3Select      */ \
              0,                                          /* SharedMemMB   */ \
              0,                                          /* ClockStop0    */ \
              0                                           /* ClockStop1    */ \
              }, \
           {  /* _BGP_Personality_DDR_t: */ \
              _BGP_PERS_DDRFLAGS_DEFAULT,             /* DDRFlags         */ \
              _BGP_PERS_SRBS0_DEFAULT,                /* SRBS0            */ \
              _BGP_PERS_SRBS1_DEFAULT,                /* SRBS1            */ \
              _BGP_PERS_DDR_PBX0_DEFAULT,             /* PBX0             */ \
              _BGP_PERS_DDR_PBX1_DEFAULT,             /* PBX1             */ \
              _BGP_PERS_DDR_MemConfig0_DEFAULT,       /* MemConfig0       */ \
              _BGP_PERS_DDR_MemConfig1_DEFAULT,       /* MemConfig1       */ \
              _BGP_PERS_DDR_ParmCtl0_DEFAULT,         /* ParmCtl0         */ \
              _BGP_PERS_DDR_ParmCtl1_DEFAULT,         /* ParmCtl1         */ \
              _BGP_PERS_DDR_MiscCtl0_DEFAULT,         /* MiscCtl0         */ \
              _BGP_PERS_DDR_MiscCtl1_DEFAULT,         /* MiscCtl1         */ \
              _BGP_PERS_DDR_CmdBufMode0_DEFAULT,      /* CmdBufMode0      */ \
              _BGP_PERS_DDR_CmdBufMode1_DEFAULT,      /* CmdBufMode1      */ \
              _BGP_PERS_DDR_RefrInterval0_DEFAULT,    /* RefrInterval0    */ \
              _BGP_PERS_DDR_RefrInterval1_DEFAULT,    /* RefrInterval1    */ \
              _BGP_PERS_DDR_ODTCtl0_DEFAULT,          /* ODTCtl0          */ \
              _BGP_PERS_DDR_ODTCtl1_DEFAULT,          /* ODTCtl1          */ \
              _BGP_PERS_DDR_TimingTweaks_DEFAULT,     /* TimingTweaks     */ \
              _BGP_PERS_Unused_DEFAULT,               /* Unused0          */ \
              _BGP_PERS_Unused_DEFAULT,               /* Unused1          */ \
              _BGP_PERS_Unused_DEFAULT,               /* Unused2          */ \
              _BGP_PERS_DDR_DataStrobeCalib1_DEFAULT, /* DataStrobeCalib1 */ \
              _BGP_PERS_DDR_DQSCtl_DEFAULT,           /* DQSCtl           */ \
              _BGP_PERS_DDR_Throttle_DEFAULT,         /* Throttle         */ \
              _BGP_PERS_DDR_DDRSizeMB_DEFAULT,        /* DDRSizeMB        */ \
              _BGP_PERS_DDR_Chips_DEFAULT,            /* Chips            */ \
              _BGP_PERS_DDR_CAS_DEFAULT               /* CAS              */ \
              }, \
           {  /* _BGP_Personality_Networks_t: */ \
              0,                                   /* BlockID                */ \
              1, 1, 1,                             /* Xnodes, Ynodes, Znodes */ \
              0, 0, 0,                             /* Xcoord, Ycoord, Zcoord */ \
              0,                                   /* PSetNum                */ \
              0,                                   /* PSetSize               */ \
              0,                                   /* RankInPSet             */ \
              0,                                   /* IOnodes                */ \
              0,                                   /* Rank                   */ \
              0,                                   /* IOnodeRank             */ \
              { 0, }                               /* TreeRoutes[ 16 ]       */ \
              }, \
           {  /* _BGP_Personality_Ethernet_t: */ \
              1536,                                /* mtu              */ \
              { 0, },                              /* EmacID[6]        */ \
              { { 0x00,0x00,0x00,0x00,             /* IPAddress        */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* IPNetmask        */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0xFF,0xFF,0xFF,0x70  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* IPBroadcast      */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* IPGateway        */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* NFSServer        */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              { { 0x00,0x00,0x00,0x00,             /* serviceNode      */ \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00, \
                  0x00,0x00,0x00,0x00  \
                  } }, \
              "",                                  /* NFSExportDir[32] */ \
              "",                                  /* NFSMountDir[32]  */ \
              { 0x00, }                            /* SecurityKey[32]  */ \
              }, \
	      0, 				   /* Block_Config */ \
           { 0, }                                  /* padd[7]          */ \
           }


__END_DECLS



#endif // Add nothing below this line.
