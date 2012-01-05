#ifndef ARCH_AMD64_CPU_H
#define ARCH_AMD64_CPU_H

/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdbool.h>
#include <stdint.h>

/* MSRS */
static const uint32_t MSR_APIC_BASE = 0x1b;
static const uint32_t MSR_EFER = 0xc0000080;

/* MSR_APIC_BASE FLAGS */
static const uint64_t MSR_APIC_BASE_BSP = 1 << 8;
static const uint64_t MSR_APIC_BASE_X2APIC_ENABLE = 1 << 10;
static const uint64_t MSR_APIC_BASE_GLOBAL_ENABLE = 1 << 11;
static const uint64_t MSR_APIC_BASE_MASK = ((1 << 24) - 1) << 12;

/* MSR_EFER FLAGS */
static const uint64_t MSR_EFER_LME = 1 << 8;
static const uint64_t MSR_EFER_LMA = 1 << 10;

/* EFLAGS */
static const uint32_t ID_FLAG = 1 << 21;

/* CPUID INDICES */
static const uint32_t CPUID_FEATURES = 0x1;
static const uint32_t CPUID_MAX_EXT_FUNC = 0x80000000;
static const uint32_t CPUID_EXT_FEATURES = 0x80000001;

/* CPUID FEATURE FLAGS */
static const uint32_t CPUID_HAS_X2APIC = 1 << 21;

/* CPUID EXTENDED FEATURE FLAGS */
static const uint32_t CPUID_EXT_HAS_LONGMODE = 1 << 29;

/* CR0 FLAGS */
static const uint64_t CR0_PG = 1 << 31;

/* CR4 FLAGS */
static const uint64_t CR4_PAE = 1 << 5;

static inline bool 
has_cpuid(void) 
{
  // Check if we can set and clear the ID Flag (bit 21) of the
  // EFLag register, indicates support of CPUID
  uint32_t eflags, flipped, flippedback;
  
  __asm__ volatile (
       //get eflags
       "pushfl \n\t"
       "pop %[eflags]\n\t"
       //flip the id flag
       "mov %[eflags], %[flipped]\n\t"
       "xor %[id_flag], %[flipped]\n\t"
       //store back into eflags
       "push %[flipped]\n\t"
       "popfl\n\t"
       "pushfl\n\t"
       "pop %[flipped]\n\t"
       //flip it back
       "push %[eflags]\n\t"
       "popf\n\t"
       "pushf\n\t"
       "pop %[flippedback]"
       : [eflags] "=r" (eflags),
	 [flipped] "=r" (flipped),
	 [flippedback] "=r" (flippedback)
       : [id_flag] "r" (ID_FLAG)
       );

  // Were we able to flip it and flip it back?
  return (((eflags & ID_FLAG) != (flipped & ID_FLAG)) &&
	  ((flipped & ID_FLAG)) != (flippedback & ID_FLAG));
}

static inline void 
cpuid(uint32_t index, uint32_t *eax, uint32_t *ebx, uint32_t *ecx,
      uint32_t *edx)
{
    __asm__ volatile (
	"cpuid"
	: "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
	: "a" (index)
	);
}

static inline bool 
has_ext_features(void)
{
  if (!has_cpuid()) {
    return false;
  }

  uint32_t max_func, dummy;

  cpuid(CPUID_MAX_EXT_FUNC, &max_func, &dummy, &dummy, &dummy);

  return max_func >= CPUID_EXT_FEATURES;
}

static inline bool 
has_longmode(void) 
{
  if (!has_ext_features()) {
    return false;
  }


  //check for long mode

  uint32_t features, dummy;

  cpuid(CPUID_EXT_FEATURES, &dummy, &dummy, &dummy, &features);

  return (features & CPUID_EXT_HAS_LONGMODE);
}

static inline void 
enable_pae(void)
{
  uint32_t cr4;
  __asm__ volatile (
	 "movl %%cr4, %[cr4]"
	 : [cr4] "=r" (cr4)
	 );
  cr4 |= CR4_PAE;
  __asm__ volatile (
	 "movl %[cr4], %%cr4"
	 :
	 : [cr4] "r" (cr4));
}

static inline void 
enable_longmode(void)
{
  uint32_t eax, edx;
  __asm__ volatile (
	 "rdmsr"
	 : "=a" (eax), "=d" (edx)
	 : "c" (MSR_EFER)
	 );

  uint64_t efer = (((uint64_t)edx) << 32) | ((uint64_t)eax);
  efer |= MSR_EFER_LME;
  eax = (uint32_t)efer;
  edx = (uint32_t)(efer >> 32);

  __asm__ volatile (
	 "wrmsr"
	 :
	 : "a" (eax), "d" (edx), "c" (MSR_EFER)
	 );
}

static inline bool 
longmode_active(void)
{
  uint32_t eax, edx;
  __asm__ volatile (
	 "rdmsr"
	 : "=a" (eax), "=d" (edx)
	 : "c" (MSR_EFER)
	 );

  uint64_t efer = (((uint64_t)edx) << 32) | ((uint64_t)eax);
  return (efer & MSR_EFER_LMA);
}

static inline void
enable_paging(void)
{
  uint32_t cr0;
  __asm__ volatile (
	 "movl %%cr0, %[cr0]"
	 : [cr0] "=r" (cr0)
	 );
  cr0 |= CR0_PG;
  __asm__ volatile (
	 "movl %[cr0], %%cr0"
	 :
	 : [cr0] "r" (cr0)
	 );
}

static inline bool
has_x2apic(void)
{
  uint32_t features, dummy;

  cpuid(CPUID_FEATURES, &dummy, &dummy, &features, &dummy);

  return (features & CPUID_HAS_X2APIC);
}

#endif
