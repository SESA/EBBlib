#include "../base/include.h"
#include "../base/types.h"
#include "../base/lrtio.h"
#include "../cobj/cobj.h"
#include "EBBTypes.h"
#include "CObjEBB.h"
#include "EBBMgrPrim.h"
#include "EBBMemMgr.h"
#include "EBBMemMgrPrim.h"
#include "EBBCtr.h"
#include "EBBCtrPrim.h"
#include "EBBCtrPrimDistributed.h"
#include "clrBTB.h"
#include "EBB9PClient.h"
#include "EBB9PClientPrim.h"
#include "EBBFile.h"
#include "EBB9PFilePrim.h"
#include "P9FS.h"
#include "CmdMenu.h"
#include "CmdMenuPrim.h"
#include "P9FSPrim.h"
#include "EBBAssert.h"

#include <pthread.h>

#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

pthread_key_t ELKey;

void 
EBBMgrPrimTest(void)
{
  EBBId id1, id2;
  EBBRC rc;

  EBB_LRT_printf("0: EBBId_DREF(theEBBMgrPrimId)=%p: ", EBBId_DREF(theEBBMgrPrimId));
  rc = EBBAllocPrimId(&id1);
  EBB_LRT_printf("rc = %ld id1=%p\n", rc, id1);

  EBB_LRT_printf("1: EBBId_DREF(theEBBMgrPrimId)=%p: ", EBBId_DREF(theEBBMgrPrimId));
  rc = EBBAllocPrimId(&id2);
  EBB_LRT_printf("rc = %ld id2=%p\n", rc, id2);
}

void
EBBMemMgrPrimTest(void)
{
  char *mem;
  EBBPrimMalloc(4, (void **)&mem, EBB_MEM_DEFAULT);
  EBB_LRT_printf("0: mem=%p\n", mem);
  EBBPrimFree(mem);
  EBBPrimMalloc(4, (void **)&mem, EBB_MEM_DEFAULT);
  EBB_LRT_printf("1: mem=%p\n", mem);
  EBBPrimFree(mem);
}

void 
EBBCtrTest(void)
{
  EBBCtrPrimId c;
  EBBCtrPrimDistributedId c2;
  EBBRC rc;
  uval v;

  EBBCtrPrimSharedCreate(&c);
  EBBCtrPrimDistributedCreate(&c2);

  EBB_LRT_printf("id=%p\n", c);
  rc = EC(c)->val(EB(c), &v);
  EBB_LRT_printf("rc=%ld, v=%ld\n", rc, v);

#if 1
  rc = EC(c)->inc(EB(c)); rc = EC(c)->val(EB(c), &v);  
  EBB_LRT_printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->inc(EB(c)); rc = EC(c)->val(EB(c), &v);  
  EBB_LRT_printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->inc(EB(c)); rc = EC(c)->val(EB(c), &v);  
  EBB_LRT_printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c)->dec(EB(c)); rc = EC(c)->val(EB(c), &v);  
  EBB_LRT_printf("rc=%ld, v=%ld\n", rc, v);
#endif

  printf("id=%p\n", c2);
  rc = EC(c2)->val(EB(c2), &v);
  printf("rc=%ld, v=%ld\n", rc, v);

#if 1
  rc = EC(c2)->inc(EB(c2)); rc = EC(c2)->val(EB(c2), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c2)->inc(EB(c2)); rc = EC(c2)->val(EB(c2), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c2)->inc(EB(c2)); rc = EC(c2)->val(EB(c2), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);

  rc = EC(c2)->dec(EB(c2)); rc = EC(c2)->val(EB(c2), &v);  
  printf("rc=%ld, v=%ld\n", rc, v);
#endif

#if 0
  sval i;
  EBBCtrPrimRef r = EB(c);
  EBBRC (*f) (void *_self) = r->ft->inc;
  EBBRC (**ftbl) (void *_self) = &f;
  // EBBRC (**ftbl) (void *_self) = r->ft;

  for (i=0; i<10000000; i++ ) {

#if 0
    clrBTB();
#endif

/*     for (j=0; j<2; j++) { */
#if 0
      rc = inc(r);
#endif
      
#if 0
      rc = f(r);
#endif
      
#if 0
      rc = ftbl[0](r);
#endif
      
#if 0
      rc = r->ft->inc(r);
#endif
      
#if 0
      rc = EC(c)->inc(EB(c)); 
#endif

      EBBRCAssert(rc);
/*     } */
  }

  rc = EC(c)->val(EB(c), &v);
  EBB_LRT_printf("i=%ld j=%ld rc=%ld, v=%ld\n", i, rc, v);
#endif
}

void
EBB9PClientTest(char *address, char *path)
{
  EBB9PClientId p;
  EBBFileId f1, f2, f3;
  EBBRC rc;
  IxpCFid  *fd;
  char buf[80];
  sval n;

  EBB_LRT_printf("EBB9PClientTest: BEGIN: address=%s path=%s\n", address, path);

  EBB9PClientPrimCreate(&p);

  rc = EBBCALL(p, mount, address);
  EBBRCAssert(rc);

  rc = EBBCALL(p, open, path, P9_OREAD, &fd);
  EBBRCAssert(rc);

  rc = EBBCALL(p, read, fd, buf, 80, &n); 
  EBBRCAssert(rc);
  
  if (n==80) n=79;
  buf[n] = 0;
  EBB_LRT_printf("%s\n", buf);
  
#if 0
  EBB9PFilePrimCreate(p, &f1);
  EBB9PFilePrimCreate(p, &f2);
  EBB9PFilePrimCreate(p, &f3);

  rc = EBBCALL(f1, open, "/tmp/stdout", EBBFILE_OWRITE | EBBFILE_OCREATE, 0777);
  EBBRCAssert(rc);
  rc = EBBCALL(f1, write, "stdout", 6, &n);
  EBBRCAssert(rc);

  rc = EBBCALL(f2, open, "/tmp/stderr", EBBFILE_OWRITE | EBBFILE_OCREATE, 0777);
  EBBRCAssert(rc);
  rc = EBBCALL(f2, write, "stderr", 6, &n);
  EBBRCAssert(rc);

  rc = EBBCALL(f3, open, "/tmp/stdin", EBBFILE_OREAD | EBBFILE_OCREATE, 0777);
  EBBRCAssert(rc);
  rc = EBBCALL(f3, read, buf, 80, &n);
  EBBRCAssert(rc);

  EBB_LRT_printf("read: rc=%ld, n=%ld buf=:\n", rc, n);
  if (n) write(1, buf, n);
#endif

  EBB_LRT_printf("EBB9PClientTest: END\n");
}

void 
P9FSTest(char *address)
{
  P9FSid fs;
  CmdMenuId menu;

  EBBRC rc;

  EBB_LRT_printf("P9FSTest: BEGIN: address=%s\n", address);

  CmdMenuPrimCreate(&menu);

  P9FSPrimCreate(&fs, menu);

  rc = EBBCALL(fs, serverloop, address);
  
  EBBRCAssert(rc);
}

int 
main (int argc, char **argv) 
{

  /* Three main EBB's are EBBMgrPrim, EBBEventMgrPrim EBBMemMgrPrim    */
  /* There creation and initialization are interdependent and requires */
  /* fancy footwork */

  pthread_key_create(&ELKey, NULL);
  
  EBBMgrPrimInit();
  EBBMgrPrimTest();

  EBBMemMgrPrimInit();
  EBBMemMgrPrimTest();
  
  EBBCtrTest();

#if 0
  if (argc == 2) 
    EBB9PClientTest(argv[1], "/etc/passwd");
  else if (argc > 2) 
    EBB9PClientTest(argv[1], argv[2]);

  if (argc > 3) 
    P9FSTest(argv[3]);
#endif

  P9FSTest(argv[1]);

  return 0;
}
