#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#define _S1(x) #x
#define _S(x) _S1(x)

#define __LRTINC(f) _S(lrt/SESA_LRT/f)
#define __LRTDIRINC(d,f) _S(d/lrt/SESA_LRT/f)

#define __ARCHINC(f) _S(arch/SESA_ARCH/f)
#define __ARCHDIRINC(d,f) _S(d/arch/SESA_ARCH/f)

#endif // __INCLUDE_H__

