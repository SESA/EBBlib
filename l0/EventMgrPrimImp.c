/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <config.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <l0/lrt/types.h>
#include <l0/cobj/cobj.h>
#include <lrt/io.h>
#include <l0/lrt/pic.h>
#include <l0/lrt/trans.h>
#include <l0/types.h>
#include <l0/sys/trans.h>
#include <lrt/assert.h>
#include <l0/cobj/CObjEBB.h>
#include <l0/EBBMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRoot.h>
#include <l0/cobj/CObjEBBRootMulti.h>
#include <l0/cobj/CObjEBBRootMultiImp.h>
#include <l0/EventMgrPrim.h>
#include <l0/EventMgrPrimImp.h>
#include <l0/lrt/pic.h>
#include <l0/MemMgr.h>
#include <l0/MemMgrPrim.h>

/*
 * We define these in .c file, since EBBCALL may be different for
 * c++..
 */
#define EBBCALL(id, method, ...) COBJ_EBBCALL(id, method, ##__VA_ARGS__)

typedef struct  {
  EventHandlerId id;
} HandlerInfoStruc;

enum {MAXEVENTS = 256};

CObject(EventMgrPrimImp){
  CObjInterface(EventMgrPrim) *ft;
  uintptr_t lock;
  CObjEBBRootMultiRef theRoot;	
  HandlerInfoStruc handlerInfo[MAXEVENTS];
  EventMgrPrimImpRef master;
  uintptr_t ipi_vec_no;
};

EventMgrPrimId theEventMgrPrimId=0;
/*
 * For now, all allocation through a master, need 
 * better way to find and allocate a master.
 */
EventMgrPrimImpRef theEventMgrPrimMaster = NULL;



// define vector functions that map vectors to events
// each function call's the appropriate handler installed
// for the corresponding event that the vector is mapped to
// We use a simple direct map: vector 0 is mapped to event 0
// etc.  This function will eventually be per-rep, and will
// as part of the invocation grab the event stack out fo the rep.
// FIXME: make this a per-rep strucutre
#define VFUNC(i)					\
static void vf##i(void)		    		        \
{							\
 EBBCALL(theEventMgrPrimId, dispatchEventLocal, i);     \
}		

VFUNC(0);
VFUNC(1);
VFUNC(2);
VFUNC(3);
VFUNC(4);
VFUNC(5);
VFUNC(6);
VFUNC(7);
VFUNC(8);
VFUNC(9);
VFUNC(10);
VFUNC(11);
VFUNC(12);
VFUNC(13);
VFUNC(14);
VFUNC(15);
VFUNC(16);
VFUNC(17);
VFUNC(18);
VFUNC(19);
VFUNC(20);
VFUNC(21);
VFUNC(22);
VFUNC(23);
VFUNC(24);
VFUNC(25);
VFUNC(26);
VFUNC(27);
VFUNC(28);
VFUNC(29);
VFUNC(30);
VFUNC(31);
VFUNC(32);
VFUNC(33);
VFUNC(34);
VFUNC(35);
VFUNC(36);
VFUNC(37);
VFUNC(38);
VFUNC(39);
VFUNC(40);
VFUNC(41);
VFUNC(42);
VFUNC(43);
VFUNC(44);
VFUNC(45);
VFUNC(46);
VFUNC(47);
VFUNC(48);
VFUNC(49);
VFUNC(50);
VFUNC(51);
VFUNC(52);
VFUNC(53);
VFUNC(54);
VFUNC(55);
VFUNC(56);
VFUNC(57);
VFUNC(58);
VFUNC(59);
VFUNC(60);
VFUNC(61);
VFUNC(62);
VFUNC(63);
VFUNC(64);
VFUNC(65);
VFUNC(66);
VFUNC(67);
VFUNC(68);
VFUNC(69);
VFUNC(70);
VFUNC(71);
VFUNC(72);
VFUNC(73);
VFUNC(74);
VFUNC(75);
VFUNC(76);
VFUNC(77);
VFUNC(78);
VFUNC(79);
VFUNC(80);
VFUNC(81);
VFUNC(82);
VFUNC(83);
VFUNC(84);
VFUNC(85);
VFUNC(86);
VFUNC(87);
VFUNC(88);
VFUNC(89);
VFUNC(90);
VFUNC(91);
VFUNC(92);
VFUNC(93);
VFUNC(94);
VFUNC(95);
VFUNC(96);
VFUNC(97);
VFUNC(98);
VFUNC(99);
VFUNC(100);
VFUNC(101);
VFUNC(102);
VFUNC(103);
VFUNC(104);
VFUNC(105);
VFUNC(106);
VFUNC(107);
VFUNC(108);
VFUNC(109);
VFUNC(110);
VFUNC(111);
VFUNC(112);
VFUNC(113);
VFUNC(114);
VFUNC(115);
VFUNC(116);
VFUNC(117);
VFUNC(118);
VFUNC(119);
VFUNC(120);
VFUNC(121);
VFUNC(122);
VFUNC(123);
VFUNC(124);
VFUNC(125);
VFUNC(126);
VFUNC(127);
VFUNC(128);
VFUNC(129);
VFUNC(130);
VFUNC(131);
VFUNC(132);
VFUNC(133);
VFUNC(134);
VFUNC(135);
VFUNC(136);
VFUNC(137);
VFUNC(138);
VFUNC(139);
VFUNC(140);
VFUNC(141);
VFUNC(142);
VFUNC(143);
VFUNC(144);
VFUNC(145);
VFUNC(146);
VFUNC(147);
VFUNC(148);
VFUNC(149);
VFUNC(150);
VFUNC(151);
VFUNC(152);
VFUNC(153);
VFUNC(154);
VFUNC(155);
VFUNC(156);
VFUNC(157);
VFUNC(158);
VFUNC(159);
VFUNC(160);
VFUNC(161);
VFUNC(162);
VFUNC(163);
VFUNC(164);
VFUNC(165);
VFUNC(166);
VFUNC(167);
VFUNC(168);
VFUNC(169);
VFUNC(170);
VFUNC(171);
VFUNC(172);
VFUNC(173);
VFUNC(174);
VFUNC(175);
VFUNC(176);
VFUNC(177);
VFUNC(178);
VFUNC(179);
VFUNC(180);
VFUNC(181);
VFUNC(182);
VFUNC(183);
VFUNC(184);
VFUNC(185);
VFUNC(186);
VFUNC(187);
VFUNC(188);
VFUNC(189);
VFUNC(190);
VFUNC(191);
VFUNC(192);
VFUNC(193);
VFUNC(194);
VFUNC(195);
VFUNC(196);
VFUNC(197);
VFUNC(198);
VFUNC(199);
VFUNC(200);
VFUNC(201);
VFUNC(202);
VFUNC(203);
VFUNC(204);
VFUNC(205);
VFUNC(206);
VFUNC(207);
VFUNC(208);
VFUNC(209);
VFUNC(210);
VFUNC(211);
VFUNC(212);
VFUNC(213);
VFUNC(214);
VFUNC(215);
VFUNC(216);
VFUNC(217);
VFUNC(218);
VFUNC(219);
VFUNC(220);
VFUNC(221);
VFUNC(222);
VFUNC(223);
VFUNC(224);
VFUNC(225);
VFUNC(226);
VFUNC(227);
VFUNC(228);
VFUNC(229);
VFUNC(230);
VFUNC(231);
VFUNC(232);
VFUNC(233);
VFUNC(234);
VFUNC(235);
VFUNC(236);
VFUNC(237);
VFUNC(238);
VFUNC(239);
VFUNC(240);
VFUNC(241);
VFUNC(242);
VFUNC(243);
VFUNC(244);
VFUNC(245);
VFUNC(246);
VFUNC(247);
VFUNC(248);
VFUNC(249);
VFUNC(250);
VFUNC(251);
VFUNC(252);
VFUNC(253);
VFUNC(254);
VFUNC(255);

typedef void (*vfunc)(void);
vfunc vfTbl[MAXEVENTS] = {
  vf0, vf1, vf2, vf3, vf4, vf5, vf6, vf7, vf8, vf9,
  vf10, vf11, vf12, vf13, vf14, vf15, vf16, vf17, vf18, vf19,
  vf20, vf21, vf22, vf23, vf24, vf25, vf26, vf27, vf28, vf29,
  vf30, vf31, vf32, vf33, vf34, vf35, vf36, vf37, vf38, vf39,
  vf40, vf41, vf42, vf43, vf44, vf45, vf46, vf47, vf48, vf49,
  vf50, vf51, vf52, vf53, vf54, vf55, vf56, vf57, vf58, vf59,
  vf60, vf61, vf62, vf63, vf64, vf65, vf66, vf67, vf68, vf69,
  vf70, vf71, vf72, vf73, vf74, vf75, vf76, vf77, vf78, vf79,
  vf80, vf81, vf82, vf83, vf84, vf85, vf86, vf87, vf88, vf89,
  vf90, vf91, vf92, vf93, vf94, vf95, vf96, vf97, vf98, vf99,
  vf100, vf101, vf102, vf103, vf104, vf105, vf106, vf107, vf108, vf109, 
  vf110, vf111, vf112, vf113, vf114, vf115, vf116, vf117, vf118, vf119, 
  vf120, vf121, vf122, vf123, vf124, vf125, vf126, vf127, vf128, vf129, 
  vf130, vf131, vf132, vf133, vf134, vf135, vf136, vf137, vf138, vf139, 
  vf140, vf141, vf142, vf143, vf144, vf145, vf146, vf147, vf148, vf149, 
  vf150, vf151, vf152, vf153, vf154, vf155, vf156, vf157, vf158, vf159, 
  vf160, vf161, vf162, vf163, vf164, vf165, vf166, vf167, vf168, vf169, 
  vf170, vf171, vf172, vf173, vf174, vf175, vf176, vf177, vf178, vf179, 
  vf180, vf181, vf182, vf183, vf184, vf185, vf186, vf187, vf188, vf189, 
  vf190, vf191, vf192, vf193, vf194, vf195, vf196, vf197, vf198, vf199, 
  vf200, vf201, vf202, vf203, vf204, vf205, vf206, vf207, vf208, vf209, 
  vf210, vf211, vf212, vf213, vf214, vf215, vf216, vf217, vf218, vf219, 
  vf220, vf221, vf222, vf223, vf224, vf225, vf226, vf227, vf228, vf229, 
  vf230, vf231, vf232, vf233, vf234, vf235, vf236, vf237, vf238, vf239, 
  vf240, vf241, vf242, vf243, vf244, vf245, vf246, vf247, vf248, vf249, 
  vf250, vf251, vf252, vf253, vf254, vf255
};


static uintptr_t
spin_lock(uintptr_t *lock)
{
  uintptr_t rc=0;
  
  while (!rc) {
    rc = __sync_bool_compare_and_swap(lock, 0, 1);
  }
  return rc;
}

static void
spin_unlock(uintptr_t *lock)
{
  __sync_bool_compare_and_swap(lock, 1, 0);
}

static EBBRC
EventMgrPrim_dispatchIPI(void *_self, EvntLoc el)
{
  if (el != MyEL()) {
    EBB_LRT_printf("%s: sending remote IPI to node %" PRIdPTR "\n", 
		   __func__,
		   el);
  }
  lrt_pic_ipi(el);
  return EBBRC_OK;
}

/*
 * This should go away once we have proper implementation of vector
 * function that should inline this.  Should also buy stack here...
 */
static EBBRC
EventMgrPrim_dispatchEventLocal(void *_self, uintptr_t eventNo) 
{
  EventMgrPrimImpRef self = (EventMgrPrimImpRef)_self;
  // assume this is atomic
  EventHandlerId handler = self->handlerInfo[eventNo].id;

  EBB_LRT_printf("%s: handling interrupt %" PRIdPTR "\n", __func__, eventNo);
  EBBAssert(handler != NULL); 
  EBBCALL(handler, handleEvent); 
  return EBBRC_OK;   
}


// this is done under protection of the master's lock
static EBBRC
lockedReplicateHandler(CObjEBBRootMultiRef root, uintptr_t eventNo, 
		       EventHandlerId handler)
{
  RepListNode *node;
  EventMgrPrimImpRef rep;
  // now iterate through all reps and put in hander
  
  for (node = root->ft->nextRep(root, 0, (EBBRep **)&rep);
       node != NULL; 
       node = root->ft->nextRep(root, node, (EBBRep **)&rep)) {
     rep->handlerInfo[eventNo].id = handler;
  }
  return EBBRC_OK;   
}



static EBBRC
lockedRegisterHandler(EventMgrPrimImpRef master, uintptr_t eventNo, 
		 EventHandlerId handler)
{
  if ( (eventNo >= MAXEVENTS) || (eventNo<0) ){
    return EBBRC_BADPARAMETER;
  };
  
  if (master->handlerInfo[eventNo].id != NULL) {
    // for now, if its not null, assume error, should we ever be able
    // to change the handler for an event?
    return EBBRC_BADPARAMETER;
  };

  // install handler in event table
  master->handlerInfo[eventNo].id = handler;

  lockedReplicateHandler(master->theRoot, eventNo, handler);

  return 0;
}

static EBBRC
EventMgrPrim_registerHandler(void *_self, uintptr_t eventNo, 
			     EventHandlerId handler, 
			     uintptr_t isrc)
{
  EventMgrPrimImpRef self = (EventMgrPrimImpRef)_self;
  EventMgrPrimImpRef master = self->master;
  EBBRC rc = EBBRC_OK;

  spin_lock(&master->lock);
  rc = lockedRegisterHandler(self->master, eventNo, handler);
  if (!EBBRC_SUCCESS(rc)) goto done;

  // map vector in pic
  if (lrt_pic_mapvec((lrt_pic_src)isrc, eventNo, vfTbl[eventNo])<0) {
    // FAILED unmap from all the tables
    lockedReplicateHandler(self->theRoot, eventNo, NULL);
    rc = EBBRC_BADPARAMETER;
    goto done;
  }

 done:
  spin_unlock(&master->lock);
  return rc;
}

/* 
 * IPI is different from other handlers, since it is a purely local
 * operation, i.e., both at the pic and in the EventMgr we are going
 * to a different handler for IPIs on each processor.  This is
 * necessary since we remap IPIs on different processors as we go
 * through the initialization.  I assume, for now, that all other
 * interrupts are globally allocated and are the same on all ELs
 * (Event Locations).
 */
static EBBRC
EventMgrPrim_registerIPIHandler(void *_self, EventHandlerId handler)
{
  EventMgrPrimImpRef self = (EventMgrPrimImpRef)_self;

  self->handlerInfo[self->ipi_vec_no].id = handler;

  // map vector in pic
  lrt_pic_mapipi(vfTbl[self->ipi_vec_no]);

  return 0;
}

static EBBRC 
EventMgrPrim_allocEventNo(void *_self, uintptr_t *eventNoPtr)
{
  if (lrt_pic_allocvec(eventNoPtr)<0) return EBBRC_OUTOFRESOURCES;
  return EBBRC_OK;
}

CObjInterface(EventMgrPrim) EventMgrPrimImp_ftable = {
  .registerHandler = EventMgrPrim_registerHandler, 
  .registerIPIHandler = EventMgrPrim_registerIPIHandler, 
  .allocEventNo = EventMgrPrim_allocEventNo, 
  .dispatchIPI = EventMgrPrim_dispatchIPI,
  .dispatchEventLocal = EventMgrPrim_dispatchEventLocal,
};

static void
EventMgrPrimSetFT(EventMgrPrimImpRef o)
{
  o->ft = &EventMgrPrimImp_ftable;
}

static EBBRep *
EventMgrPrimImp_createRepAssert(CObjEBBRootMultiRef root) 
{
  EBBAssert(0);
  return NULL;
}


static EventMgrPrimImpRef
EventMgrPrimImp_createRep(CObjEBBRootMultiImpRef root) 
{
  int i; 
  EventMgrPrimImpRef repRef;

  EBBRCAssert(EBBPrimMalloc(sizeof(*repRef), &repRef, EBB_MEM_DEFAULT));
  EventMgrPrimSetFT(repRef);
  repRef->theRoot = (CObjEBBRootMultiRef)root;
  repRef->ipi_vec_no = lrt_pic_getIPIvec();
  spin_lock(&repRef->lock);
  if (__sync_bool_compare_and_swap(&theEventMgrPrimMaster, 
				   (EventMgrPrimRef)NULL,
				   repRef)) {
    // cool, I am the master, 
    repRef->master = repRef;
    for (i=0; i<MAXEVENTS; i++) {
      repRef->handlerInfo[i].id = NULL;
    }
    spin_unlock(&repRef->lock);
  } else {
    EventMgrPrimImpRef mstr = theEventMgrPrimMaster;

    repRef->master = mstr;
    spin_unlock(&repRef->lock);
    spin_lock(&mstr->lock);
    spin_lock(&repRef->lock);
    // copy the handler list of the master rep
    for (i=0; i<MAXEVENTS; i++) {
      repRef->handlerInfo[i].id = mstr->handlerInfo[i].id;
    }
    spin_unlock(&repRef->lock);
    spin_unlock(&mstr->lock);
  }
  return repRef;
}

EBBRC
EventMgrPrimImpInit(void)
{
  EBBRC rc;
  static CObjEBBRootMultiImpRef rootRef;
  EventMgrPrimImpRef repRef;
  EvntLoc myel;

  if (__sync_bool_compare_and_swap(&theEventMgrPrimId, (EventMgrPrimId)0,
				   (EventMgrPrimId)-1)) {
    EBBId id;
    EBBAssert(MAXEVENTS >= lrt_pic_numvec());
    rc = CObjEBBRootMultiImpCreate(&rootRef, EventMgrPrimImp_createRepAssert);
    EBBRCAssert(rc);
    rc = EBBAllocPrimId(&id);
    EBBRCAssert(rc);
    rc = EBBBindPrimId(id, CObjEBBMissFunc, (EBBMissArg)rootRef);
    EBBRCAssert(rc);
    theEventMgrPrimId = (EventMgrPrimId)id;
  } else {
    while (((volatile uintptr_t)theEventMgrPrimId)==-1);
  }
  // It makes no sense to handle miss on this object lazily, since it will 
  // always be invoked on every node, everything is in an event
  repRef = EventMgrPrimImp_createRep(rootRef);
  myel = MyEL();
  
  rootRef->ft->addRepOn((CObjEBBRootMultiRef)rootRef, myel, (EBBRep *)repRef);
  return EBBRC_OK;
};

