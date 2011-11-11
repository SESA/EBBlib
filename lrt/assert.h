#ifndef __EBB_ASSERT_H__
#define __EBB_ASSERT_H__

#ifdef LRT_ULNX
#include <lrt/ulnx/assert.h>
#endif

#define EBBAssert LRT_EBBAssert
#define EBBRCAssert(rc) EBBAssert(EBBRC_SUCCESS(rc))

#endif
