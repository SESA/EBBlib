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


#ifndef __BGP_ERRNO_H__
#define __BGP_ERRNO_H__


/********************************************************
*
* Master file containing rules for Blue light errors.
* and error code bases.
*
* NOTE: All error codes are < 0...
*
*/

/**************************************************************
*
*
* There are three flavor of error?? return code.
*
* 1.  Error..
* 2.  Warning..
* 2.  Info...
*
* All error codes are < Zero s
* and all success codes (info/warning) are > than zero.
* unconditional success is == 0, or = BGPERR_SUCCESS.
*
*/
#define BGPERR_SUCCESS 0

/*************************************************************************
*
* Each subsysetm is assigned a error range, or error base.
*
* Errors are constructed by the macro BGL_ERROR(n, base) ((n) - base)
*
* Warnings and information are distinguished by the top two bits [0:1]
* in the return code.
*
* 00 -- information
* 01 -- warning
*
*/
#define BGPERR_FL_WARNING (1<<(31-1))
/************************************************************************
*
* the following macros are used to construct error, warnings and info
* codes as follows:
*/

#define BGPERR(x, base)   (-(base) - x)
#define BGPWARN(x, base)  (((base) | (BGPERR_FL_WARNING)) + (x))
#define BGPINFO(x, base)  ((base) + (x))

/***********************************************************
*
* Currently each error subrange has a 1000 element range
*
* Create new subrantes by adding new base codes
*/
#define BGPERR_BASE             0           /* base for common errors */
#define BGPERR_BASE_CTRL_NET    (1*1000)    /* base for control network errors */
#define BGPERR_BASE_MCSERVER    (1*2000)    /* base for MCServer errors */

/***********************************
*
* ... add more subranges here...
*
*/



#define BGPERR_INPROG           BGPINFO(1, BGPERR_BASE) /* device is busy (not an error). */
                                                                /* */
#define BGPERR_NO_MEMORY        BGPERR(1, BGPERR_BASE) /* Out of memory */

#define BGPERR_UNIMPLEMENTED    BGPERR(2,  BGPERR_BASE) /* feature unimplemented */
#define BGPERR_FILEOPEN_ERR     BGPERR(3,  BGPERR_BASE) /* generic file open error */
#define BGPERR_INVALID_DATA     BGPERR(4,  BGPERR_BASE) /* generic invalid data error */
#define BGPERR_INTERNAL_ERR     BGPERR(5,  BGPERR_BASE) /* generic internal error. */
#define BGPERR_FILEREAD_ERR     BGPERR(6,  BGPERR_BASE) /* generic file read error */
#define BGPERR_PARAM_ERR        BGPERR(7,  BGPERR_BASE) /* generic parameter error */
#define BGPERR_NOT_FOUND        BGPERR(8,  BGPERR_BASE) /* whatever you were were looling for  */
                                                        /* it was not found. */
#define BGPERR_STRUCT_TOO_SMALL BGPERR(9,  BGPERR_BASE) /* structure or array passed in was too small. */
#define BGPERR_DUPLICATE_ENTRY  BGPERR(10, BGPERR_BASE) /* duplicate entry in some structure. */
#define BGPERR_INVALID_HANDLE   BGPERR(11, BGPERR_BASE) /* invalid handle. */
#define BGPERR_FILEWRITE_ERR    BGPERR(12, BGPERR_BASE) /* generic file read error */
#define BGPERR_DDR_ERR          BGPERR(13, BGPERR_BASE) /* ddr failure */
#define BGPERR_L3_ERR           BGPERR(14, BGPERR_BASE) /* l3 cache failure */
#define BGPERR_FILE_EOF         BGPERR(15, BGPERR_BASE) /* end of file error */

#endif
