#include "../base/include.h"
#include "../base/types.h"
#include "EBBTypes.H"
#include "EBBObj.H"

class IntegerCounter : public EBBRep {
public:
  virtual EBBRC inc()=0;
  virtual EBBRC dec()=0;
  virtual EBBRC value(sval &count)=0;
};
typedef IntegerCounter **IntegerCounterId;

class SharedPrimIntegerCounter : public IntegerCounter {
protected:
    sval _value;
public:
    SharedPrimIntegerCounter(sval *count) 
    : _value(0) {
      /* empty body */
    }
    virtual EBBRC inc();
    virtual EBBRC dec();
    virtual EBBRC value(sval &count);
    static  EBBRC Create(IntegerCounterId &id) {
      //      id=(IntegerCounterId)
      //	(CObjRootSingleRep::Create(new SharedIntegerCounter(count)));
      return EBBRC_OK;
    }
};


