include l0/cobj/Makefile.am
include l0/cplus/Makefile.am
include l0/lrt/Makefile.am
include l0/sys/Makefile.am

l0_ebbmgrprim_sources = l0/EBBMgrPrim.c l0/EBBMgrPrim.h l0/EBBMgrPrimBoot.c \
	l0/EBBMgrPrimBoot.h
l0_eventmgrprim_sources = l0/EventMgrPrim.h l0/EventMgrPrimImp.c l0/EventMgrPrimImp.h
l0_memmgrprimstupid_sources = l0/MemMgrPrimStupid.h l0/MemMgrPrimStupid.c
l0_memmgrprimrb_sources = l0/MemMgrPrimRB.h l0/MemMgrPrimRB.c

if MemMgrPrimStupid
l0_memmgrprim_sources = l0/MemMgr.h l0/MemMgrPrim.c l0/MemMgrPrim.h $(l0_memmgrprimstupid_sources)
endif
if MemMgrPrimRB
l0_memmgrprim_sources = l0/MemMgr.h l0/MemMgrPrim.c l0/MemMgrPrim.h $(l0_memmgrprimrb_sources)
endif

l0_sources = l0/const-asm.h l0/const.h l0/types.h l0/l0_start.c $(l0_ebbmgrprim_sources) \
	$(l0_eventmgrprim_sources) $(l0_memmgrprim_sources) $(l0_cobj_sources) \
	$(l0_cplus_sources) $(l0_lrt_sources) $(l0_sys_sources)
