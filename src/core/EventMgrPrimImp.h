#ifndef __EBB_EVENT_MGR_PRIM_IMP_H__
#define __EBB_EVENT_MGR_PRIM_IMP_H__
extern EBBRC EventMgrPrimImpInit(void);
extern void EventMgrPrimImpTest(void); /* move to a seperate file eventually, should not be imp specific */
extern void EventMgrEventLoop(void);
#endif
