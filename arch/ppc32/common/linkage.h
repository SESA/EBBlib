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
 * \file common/linkage.h
 */

#ifndef	_LINKAGE_H_ // Prevent multiple inclusion
#define	_LINKAGE_H_



#include <common/namespace.h>

#if defined(__ASSEMBLY__)

// Coordinate Prolog Segmentation with the linker script (.lds) files.

#define _PROLOG_CHECKSUM(fcn) \
         .section ".kernel_checksum","ax" ; \
         .type fcn,@function ; \
         .global fcn;

#define _PROLOG_START(fcn) \
         .section ".ktext.start","ax" ; \
         .type fcn,@function ; \
         .global fcn;

#define _PROLOG_KTEXT(fcn) \
         .section ".ktext","ax" ; \
         .type fcn,@function ; \
         .global fcn;

#define _PROLOG_KTEXT_SRAM(fcn) \
         .section ".ktext.sram","ax" ; \
         .type fcn,@function ; \
         .global fcn;

#define _PROLOG_TEXT(fcn) \
         .section ".text","ax" ; \
         .type fcn,@function ; \
         .global fcn;

// 4 bytes for a single instruction right at 0xFFFFFFFC
#define _PROLOG_RESET_VECTOR(fcn) \
         .section ".text.resetvector","ax" ; \
         .type fcn,@function ; \
         .global fcn;

// 28 bytes abutted to .text.resetvector above
#define _DPROLOG_RESET_VECTOR(dat) \
         .section ".data.resetvector","aw" ; \
         .type dat,@object ; \
         .global dat

// 28 bytes abutted to .text.resetvector above
#define _DPROLOG_BLOAD_STATE(dat) \
         .section ".data.bloadstate","aw" ; \
         .type dat,@object ; \
         .global dat

#define _PROLOG_RESET(fcn) \
         .section ".text.reset","ax" ; \
         .align ALIGN_L1I_CACHE; \
         .type fcn,@function ; \
         .global fcn;

#define _PROLOG_RESETEXTRA(fcn) \
         .section ".text.resetextra","ax" ; \
         .align 2; \
         .type fcn,@function ; \
         .global fcn;

#define _DPROLOG_RESET(dat,a) \
         .section ".data.reset","aw" ; \
         .type dat,@object ; \
         .align a ; \
         .global dat

#define _DPROLOG_SRAM_STACK(dat,a) \
         .section ".sram_stack","aw" ; \
         .type dat,@object ; \
         .align a ; \
         .global dat

#define _DPROLOG_TLB_TABLE(dat,a) \
         .section ".tlb_table","ax" ; \
         .type dat,@object ; \
         .align a ; \
         .global dat

#define _EPILOG(fcn) \
         .size fcn,.-fcn ; \
         .previous

#define _DEPILOG(dat) \
         .size dat,.-dat ;


#else // not __ASSEMBLY__

__BEGIN_DECLS

// Locate code or data in the "init" section
//  eg: int INIT foo(...) { }
//      char *str INITDATA = "Hello world!";

#define _u_text           __attribute__ ((__section__ (".text")))
#define _u_data           __attribute__ ((__section__ (".data")))
#define _u_stack          __attribute__ ((__section__ (".stack")))

#define _u_init           __attribute__ ((__section__ (".text.init")))
#define _u_initdata       __attribute__ ((__section__ (".data.init")))

#define _u_sram           __attribute__ ((__section__ (".text.sram")))
#define _u_sramdata       __attribute__ ((__section__ (".data.sram")))

#define _u_rodata         __attribute__ ((__section__ (".rodata")))
#define _u_bss            __attribute__ ((__section__ (".bss")))

#define _k_textreset      __attribute__ ((__section__ (".text.reset")))
#define _k_textresetextra __attribute__ ((__section__ (".text.resetextra")))
#define _k_datareset      __attribute__ ((__section__ (".data.reset")))

#define _k_textstart      __attribute__ ((__section__ (".ktext.start")))
#define _k_text           __attribute__ ((__section__ (".ktext")))
#define _k_textuncached   __attribute__ ((__section__ (".ktext.uncached")))
#define _k_texttransient  __attribute__ ((__section__ (".ktext.transient")))

#define _k_datachksum     __attribute__ ((__section__ (".kdata.checksum")))
#define _k_data           __attribute__ ((__section__ (".kdata")))
#define _k_stack          __attribute__ ((__section__ (".stack")))

#define _k_init           __attribute__ ((__section__ (".ktext.init")))
#define _k_initdata       __attribute__ ((__section__ (".kdata.init")))
#define _k_datanodestate  __attribute__ ((__section__ (".kdata.nodestate")))
#define _k_dataconfig     __attribute__ ((__section__ (".kdata.config")))
#define _k_datauncached   __attribute__ ((__section__ (".kdata.uncached")))
#define _k_datatransient  __attribute__ ((__section__ (".kdata.transient")))

#define _k_sram           __attribute__ ((__section__ (".ktext.sram")))
#define _k_sramdata       __attribute__ ((__section__ (".kdata.sram")))

#define _k_rodata         __attribute__ ((__section__ (".krodata")))
#define _k_bss            __attribute__ ((__section__ (".kbss")))

#define _k_tlb_table      __attribute__ ((__section__ (".tlb_table")))

#define __long_call       __attribute__ ((longcall))
#define __short_call      __attribute__ ((shortcall))
#define __noreturn        __attribute__ ((noreturn))

#define LONG_CALL         __long_call
#define SHORT_CALL        __short_call
#define NORETURN          __noreturn

// Use these definitions in the code so we can more easily
//  change or (en|dis)able the segmentation.
#define TEXT              _u_text
#define DATA              _u_data
#define U_STACK           _u_stack
#define INIT              _u_init
#define INITDATA          _u_initdata
#define SRAM              _u_sram
#define SRAMDATA          _u_sramdata
#define RODATA            _u_rodata
#define BSSDATA           _u_bss

#define K_TEXT_START      _k_textstart
#define K_TEXT            _k_text
#define K_TEXT_UNCACHED   _k_textuncached
#define K_TEXT_TRANSIENT  _k_texttransient
#define K_TEXTRESET       _k_textreset
#define K_TEXTRESETEXTRA  _k_textresetextra
#define K_DATARESET       _k_datareset
#define K_DATA_CHECKSUM   _k_datachksum
#define K_DATA            _k_data
#define K_DATA_NODE_STATE _k_datanodestate
#define K_DATA_UNCACHED   _k_datauncached
#define K_DATA_TRANSIENT  _k_datatransient
#define K_DATACONFIG      _k_dataconfig
#define K_INIT            _k_init
#define K_INITDATA        _k_initdata
#define K_SRAM            _k_sram
#define K_SRAMDATA        _k_sramdata
#define K_RODATA          _k_rodata
#define K_BSSDATA         _k_bss
#define K_TLB_TABLE       _k_tlb_table
#define K_STACK           _k_stack

#define _SRAM_MAILBOX_TO_CORE __attribute__ ((__section__ (".MailboxToCore")))
#define _SRAM_MAILBOX_TO_HOST __attribute__ ((__section__ (".MailboxToHost")))
#define _SRAM_PERSONALITY     __attribute__ ((__section__ (".personality")))
#define _SRAM_MBOX_TABLE      __attribute__ ((__section__ (".data.mboxtable")))
#define _SRAM_PERS_TABLE      __attribute__ ((__section__ (".data.perstable")))
#define _SRAM_BLOAD_STATE     __attribute__ ((__section__ (".data.bloadstate")))
#define _SRAM_TEXT            __attribute__ ((__section__ (".text_sram")))
#define _SRAM_DATA            __attribute__ ((__section__ (".data_sram")))


// This is needed whenever DDR is not ECC Initialized everywhere.
#define K_TEXT_SIM_PAD  __attribute__ ((__section__ (".text_pad_for_sim")))
#define K_DATA_SIM_PAD  __attribute__ ((__section__ (".data_pad_for_sim")))

#define PACKED __attribute__ ((packed))


__END_DECLS

#endif



#endif // Add nothing below this line

