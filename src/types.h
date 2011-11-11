/* Copyright 2011 Boston University. All rights reserved.                  */
/*                                                                         */  
/* Redistribution and use in source and binary forms, with or without      */ 
/* modification, are permitted provided that the following conditions are  */
/* met:                                                                    */
/*                                                                         */  
/*    1. Redistributions of source code must retain the above copyright    */
/*       notice, this list of conditions and the following disclaimer.     */
/*                                                                         */  
/*    2. Redistributions in binary form must reproduce the above copyright */ 
/*       notice, this list of conditions and the following disclaimer in   */
/*       the documentation and/or other materials  provided with the       */
/*       distribution.                                                     */
/*                                                                         */  
/*                                                                         */  
/* THIS SOFTWARE IS PROVIDED BY BOSTON UNIVERSITY ``AS IS'' AND ANY        */
/* EXPRESS OR IMPLIED  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      */
/* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BOSTON UNIVERSITY OR          */
/* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,   */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,     */
/* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR      */
/* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS      */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.            */
/*                                                                         */  
/* The views and conclusions contained in the software and documentation   */
/* are those of the  authors and should not be interpreted as representing */
/* official policies, either expressed or implied, of Boston University    */

#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char uval8;
typedef unsigned short uval16;
typedef unsigned int uval32;
typedef unsigned long long uval64;
typedef unsigned long uval;
typedef char sval8;
typedef short sval16;
typedef int sval32;
typedef long long sval64;
typedef long sval;

#ifndef NULL
#define NULL (void *)(0)
#endif

#endif	