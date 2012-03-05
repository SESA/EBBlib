set output-radix 16
set print pretty on

# define hook-stop
# disable breakpoints
# end

# define hook-run
# enable breakpoints
# end

# define hook-continue
# enable breakpoints
# end


# pass SIGINT to EbbOS process
handle SIGINT nostop pass

define ID2GT
  p ((EBBGTrans *)lrt_trans_id2gt($arg0))
end
document ID2GT
ID2GT <id>: prints the global translation entry associated with <id>
end

define GTMF
  p ((EBBGTrans *)$arg0)->mf
end
document GTMF
GTMF <gt>: print the missfunction assoicated with the <gt>
end

define IDMF
  ID2GT $arg0
  GTMF $
end
document IDMF
IDMF <id>: prints the miss function bound to <id>
end

define GTARG
  p ((EBBGTrans *)$arg0)->arg
end
document GTARG
GTARG <gt>: print the arg field of the gt 
end

define IDARG
  ID2GT $arg0
  GTARG $
end
document IDARG
IDARG <id>: prints the miss argument bound to <id>
end

define GTROOT
  GTARG $arg0
  p *(((CObjEBBRootRef)($))->ft) 
end
document GTROOT
GTROOT <gt>: print function table to identify the root of the <gt>
end

define IDROOT
  ID2GT $arg0
  GTROOT $  
end
document IDROOT
IDROOT <id>: print function table to identify the root of the <id>   
end

define GTSROOT
   GTARG $arg0
   p *((CObjEBBRootSharedImpRef)($))
end
document GTSROOT
GTSROOT <gt>: assume the gt is to a shared EBB then print it out the root as one
end

define IDSROOT
  ID2GT $arg0
  GTSROOT $
end
document IDSROOT
IDSROOT <id>: assume the id is to a shared EBB the print out the root as one
end

define GTMROOT
   GTARG $arg0
   p *((CObjEBBRootMultiImpRef)($))
end
document GTMROOT
GTMROOT <gt>: assume the gt is to a multi EBB then print it out as one
end
define IDMROOT
   ID2GT $arg0
   GTMROOT $
end
document IDMROOT
IDMROOT <id>: assume the id is to a multi EBB then print it out as one
end

define decodegt
  set $gt=$arg0
#FIXME:  Add more conditions eg tests for free and more tests on mf
  GTROOT $gt
  set $mf = $
  if ($mf)->handleMiss == $sharedMF
    GTSROOT $gt
  else 
    if ($mf)->handleMiss == $multiMF
      GTMROOT $gt
    end
  end
end
document decodegt
decodegt <gt>: try and intelligently decode the gt
end

define cobj
  set $f=((EBBFunc *)(*(EBBRepRef)$arg0))
  set $i=0 
  set $n=1

  if $argc == 2
    set $n=$arg1
  end

  while $i<$n
    p $f[$i]
    set $i=$i+1
  end
end
document cobj
funcs <cobj> [n=1]: takes a pointer to an cobj and prints n of the funcs of its interface
end

define ebb
  ID2GT $arg0
  decodegt $
end
document ebb
ebb <id>:  takes and id and attempts to decode it and provide info about it
end
  
define blrt
  break lrt_start
end
define clrt
  clear lrt_start
end
document BLRT
blrt: put a breakpoint on lrt startup
end

define bl0
  break l0_start
end
define cl0
  clear l0_start
end
document bl0
bl0: put a breakpoint on L0 start up
end

define bl1
  break L1Prim_start
end
define cl1
  clear L1Prim_start
end
document bl1
bl1: put a breakpoint on L1 start up
end

define bstartMsg
  break L1Prim_MsgHandler_startMH
end
define cstartMsg
  clear L1Prim_MsgHandler_startMH
end
document bstartMsg
bstartMsg: put a breakpoint on the beginning of the startup msg
end

define bstartup
  blrt
  bl0
  bl1
  bstartMsg
end
define cstartup
  clrt
  cl0
  cl1
  cstartMsg
end
document bstartup
bstartup: put breakpoints on key points along startup
end

define EbbOS
    if &TransMem != 0
      #this is a kludge figure out a better way
      ptype struct lrt_trans
      set $multiMF=CObjEBBRootMulti_handleMiss	
      set $sharedMF=CObjEBBRootSharedImp_handleMiss
      set prompt (EbbOS)\040
    end
end
document EbbOS
EbbOS: EbbOS debugging
end

define hookpost-file
  EbbOS
end

EbbOS

