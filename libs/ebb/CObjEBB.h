#ifndef __COBJ_EBB_H__
#define __COBJ_EBB_H__

extern EBBRC CObjEBBMissFunc(void *, EBBLTrans *, FuncNum, EBBMissArg);

#define EB(ID) EBBId_DREF(ID)
#define EC(ID) EB(ID)->ft

#define COBJ_EBBCALL(id, method, ...) (EC(id)->method(EB(id), ##__VA_ARGS__))

#endif
