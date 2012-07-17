/*
 * Copyright (C) 2012 by Project SESA, Boston University
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

#include <stdbool.h>
#include <stdint.h>

#include <l0/EBBMgrPrim.h>
#include <l0/MemMgrPrim.h>
#include <l0/cobj/CObjEBBUtils.h>
#include <l0/cobj/CObjEBBRootShared.h>
#include <l0/lrt/event.h>
#include <intercept/Interceptor.h>

static EBBFunc InterceptorProxyRep_ftable[];

CObject(InterceptorProxyRep) {
  EBBFunc *ft;
  EBBId curr_id;
  InterceptorId interceptor_id;
  EBBId target_id;
  bool removing;
  EBBMissFunc targmf;
  EBBBindFunc targbf;
  EBBArg targarg;
};

CObject(InterceptorControllerImp) {
  CObjInterface(InterceptorController) *ft;
  CObjEBBRootSharedRef controllerRootRef;
  CObjEBBRootSharedRef proxyRootRef;
  InterceptorProxyRepRef proxyRepRef;
};

static void
interceptBindFunc(EBBId *id, EBBMissFunc *mf, void *bf, EBBArg *arg, EBBArg oldarg)
{
  CObjEBBRootSharedRef rootref = (CObjEBBRootSharedRef)oldarg;
  InterceptorProxyRepRef repref = (InterceptorProxyRepRef)
    rootref->ft->getRep(rootref);
  if (!repref->removing) {
    *id = repref->target_id;
    if (mf) {
      repref->targmf = *mf;
    }
    if (bf) {
      repref->targbf = *(EBBBindFunc *)bf;
    }
    if (arg) {
      repref->targarg = *arg;
    }
  }
}

EBBRC
intercept_precall(struct args *args, EBBFuncNum fnum, union func_ret *fr)
{
  //we know the first arg is the this pointer
  InterceptorProxyRepRef ref = *(InterceptorProxyRepRef *)args;
  lrt_trans_rep_ref targ_ref = lrt_trans_id_dref(ref->target_id);
  *(lrt_trans_rep_ref *)args = targ_ref;
  lrt_event_altstack_push((uintptr_t)ref);
  fr->func = targ_ref->ft[fnum];
  InterceptorId intId = ref->interceptor_id;
  return COBJ_EBBCALL(intId, PreCall, args, fnum, fr);
}

EBBRC
intercept_postcall(EBBFuncNum fnum, EBBRC rc)
{
  InterceptorProxyRepRef ref = (InterceptorProxyRepRef)lrt_event_altstack_pop();
  InterceptorId intId = ref->interceptor_id;
  return COBJ_EBBCALL(intId, PostCall, rc);
}

EBBRC
InterceptorControllerImp_Start(InterceptorControllerRef _self, EBBId target_id,
                InterceptorId interceptor_id)
{
  InterceptorControllerImpRef self = (InterceptorControllerImpRef)_self;

  EBBMissFunc mf = CObjEBBMissFunc;
  EBBBindFunc bf = interceptBindFunc;
  EBBArg arg = (EBBArg)self->proxyRootRef;

  InterceptorProxyRepRef proxyRepRef = self->proxyRepRef;
  proxyRepRef->curr_id = target_id;
  proxyRepRef->interceptor_id = interceptor_id;
  proxyRepRef->removing = false;

  EBBId newTarg = proxyRepRef->target_id;

  //bind in the interceptorproxy at the target_id
  EBBRC rc = COBJ_EBBCALL(theEBBMgrPrimId, BindId, target_id, &mf, &bf, &arg, 1);
  LRT_RCAssert(rc);

  proxyRepRef->targmf = mf;
  proxyRepRef->targbf = bf;
  proxyRepRef->targarg = arg;

  //bind in the target to the new id
  rc = COBJ_EBBCALL(theEBBMgrPrimId, BindId, newTarg, &mf, &bf, &arg, 0);
  LRT_RCAssert(rc);
  return EBBRC_OK;
}

EBBRC
InterceptorControllerImp_Stop(InterceptorControllerRef _self)
{
  InterceptorControllerImpRef self = (InterceptorControllerImpRef)_self;
  InterceptorProxyRepRef proxyRepRef = self->proxyRepRef;
  proxyRepRef->removing = true;
  EBBId currId = proxyRepRef->curr_id;
  EBBMissFunc mf = proxyRepRef->targmf;
  EBBBindFunc bf = proxyRepRef->targbf;
  EBBArg arg = proxyRepRef->targarg;
  EBBRC rc = COBJ_EBBCALL(theEBBMgrPrimId, BindId, currId, &mf, &bf, &arg, 0);
  LRT_RCAssert(rc);
  return EBBRC_OK;
}

EBBRC
InterceptorControllerImp_Destroy(InterceptorControllerRef _self)
{
  InterceptorControllerImpRef self = (InterceptorControllerImpRef)_self;

  EBBRC rc = CObjEBBRootSharedDestroy(self->proxyRootRef);
  LRT_RCAssert(rc);

  rc = CObjEBBRootSharedDestroy(self->controllerRootRef);
  LRT_RCAssert(rc);

  rc = COBJ_EBBCALL(theEBBMgrPrimId, FreeId, self->proxyRepRef->target_id);
  LRT_RCAssert(rc);

  rc = EBBPrimFree(sizeof(InterceptorProxyRep), self->proxyRepRef);
  LRT_RCAssert(rc);

  rc = EBBPrimFree(sizeof(InterceptorControllerImp), self);
  LRT_RCAssert(rc);

  return EBBRC_OK;
}

CObjInterface(InterceptorController) InterceptorControllerImp_ftable = {
  .start = InterceptorControllerImp_Start,
  .stop = InterceptorControllerImp_Stop,
  .destroy = InterceptorControllerImp_Destroy
};

EBBRC
InterceptorControllerImp_Create(InterceptorControllerId id)
{
  //Allocate controller
  InterceptorControllerImpRef ref;
  EBBRC rc = EBBPrimMalloc(sizeof(InterceptorControllerImp), &ref,
                           EBB_MEM_GLOBAL);
  LRT_RCAssert(rc);

  //set its FT
  ref->ft = &InterceptorControllerImp_ftable;

  //Allocate proxyRep
  rc = EBBPrimMalloc(sizeof(InterceptorProxyRep), &ref->proxyRepRef,
                EBB_MEM_GLOBAL);
  LRT_RCAssert(rc);
  InterceptorProxyRepRef proxyRepRef = ref->proxyRepRef;
  proxyRepRef->ft = InterceptorProxyRep_ftable;

  //setup root for the proxy
  rc = CObjEBBRootSharedCreate(&ref->proxyRootRef, (EBBRepRef)proxyRepRef);
  LRT_RCAssert(rc);

  //allocate a new id to move the target into
  EBBId newTarg;
  rc = EBBAllocPrimId(&newTarg);
  LRT_RCAssert(rc);
  proxyRepRef->target_id = newTarg;

  //setup root for controller
  rc = CObjEBBRootSharedCreate(&ref->controllerRootRef, (EBBRepRef)ref);
  LRT_RCAssert(rc);

  CObjEBBBind((EBBId)id, ref->controllerRootRef);
  LRT_RCAssert(rc);

  return EBBRC_OK;
}

extern EBBRC interceptor_proxy_func_0(EBBRepRef);
extern EBBRC interceptor_proxy_func_1(EBBRepRef);
extern EBBRC interceptor_proxy_func_2(EBBRepRef);
extern EBBRC interceptor_proxy_func_3(EBBRepRef);
extern EBBRC interceptor_proxy_func_4(EBBRepRef);
extern EBBRC interceptor_proxy_func_5(EBBRepRef);
extern EBBRC interceptor_proxy_func_6(EBBRepRef);
extern EBBRC interceptor_proxy_func_7(EBBRepRef);
extern EBBRC interceptor_proxy_func_8(EBBRepRef);
extern EBBRC interceptor_proxy_func_9(EBBRepRef);
extern EBBRC interceptor_proxy_func_10(EBBRepRef);
extern EBBRC interceptor_proxy_func_11(EBBRepRef);
extern EBBRC interceptor_proxy_func_12(EBBRepRef);
extern EBBRC interceptor_proxy_func_13(EBBRepRef);
extern EBBRC interceptor_proxy_func_14(EBBRepRef);
extern EBBRC interceptor_proxy_func_15(EBBRepRef);
extern EBBRC interceptor_proxy_func_16(EBBRepRef);
extern EBBRC interceptor_proxy_func_17(EBBRepRef);
extern EBBRC interceptor_proxy_func_18(EBBRepRef);
extern EBBRC interceptor_proxy_func_19(EBBRepRef);
extern EBBRC interceptor_proxy_func_20(EBBRepRef);
extern EBBRC interceptor_proxy_func_21(EBBRepRef);
extern EBBRC interceptor_proxy_func_22(EBBRepRef);
extern EBBRC interceptor_proxy_func_23(EBBRepRef);
extern EBBRC interceptor_proxy_func_24(EBBRepRef);
extern EBBRC interceptor_proxy_func_25(EBBRepRef);
extern EBBRC interceptor_proxy_func_26(EBBRepRef);
extern EBBRC interceptor_proxy_func_27(EBBRepRef);
extern EBBRC interceptor_proxy_func_28(EBBRepRef);
extern EBBRC interceptor_proxy_func_29(EBBRepRef);
extern EBBRC interceptor_proxy_func_30(EBBRepRef);
extern EBBRC interceptor_proxy_func_31(EBBRepRef);
extern EBBRC interceptor_proxy_func_32(EBBRepRef);
extern EBBRC interceptor_proxy_func_33(EBBRepRef);
extern EBBRC interceptor_proxy_func_34(EBBRepRef);
extern EBBRC interceptor_proxy_func_35(EBBRepRef);
extern EBBRC interceptor_proxy_func_36(EBBRepRef);
extern EBBRC interceptor_proxy_func_37(EBBRepRef);
extern EBBRC interceptor_proxy_func_38(EBBRepRef);
extern EBBRC interceptor_proxy_func_39(EBBRepRef);
extern EBBRC interceptor_proxy_func_40(EBBRepRef);
extern EBBRC interceptor_proxy_func_41(EBBRepRef);
extern EBBRC interceptor_proxy_func_42(EBBRepRef);
extern EBBRC interceptor_proxy_func_43(EBBRepRef);
extern EBBRC interceptor_proxy_func_44(EBBRepRef);
extern EBBRC interceptor_proxy_func_45(EBBRepRef);
extern EBBRC interceptor_proxy_func_46(EBBRepRef);
extern EBBRC interceptor_proxy_func_47(EBBRepRef);
extern EBBRC interceptor_proxy_func_48(EBBRepRef);
extern EBBRC interceptor_proxy_func_49(EBBRepRef);
extern EBBRC interceptor_proxy_func_50(EBBRepRef);
extern EBBRC interceptor_proxy_func_51(EBBRepRef);
extern EBBRC interceptor_proxy_func_52(EBBRepRef);
extern EBBRC interceptor_proxy_func_53(EBBRepRef);
extern EBBRC interceptor_proxy_func_54(EBBRepRef);
extern EBBRC interceptor_proxy_func_55(EBBRepRef);
extern EBBRC interceptor_proxy_func_56(EBBRepRef);
extern EBBRC interceptor_proxy_func_57(EBBRepRef);
extern EBBRC interceptor_proxy_func_58(EBBRepRef);
extern EBBRC interceptor_proxy_func_59(EBBRepRef);
extern EBBRC interceptor_proxy_func_60(EBBRepRef);
extern EBBRC interceptor_proxy_func_61(EBBRepRef);
extern EBBRC interceptor_proxy_func_62(EBBRepRef);
extern EBBRC interceptor_proxy_func_63(EBBRepRef);
extern EBBRC interceptor_proxy_func_64(EBBRepRef);
extern EBBRC interceptor_proxy_func_65(EBBRepRef);
extern EBBRC interceptor_proxy_func_66(EBBRepRef);
extern EBBRC interceptor_proxy_func_67(EBBRepRef);
extern EBBRC interceptor_proxy_func_68(EBBRepRef);
extern EBBRC interceptor_proxy_func_69(EBBRepRef);
extern EBBRC interceptor_proxy_func_70(EBBRepRef);
extern EBBRC interceptor_proxy_func_71(EBBRepRef);
extern EBBRC interceptor_proxy_func_72(EBBRepRef);
extern EBBRC interceptor_proxy_func_73(EBBRepRef);
extern EBBRC interceptor_proxy_func_74(EBBRepRef);
extern EBBRC interceptor_proxy_func_75(EBBRepRef);
extern EBBRC interceptor_proxy_func_76(EBBRepRef);
extern EBBRC interceptor_proxy_func_77(EBBRepRef);
extern EBBRC interceptor_proxy_func_78(EBBRepRef);
extern EBBRC interceptor_proxy_func_79(EBBRepRef);
extern EBBRC interceptor_proxy_func_80(EBBRepRef);
extern EBBRC interceptor_proxy_func_81(EBBRepRef);
extern EBBRC interceptor_proxy_func_82(EBBRepRef);
extern EBBRC interceptor_proxy_func_83(EBBRepRef);
extern EBBRC interceptor_proxy_func_84(EBBRepRef);
extern EBBRC interceptor_proxy_func_85(EBBRepRef);
extern EBBRC interceptor_proxy_func_86(EBBRepRef);
extern EBBRC interceptor_proxy_func_87(EBBRepRef);
extern EBBRC interceptor_proxy_func_88(EBBRepRef);
extern EBBRC interceptor_proxy_func_89(EBBRepRef);
extern EBBRC interceptor_proxy_func_90(EBBRepRef);
extern EBBRC interceptor_proxy_func_91(EBBRepRef);
extern EBBRC interceptor_proxy_func_92(EBBRepRef);
extern EBBRC interceptor_proxy_func_93(EBBRepRef);
extern EBBRC interceptor_proxy_func_94(EBBRepRef);
extern EBBRC interceptor_proxy_func_95(EBBRepRef);
extern EBBRC interceptor_proxy_func_96(EBBRepRef);
extern EBBRC interceptor_proxy_func_97(EBBRepRef);
extern EBBRC interceptor_proxy_func_98(EBBRepRef);
extern EBBRC interceptor_proxy_func_99(EBBRepRef);
extern EBBRC interceptor_proxy_func_100(EBBRepRef);
extern EBBRC interceptor_proxy_func_101(EBBRepRef);
extern EBBRC interceptor_proxy_func_102(EBBRepRef);
extern EBBRC interceptor_proxy_func_103(EBBRepRef);
extern EBBRC interceptor_proxy_func_104(EBBRepRef);
extern EBBRC interceptor_proxy_func_105(EBBRepRef);
extern EBBRC interceptor_proxy_func_106(EBBRepRef);
extern EBBRC interceptor_proxy_func_107(EBBRepRef);
extern EBBRC interceptor_proxy_func_108(EBBRepRef);
extern EBBRC interceptor_proxy_func_109(EBBRepRef);
extern EBBRC interceptor_proxy_func_110(EBBRepRef);
extern EBBRC interceptor_proxy_func_111(EBBRepRef);
extern EBBRC interceptor_proxy_func_112(EBBRepRef);
extern EBBRC interceptor_proxy_func_113(EBBRepRef);
extern EBBRC interceptor_proxy_func_114(EBBRepRef);
extern EBBRC interceptor_proxy_func_115(EBBRepRef);
extern EBBRC interceptor_proxy_func_116(EBBRepRef);
extern EBBRC interceptor_proxy_func_117(EBBRepRef);
extern EBBRC interceptor_proxy_func_118(EBBRepRef);
extern EBBRC interceptor_proxy_func_119(EBBRepRef);
extern EBBRC interceptor_proxy_func_120(EBBRepRef);
extern EBBRC interceptor_proxy_func_121(EBBRepRef);
extern EBBRC interceptor_proxy_func_122(EBBRepRef);
extern EBBRC interceptor_proxy_func_123(EBBRepRef);
extern EBBRC interceptor_proxy_func_124(EBBRepRef);
extern EBBRC interceptor_proxy_func_125(EBBRepRef);
extern EBBRC interceptor_proxy_func_126(EBBRepRef);
extern EBBRC interceptor_proxy_func_127(EBBRepRef);
extern EBBRC interceptor_proxy_func_128(EBBRepRef);
extern EBBRC interceptor_proxy_func_129(EBBRepRef);
extern EBBRC interceptor_proxy_func_130(EBBRepRef);
extern EBBRC interceptor_proxy_func_131(EBBRepRef);
extern EBBRC interceptor_proxy_func_132(EBBRepRef);
extern EBBRC interceptor_proxy_func_133(EBBRepRef);
extern EBBRC interceptor_proxy_func_134(EBBRepRef);
extern EBBRC interceptor_proxy_func_135(EBBRepRef);
extern EBBRC interceptor_proxy_func_136(EBBRepRef);
extern EBBRC interceptor_proxy_func_137(EBBRepRef);
extern EBBRC interceptor_proxy_func_138(EBBRepRef);
extern EBBRC interceptor_proxy_func_139(EBBRepRef);
extern EBBRC interceptor_proxy_func_140(EBBRepRef);
extern EBBRC interceptor_proxy_func_141(EBBRepRef);
extern EBBRC interceptor_proxy_func_142(EBBRepRef);
extern EBBRC interceptor_proxy_func_143(EBBRepRef);
extern EBBRC interceptor_proxy_func_144(EBBRepRef);
extern EBBRC interceptor_proxy_func_145(EBBRepRef);
extern EBBRC interceptor_proxy_func_146(EBBRepRef);
extern EBBRC interceptor_proxy_func_147(EBBRepRef);
extern EBBRC interceptor_proxy_func_148(EBBRepRef);
extern EBBRC interceptor_proxy_func_149(EBBRepRef);
extern EBBRC interceptor_proxy_func_150(EBBRepRef);
extern EBBRC interceptor_proxy_func_151(EBBRepRef);
extern EBBRC interceptor_proxy_func_152(EBBRepRef);
extern EBBRC interceptor_proxy_func_153(EBBRepRef);
extern EBBRC interceptor_proxy_func_154(EBBRepRef);
extern EBBRC interceptor_proxy_func_155(EBBRepRef);
extern EBBRC interceptor_proxy_func_156(EBBRepRef);
extern EBBRC interceptor_proxy_func_157(EBBRepRef);
extern EBBRC interceptor_proxy_func_158(EBBRepRef);
extern EBBRC interceptor_proxy_func_159(EBBRepRef);
extern EBBRC interceptor_proxy_func_160(EBBRepRef);
extern EBBRC interceptor_proxy_func_161(EBBRepRef);
extern EBBRC interceptor_proxy_func_162(EBBRepRef);
extern EBBRC interceptor_proxy_func_163(EBBRepRef);
extern EBBRC interceptor_proxy_func_164(EBBRepRef);
extern EBBRC interceptor_proxy_func_165(EBBRepRef);
extern EBBRC interceptor_proxy_func_166(EBBRepRef);
extern EBBRC interceptor_proxy_func_167(EBBRepRef);
extern EBBRC interceptor_proxy_func_168(EBBRepRef);
extern EBBRC interceptor_proxy_func_169(EBBRepRef);
extern EBBRC interceptor_proxy_func_170(EBBRepRef);
extern EBBRC interceptor_proxy_func_171(EBBRepRef);
extern EBBRC interceptor_proxy_func_172(EBBRepRef);
extern EBBRC interceptor_proxy_func_173(EBBRepRef);
extern EBBRC interceptor_proxy_func_174(EBBRepRef);
extern EBBRC interceptor_proxy_func_175(EBBRepRef);
extern EBBRC interceptor_proxy_func_176(EBBRepRef);
extern EBBRC interceptor_proxy_func_177(EBBRepRef);
extern EBBRC interceptor_proxy_func_178(EBBRepRef);
extern EBBRC interceptor_proxy_func_179(EBBRepRef);
extern EBBRC interceptor_proxy_func_180(EBBRepRef);
extern EBBRC interceptor_proxy_func_181(EBBRepRef);
extern EBBRC interceptor_proxy_func_182(EBBRepRef);
extern EBBRC interceptor_proxy_func_183(EBBRepRef);
extern EBBRC interceptor_proxy_func_184(EBBRepRef);
extern EBBRC interceptor_proxy_func_185(EBBRepRef);
extern EBBRC interceptor_proxy_func_186(EBBRepRef);
extern EBBRC interceptor_proxy_func_187(EBBRepRef);
extern EBBRC interceptor_proxy_func_188(EBBRepRef);
extern EBBRC interceptor_proxy_func_189(EBBRepRef);
extern EBBRC interceptor_proxy_func_190(EBBRepRef);
extern EBBRC interceptor_proxy_func_191(EBBRepRef);
extern EBBRC interceptor_proxy_func_192(EBBRepRef);
extern EBBRC interceptor_proxy_func_193(EBBRepRef);
extern EBBRC interceptor_proxy_func_194(EBBRepRef);
extern EBBRC interceptor_proxy_func_195(EBBRepRef);
extern EBBRC interceptor_proxy_func_196(EBBRepRef);
extern EBBRC interceptor_proxy_func_197(EBBRepRef);
extern EBBRC interceptor_proxy_func_198(EBBRepRef);
extern EBBRC interceptor_proxy_func_199(EBBRepRef);
extern EBBRC interceptor_proxy_func_200(EBBRepRef);
extern EBBRC interceptor_proxy_func_201(EBBRepRef);
extern EBBRC interceptor_proxy_func_202(EBBRepRef);
extern EBBRC interceptor_proxy_func_203(EBBRepRef);
extern EBBRC interceptor_proxy_func_204(EBBRepRef);
extern EBBRC interceptor_proxy_func_205(EBBRepRef);
extern EBBRC interceptor_proxy_func_206(EBBRepRef);
extern EBBRC interceptor_proxy_func_207(EBBRepRef);
extern EBBRC interceptor_proxy_func_208(EBBRepRef);
extern EBBRC interceptor_proxy_func_209(EBBRepRef);
extern EBBRC interceptor_proxy_func_210(EBBRepRef);
extern EBBRC interceptor_proxy_func_211(EBBRepRef);
extern EBBRC interceptor_proxy_func_212(EBBRepRef);
extern EBBRC interceptor_proxy_func_213(EBBRepRef);
extern EBBRC interceptor_proxy_func_214(EBBRepRef);
extern EBBRC interceptor_proxy_func_215(EBBRepRef);
extern EBBRC interceptor_proxy_func_216(EBBRepRef);
extern EBBRC interceptor_proxy_func_217(EBBRepRef);
extern EBBRC interceptor_proxy_func_218(EBBRepRef);
extern EBBRC interceptor_proxy_func_219(EBBRepRef);
extern EBBRC interceptor_proxy_func_220(EBBRepRef);
extern EBBRC interceptor_proxy_func_221(EBBRepRef);
extern EBBRC interceptor_proxy_func_222(EBBRepRef);
extern EBBRC interceptor_proxy_func_223(EBBRepRef);
extern EBBRC interceptor_proxy_func_224(EBBRepRef);
extern EBBRC interceptor_proxy_func_225(EBBRepRef);
extern EBBRC interceptor_proxy_func_226(EBBRepRef);
extern EBBRC interceptor_proxy_func_227(EBBRepRef);
extern EBBRC interceptor_proxy_func_228(EBBRepRef);
extern EBBRC interceptor_proxy_func_229(EBBRepRef);
extern EBBRC interceptor_proxy_func_230(EBBRepRef);
extern EBBRC interceptor_proxy_func_231(EBBRepRef);
extern EBBRC interceptor_proxy_func_232(EBBRepRef);
extern EBBRC interceptor_proxy_func_233(EBBRepRef);
extern EBBRC interceptor_proxy_func_234(EBBRepRef);
extern EBBRC interceptor_proxy_func_235(EBBRepRef);
extern EBBRC interceptor_proxy_func_236(EBBRepRef);
extern EBBRC interceptor_proxy_func_237(EBBRepRef);
extern EBBRC interceptor_proxy_func_238(EBBRepRef);
extern EBBRC interceptor_proxy_func_239(EBBRepRef);
extern EBBRC interceptor_proxy_func_240(EBBRepRef);
extern EBBRC interceptor_proxy_func_241(EBBRepRef);
extern EBBRC interceptor_proxy_func_242(EBBRepRef);
extern EBBRC interceptor_proxy_func_243(EBBRepRef);
extern EBBRC interceptor_proxy_func_244(EBBRepRef);
extern EBBRC interceptor_proxy_func_245(EBBRepRef);
extern EBBRC interceptor_proxy_func_246(EBBRepRef);
extern EBBRC interceptor_proxy_func_247(EBBRepRef);
extern EBBRC interceptor_proxy_func_248(EBBRepRef);
extern EBBRC interceptor_proxy_func_249(EBBRepRef);
extern EBBRC interceptor_proxy_func_250(EBBRepRef);
extern EBBRC interceptor_proxy_func_251(EBBRepRef);
extern EBBRC interceptor_proxy_func_252(EBBRepRef);
extern EBBRC interceptor_proxy_func_253(EBBRepRef);
extern EBBRC interceptor_proxy_func_254(EBBRepRef);
extern EBBRC interceptor_proxy_func_255(EBBRepRef);

static EBBFunc InterceptorProxyRep_ftable[] = {
  interceptor_proxy_func_0,
  interceptor_proxy_func_1,
  interceptor_proxy_func_2,
  interceptor_proxy_func_3,
  interceptor_proxy_func_4,
  interceptor_proxy_func_5,
  interceptor_proxy_func_6,
  interceptor_proxy_func_7,
  interceptor_proxy_func_8,
  interceptor_proxy_func_9,
  interceptor_proxy_func_10,
  interceptor_proxy_func_11,
  interceptor_proxy_func_12,
  interceptor_proxy_func_13,
  interceptor_proxy_func_14,
  interceptor_proxy_func_15,
  interceptor_proxy_func_16,
  interceptor_proxy_func_17,
  interceptor_proxy_func_18,
  interceptor_proxy_func_19,
  interceptor_proxy_func_20,
  interceptor_proxy_func_21,
  interceptor_proxy_func_22,
  interceptor_proxy_func_23,
  interceptor_proxy_func_24,
  interceptor_proxy_func_25,
  interceptor_proxy_func_26,
  interceptor_proxy_func_27,
  interceptor_proxy_func_28,
  interceptor_proxy_func_29,
  interceptor_proxy_func_30,
  interceptor_proxy_func_31,
  interceptor_proxy_func_32,
  interceptor_proxy_func_33,
  interceptor_proxy_func_34,
  interceptor_proxy_func_35,
  interceptor_proxy_func_36,
  interceptor_proxy_func_37,
  interceptor_proxy_func_38,
  interceptor_proxy_func_39,
  interceptor_proxy_func_40,
  interceptor_proxy_func_41,
  interceptor_proxy_func_42,
  interceptor_proxy_func_43,
  interceptor_proxy_func_44,
  interceptor_proxy_func_45,
  interceptor_proxy_func_46,
  interceptor_proxy_func_47,
  interceptor_proxy_func_48,
  interceptor_proxy_func_49,
  interceptor_proxy_func_50,
  interceptor_proxy_func_51,
  interceptor_proxy_func_52,
  interceptor_proxy_func_53,
  interceptor_proxy_func_54,
  interceptor_proxy_func_55,
  interceptor_proxy_func_56,
  interceptor_proxy_func_57,
  interceptor_proxy_func_58,
  interceptor_proxy_func_59,
  interceptor_proxy_func_60,
  interceptor_proxy_func_61,
  interceptor_proxy_func_62,
  interceptor_proxy_func_63,
  interceptor_proxy_func_64,
  interceptor_proxy_func_65,
  interceptor_proxy_func_66,
  interceptor_proxy_func_67,
  interceptor_proxy_func_68,
  interceptor_proxy_func_69,
  interceptor_proxy_func_70,
  interceptor_proxy_func_71,
  interceptor_proxy_func_72,
  interceptor_proxy_func_73,
  interceptor_proxy_func_74,
  interceptor_proxy_func_75,
  interceptor_proxy_func_76,
  interceptor_proxy_func_77,
  interceptor_proxy_func_78,
  interceptor_proxy_func_79,
  interceptor_proxy_func_80,
  interceptor_proxy_func_81,
  interceptor_proxy_func_82,
  interceptor_proxy_func_83,
  interceptor_proxy_func_84,
  interceptor_proxy_func_85,
  interceptor_proxy_func_86,
  interceptor_proxy_func_87,
  interceptor_proxy_func_88,
  interceptor_proxy_func_89,
  interceptor_proxy_func_90,
  interceptor_proxy_func_91,
  interceptor_proxy_func_92,
  interceptor_proxy_func_93,
  interceptor_proxy_func_94,
  interceptor_proxy_func_95,
  interceptor_proxy_func_96,
  interceptor_proxy_func_97,
  interceptor_proxy_func_98,
  interceptor_proxy_func_99,
  interceptor_proxy_func_100,
  interceptor_proxy_func_101,
  interceptor_proxy_func_102,
  interceptor_proxy_func_103,
  interceptor_proxy_func_104,
  interceptor_proxy_func_105,
  interceptor_proxy_func_106,
  interceptor_proxy_func_107,
  interceptor_proxy_func_108,
  interceptor_proxy_func_109,
  interceptor_proxy_func_110,
  interceptor_proxy_func_111,
  interceptor_proxy_func_112,
  interceptor_proxy_func_113,
  interceptor_proxy_func_114,
  interceptor_proxy_func_115,
  interceptor_proxy_func_116,
  interceptor_proxy_func_117,
  interceptor_proxy_func_118,
  interceptor_proxy_func_119,
  interceptor_proxy_func_120,
  interceptor_proxy_func_121,
  interceptor_proxy_func_122,
  interceptor_proxy_func_123,
  interceptor_proxy_func_124,
  interceptor_proxy_func_125,
  interceptor_proxy_func_126,
  interceptor_proxy_func_127,
  interceptor_proxy_func_128,
  interceptor_proxy_func_129,
  interceptor_proxy_func_130,
  interceptor_proxy_func_131,
  interceptor_proxy_func_132,
  interceptor_proxy_func_133,
  interceptor_proxy_func_134,
  interceptor_proxy_func_135,
  interceptor_proxy_func_136,
  interceptor_proxy_func_137,
  interceptor_proxy_func_138,
  interceptor_proxy_func_139,
  interceptor_proxy_func_140,
  interceptor_proxy_func_141,
  interceptor_proxy_func_142,
  interceptor_proxy_func_143,
  interceptor_proxy_func_144,
  interceptor_proxy_func_145,
  interceptor_proxy_func_146,
  interceptor_proxy_func_147,
  interceptor_proxy_func_148,
  interceptor_proxy_func_149,
  interceptor_proxy_func_150,
  interceptor_proxy_func_151,
  interceptor_proxy_func_152,
  interceptor_proxy_func_153,
  interceptor_proxy_func_154,
  interceptor_proxy_func_155,
  interceptor_proxy_func_156,
  interceptor_proxy_func_157,
  interceptor_proxy_func_158,
  interceptor_proxy_func_159,
  interceptor_proxy_func_160,
  interceptor_proxy_func_161,
  interceptor_proxy_func_162,
  interceptor_proxy_func_163,
  interceptor_proxy_func_164,
  interceptor_proxy_func_165,
  interceptor_proxy_func_166,
  interceptor_proxy_func_167,
  interceptor_proxy_func_168,
  interceptor_proxy_func_169,
  interceptor_proxy_func_170,
  interceptor_proxy_func_171,
  interceptor_proxy_func_172,
  interceptor_proxy_func_173,
  interceptor_proxy_func_174,
  interceptor_proxy_func_175,
  interceptor_proxy_func_176,
  interceptor_proxy_func_177,
  interceptor_proxy_func_178,
  interceptor_proxy_func_179,
  interceptor_proxy_func_180,
  interceptor_proxy_func_181,
  interceptor_proxy_func_182,
  interceptor_proxy_func_183,
  interceptor_proxy_func_184,
  interceptor_proxy_func_185,
  interceptor_proxy_func_186,
  interceptor_proxy_func_187,
  interceptor_proxy_func_188,
  interceptor_proxy_func_189,
  interceptor_proxy_func_190,
  interceptor_proxy_func_191,
  interceptor_proxy_func_192,
  interceptor_proxy_func_193,
  interceptor_proxy_func_194,
  interceptor_proxy_func_195,
  interceptor_proxy_func_196,
  interceptor_proxy_func_197,
  interceptor_proxy_func_198,
  interceptor_proxy_func_199,
  interceptor_proxy_func_200,
  interceptor_proxy_func_201,
  interceptor_proxy_func_202,
  interceptor_proxy_func_203,
  interceptor_proxy_func_204,
  interceptor_proxy_func_205,
  interceptor_proxy_func_206,
  interceptor_proxy_func_207,
  interceptor_proxy_func_208,
  interceptor_proxy_func_209,
  interceptor_proxy_func_210,
  interceptor_proxy_func_211,
  interceptor_proxy_func_212,
  interceptor_proxy_func_213,
  interceptor_proxy_func_214,
  interceptor_proxy_func_215,
  interceptor_proxy_func_216,
  interceptor_proxy_func_217,
  interceptor_proxy_func_218,
  interceptor_proxy_func_219,
  interceptor_proxy_func_220,
  interceptor_proxy_func_221,
  interceptor_proxy_func_222,
  interceptor_proxy_func_223,
  interceptor_proxy_func_224,
  interceptor_proxy_func_225,
  interceptor_proxy_func_226,
  interceptor_proxy_func_227,
  interceptor_proxy_func_228,
  interceptor_proxy_func_229,
  interceptor_proxy_func_230,
  interceptor_proxy_func_231,
  interceptor_proxy_func_232,
  interceptor_proxy_func_233,
  interceptor_proxy_func_234,
  interceptor_proxy_func_235,
  interceptor_proxy_func_236,
  interceptor_proxy_func_237,
  interceptor_proxy_func_238,
  interceptor_proxy_func_239,
  interceptor_proxy_func_240,
  interceptor_proxy_func_241,
  interceptor_proxy_func_242,
  interceptor_proxy_func_243,
  interceptor_proxy_func_244,
  interceptor_proxy_func_245,
  interceptor_proxy_func_246,
  interceptor_proxy_func_247,
  interceptor_proxy_func_248,
  interceptor_proxy_func_249,
  interceptor_proxy_func_250,
  interceptor_proxy_func_251,
  interceptor_proxy_func_252,
  interceptor_proxy_func_253,
  interceptor_proxy_func_254,
  interceptor_proxy_func_255
};
