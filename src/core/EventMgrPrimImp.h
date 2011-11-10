#ifndef __EBB_EVENT_MGR_PRIM_IMP_H__
#define __EBB_EVENT_MGR_PRIM_IMP_H__
extern EBBRC EBBEventMgrPrimImpInit(void);
extern void EBBEventMgrPrimImpTest(void); /* move to a seperate file eventually, should not be imp specific */
extern void EBBEventMgrEventLoop(void);
#endif
