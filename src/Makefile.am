AM_CFLAGS = -Wall -Werror

bin_PROGRAMS = ebbtest

ebbtest_SOURCES = core/cobj/CObjEBBRootShared.c core/cobj/CObjEBBRootMulti.c
ebbtest_SOURCES += core/cobj/CObjEBB.c core/EBBMgrPrim.c EBBStart.c 
ebbtest_SOURCES += core/EventMgrPrimImp.c core/MemMgrPrim.c net/EthMgrPrim.c
ebbtest_SOURCES += core/sys/defFT.c 

#headers
ebbtest_SOURCES += core/cobj/CObjEBBRootShared.h lrt/io.h core/sys/trans.h
ebbtest_SOURCES += core/cobj/cobj.h core/cobj/CObjEBB.h core/cobj/CObjEBBRoot.h
ebbtest_SOURCES += types.h core/types.h core/cobj/CObjEBBRootMulti.h
ebbtest_SOURCES += core/cobj/CObjEBBRootShared.h core/lrt/types.h
ebbtest_SOURCES += core/MemMgr.h core/MemMgrPrim.h core/const.h core/lrt/const.h
ebbtest_SOURCES += lrt/assert.h core/EBBMgrPrim.h core/EventMgrPrim.h 
ebbtest_SOURCES += core/EventMgrPrimImp.h core/MemMgr.h core/sys/defFT.h
ebbtest_SOURCES += core/MemMgrPrim.h core/lrt/pic.h net/lrt/ethlib.h
ebbtest_SOURCES += net/EthTypeMgr.h net/EthMgr.h net/EthMgrPrim.h

#LRT conditional
ebbtest_SOURCES += core/lrt/ulnx/pic.c lrt/ulnx/lrt_start.c
ebbtest_SOURCES += net/lrt/ulnx/ethlib.c

#headers
ebbtest_SOURCES += core/lrt/ulnx/types.h core/lrt/ulnx/pic.h
ebbtest_SOURCES += core/lrt/ulnx/const.h lrt/ulnx/io.h lrt/ulnx/assert.h

#ARCH conditional
ebbtest_SOURCES += core/sys/arch/amd64/defFT.S

#headers
ebbtest_SOURCES += arch/amd64/asm.h arch/amd64/fpr.h

#PPC32
#ebbtest_SOURCES += core/sys/arch/ppc32/defFT.S 

#headers
#ebbtest_SOURCES += arch/ppc32/asdef.h arch/ppc32/asm.h