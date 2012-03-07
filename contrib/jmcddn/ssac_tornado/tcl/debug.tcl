set ACTIVEGLP("NOCPU") 0
set ACTIVEREALPD("NOCPU") 0
set ACTIVEVP("NOCPU") 0
set ACTIVEPORTID("NOCPU") 0
set ACTIVEPROC("NOCPU") 0  

set SYM_ACTIVEGLP    "kernel::(($GLOCAL)->activePDA)->glp"
set SYM_ACTIVEREALPD "kernel::(($GLOCAL)->activePDA)->realPD"
set SYM_ACTIVEVP     "kernel::(($GLOCAL)->activePDA)->vp"
set SYM_ACTIVEPORTID "kernel::(($GLOCAL)->activePDA)->portId"
set SYM_ACTIVEPROC   "kernel::(($GLOCAL)->activePDA)->proc"

annotation set pc kernel::PostConsInitStuffHack__Fi:END {
    set ACTIVEGLP($CPU) [symbol read $SYM_ACTIVEGLP]
    set ACTIVEREALPD($CPU) [symbol read $SYM_ACTIVEREALPD]
    set ACTIVEVP($CPU) [symbol read $SYM_ACTIVEVP]
    set ACTIVEPORTID($CPU) [symbol read $SYM_ACTIVEPORTID]
    set ACTIVEPROC($CPU) [symbol read $SYM_ACTIVEPROC]
}

annotation set inst rfe -tag osAnnRfe {
    set ACTIVEGLP($CPU) [symbol read $SYM_ACTIVEGLP]
    set ACTIVEREALPD($CPU) [symbol read $SYM_ACTIVEREALPD]
    set ACTIVEVP($CPU) [symbol read $SYM_ACTIVEVP]
    set ACTIVEPORTID($CPU) [symbol read $SYM_ACTIVEPORTID]
    set ACTIVEPROC($CPU) [symbol read $SYM_ACTIVEPROC]
}
