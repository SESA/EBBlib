#ifndef __EBB_CTR_H__
#define __EBB_CTR_H__

CObject(EBBCtr);

CObjInterface(EBBCtr) {
  EBBRC (*inc)  (EBBCtrRef _self);
  EBBRC (*dec)  (EBBCtrRef _self);
  EBBRC (*val)  (EBBCtrRef _self, uval *v);
};

CObjectDefine(EBBCtr) {
  CObjInterface(EBBCtr) *ft;
};

typedef EBBCtrRef *EBBCtrId;

#endif
