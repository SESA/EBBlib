#ifndef __EBB_ASSERT_H__
#define __EBB_ASSERT_H__

#include "lrt/ulnx/EBBAssert.h"

#define EBBAssert LRT_EBBAssert
#define EBBRCAssert(rc) EBBAssert(EBBRC_SUCCESS(rc))

#endif
