set output-radix 16

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

define EBB
  ID2GT $arg0
  set $gt=$
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

define gdbEbbOS
# this is a kludge figure out what symbols are really worth getting
  ptype struct lrt_trans
  set $multiMF=CObjEBBRootMulti_handleMiss	
  set $sharedMF=CObjEBBRootSharedImp_handleMiss
end
document gdbEbbOS
gdbEbbOS: call this first to preload some symbols that will make your
          life easier
end

define hookpost-file
  gdbEbbOS
end