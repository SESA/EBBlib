bin_PROGRAMS = ebbtest
ebbtest_SOURCES = core/cobj/CObjEBBRootShared.c core/cobj/CObjEBBRootMulti.c
ebbtest_SOURCES += core/cobj/CObjEBB.c core/cobj/CObjEBBRootShared.h
ebbtest_SOURCES += core/cobj/cobj.h core/cobj/CObjEBB.h core/cobj/CObjEBBRoot.h
ebbtest_SOURCES += types.h core/types.h core/cobj/CObjEBBRootMulti.h
ebbtest_SOURCES += core/cobj/CObjEBBRootShared.h core/lrt/types.h
ebbtest_SOURCES += core/MemMgr.h core/MemMgrPrim.h core/const.h
ebbtest_SOURCES += core/EBBMgrPrim.c lrt/io.h core/sys/trans.h core/lrt/const.h
ebbtest_SOURCES += lrt/assert.h core/EBBMgrPrim.h core/EventMgrPrim.h 
ebbtest_SOURCES += core/EventMgrPrimImp.c core/EventMgrPrimImp.h core/MemMgr.h
ebbtest_SOURCES += core/MemMgrPrim.h core/MemMgrPrim.c core/sys/defFT.h 
ebbtest_SOURCES += core/lrt/pic.h core/sys/defFT.c EBBStart.c
ebbtest_SOURCES += net/EthTypeMgr.h net/EthMgr.h net/EthMgrPrim.h
ebbtest_SOURCES += net/EthMgrPrim.c net/lrt/ethlib.h

#LRT conditional
ebbtest_SOURCES += core/lrt/ulnx/types.h core/lrt/ulnx/pic.h
ebbtest_SOURCES += core/lrt/ulnx/const.h core/lrt/ulnx/pic.c
ebbtest_SOURCES += lrt/ulnx/io.h lrt/ulnx/assert.h lrt/ulnx/lrt_start.c
ebbtest_SOURCES += net/lrt/ulnx/ethlib.c

#ARCH conditional
ebbtest_SOURCES += core/sys/arch/amd64/defFT.S arch/amd64/asm.h arch/amd64/fpr.h
#ebbtest_SOURCES += core/sys/arch/ppc32/defFT.S arch/ppc32/asm.h
#ebbtest_SOURCES += arch/ppc32/asdef.h