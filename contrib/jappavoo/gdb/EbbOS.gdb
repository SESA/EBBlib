set output-radix 16

# pass SIGINT to EbbOS process
handle SIGINT nostop pass

define ID2GT
#  p *((struct lrt_trans *)lrt_trans_id2gt($arg0))
  p *((EBBGTrans *)lrt_trans_id2gt($arg0))
end
document ID2GT
ID2GT <id>: prints the global translation entry associated with <id>
end

define IDMF
  p ((EBBGTrans *)lrt_trans_id2gt($arg0))->mf
end
document IDMF
IDMF <id>: prints the miss function bound to <id>
end

define IDARG
  p ((EBBGTrans *)lrt_trans_id2gt($arg0))->arg
end
document IDARG
IDARG <id>: prints the miss argument bound to <id>
end

define IDROOT
   p *(((CObjEBBRootRef)(((EBBGTrans *)lrt_trans_id2gt($arg0))->arg))->ft)
end

define IDSROOT
   p *((CObjEBBRootSharedImpRef)(((EBBGTrans *)lrt_trans_id2gt($arg0))->arg))
end

define IDMROOT
   p *((CObjEBBRootMultiImpRef)(((EBBGTrans *)lrt_trans_id2gt($arg0))->arg))
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
# 
  ptype struct lrt_trans	
end
document gdbEbbOS
gdbEbbOS: call this first to preload some symbols that will make your
          life easier
end