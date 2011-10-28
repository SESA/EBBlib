#ifndef __ETH_TYPE_MGR_H__
#define __ETH_TYPE_MGR_H__

CObjInterface(EthTypeMgr) {
  EBBRC (*handlePacket)(void *_self);
};

CObject(EthTypeMgr) {
  CObjInterface(EthTypeMgr) *ft;
};

typedef EthTypeMgr *EthTypeMgrId;

#endif  // __ETH_TYPE_MGR_H__
