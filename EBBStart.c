#include <config.h>
#include <types.h>
#include <lrt/io.h>
#include <l0/lrt/pic.h>
#include <lrt/assert.h>

#include <l0/cobj/cobj.h>
#include <l0/types.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/MemMgr.h> 
#include <l0/MemMgrPrim.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <net/EthTypeMgr.h>
#include <net/EthMgr.h>
#include <net/EthMgrPrim.h>

pthread_key_t ELKey;
static void 
kludge(void)
{
  EBBRC rc;
  EthMgrId ethmgr;

    EBB_LRT_printf("%s: start\n", __func__);
  pthread_setspecific(ELKey, (void *)lrt_pic_myid);

  EBBMgrPrimInit();
  rc = EBBMemMgrPrimInit();
  EBBRCAssert(rc);
  rc = EventMgrPrimImpInit();
  EBBRCAssert(rc);
  EBB_LRT_printf("%s: about to call init eth\n", __func__);
  EthMgrPrimCreate(&ethmgr);
  EBBRCAssert(rc);
}

void
ipihdlr(void)
{
  lrt_pic_ackipi();
  fprintf(stderr, "%ld", lrt_pic_myid);
  fflush(stderr);
  sleep(2);
  lrt_pic_enableipi();
  // pass the ipi along to the next lrt
  lrt_pic_ipi((lrt_pic_myid+1)%(lrt_pic_lastid+1));
}


void
EBBStart(void)
{
  /* Three main EBB's are EBBMgrPrim, EventMgrPrim EBBMemMgrPrim    */
  /* There creation and initialization are interdependent and requires */
  /* fancy footwork */

  // put code here to get preboot versions of l0 EBBs ready

  // then invoke a method of BootInfo object on first message
  // this object should gather boot information (sysfacts and boot args)
  // and then get full blown primitive l0 EBBS up (perhaps by a hot swap)
#if 0
  EBB_LRT_printf("%s: ADD REST OF INIT CODE HERE!\n", __func__);
  LRT_EBBAssert(0);
#else
  EBB_LRT_printf("%s: start\n", __func__);
  kludge();
  lrt_pic_mapipi(ipihdlr);
  lrt_pic_ipi(lrt_pic_firstid);
#endif
}
