AM_CFLAGS = -Wall -Werror

bin_PROGRAMS = ebbtest

ebbtest_SOURCES = l0/cobj/CObjEBBRootShared.c l0/cobj/CObjEBBRootMulti.c
ebbtest_SOURCES += l0/cobj/CObjEBB.c l0/EBBMgrPrim.c EBBStart.c 
ebbtest_SOURCES += l0/EventMgrPrimImp.c l0/MemMgrPrim.c net/EthMgrPrim.c
ebbtest_SOURCES += l0/sys/defFT.c misc/CtrPrim.c misc/CtrPrimDistributed.c

#headers
ebbtest_SOURCES += l0/cobj/CObjEBBRootShared.h lrt/io.h l0/sys/trans.h
ebbtest_SOURCES += l0/cobj/cobj.h l0/cobj/CObjEBB.h l0/cobj/CObjEBBRoot.h
ebbtest_SOURCES += types.h l0/types.h l0/cobj/CObjEBBRootMulti.h
ebbtest_SOURCES += l0/cobj/CObjEBBRootShared.h l0/lrt/types.h
ebbtest_SOURCES += l0/MemMgr.h l0/MemMgrPrim.h l0/const.h l0/lrt/const.h
ebbtest_SOURCES += lrt/assert.h l0/EBBMgrPrim.h l0/EventMgrPrim.h 
ebbtest_SOURCES += l0/EventMgrPrimImp.h l0/MemMgr.h l0/sys/defFT.h
ebbtest_SOURCES += l0/MemMgrPrim.h l0/lrt/pic.h net/lrt/ethlib.h
ebbtest_SOURCES += net/EthTypeMgr.h net/EthMgr.h net/EthMgrPrim.h
ebbtest_SOURCES += l0/cobj/CObjEBBUtils.h misc/Ctr.h misc/CtrPrim.h
ebbtest_SOURCES += misc/CtrPrimDistributed.h

#LRT conditional
ebbtest_SOURCES += l0/lrt/ulnx/pic.c lrt/ulnx/lrt_start.c
ebbtest_SOURCES += net/lrt/ulnx/ethlib.c

#headers
ebbtest_SOURCES += l0/lrt/ulnx/types.h l0/lrt/ulnx/pic.h
ebbtest_SOURCES += l0/lrt/ulnx/const.h lrt/ulnx/io.h lrt/ulnx/assert.h

#ARCH conditional
ebbtest_SOURCES += l0/sys/arch/amd64/defFT.S

#headers
ebbtest_SOURCES += arch/amd64/asm.h arch/amd64/fpr.h

#PPC32
#ebbtest_SOURCES += l0/sys/arch/ppc32/defFT.S 

#headers
#ebbtest_SOURCES += arch/ppc32/asdef.h arch/ppc32/asm.h