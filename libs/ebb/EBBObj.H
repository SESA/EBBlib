#ifndef __EBB_OBJ_H__
#define __EBB_OBJ_H__

class EBBBase {
protected:
  virtual ~EBBBase() {}
};

class EBBRoot : public EBBBase {
protected:
  EBBId id;
public:
  EBBId getId() { return id; }
  virtual EBBRC handlemiss();
};

class EBBRep : public EBBBase {
protected:
  EBBRoot *myRoot;
public:
  EBBRep() : myRoot(0) { /* empty body */ }
  EBBRep(EBBRoot *root) : myRoot(root) { /* empty body */ }
  
  // FIXME: These calls should not really be public
  void setRoot(EBBRoot *root) { myRoot = root; }
  // FIXME: If the poor perf of a method call here is a problem
  //        we then need to consider caching the ref in the rep
  //        and returning it directly with out call the root.
  EBBId getId() { return myRoot->getId(); }
  
  // cleanup should only be called by a root
  // FIXME:  unfortunately there is no nice way to express this
  virtual EBBRC cleanup()  { delete this; return EBBRC_OK; }
};


#endif 
