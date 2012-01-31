annotation set pc kernel::worker__FPP4SSACP7requesti:START {
    annotation exec tstEvent startworker
}

annotation set pc kernel::worker__FPP4SSACP7requesti:END {
    annotation exec tstEvent endworker
}

annotation set pc kernel::driver__Fv:START {
    annotation exec tstEvent starttest
    annotation exec tstEvent startdriver
}

annotation set pc kernel::driver__Fv:END {
    annotation exec tstEvent enddriver
}

annotation set pc kernel::test__Fv:END {
    annotation exec tstEvent endtest
}

annotation set simos enter {
    annotation disable functrace
}

annotation set tstEvent startworker {
    annotation enable functrace
}

set NUMOFSLEEPS 0

annotation set pc kernel::sleep__16CacheEntrySimpleFv:START -tag functrace {
    set NUMOFSLEEPS [expr $NUMOFSLEEPS + 1]
}

set NUMOFWAKEUPS 0

annotation set pc kernel::wakeup__16CacheEntrySimpleFv:START -tag functrace {
    set NUMOFWAKEUPS [expr $NUMOFWAKEUPS + 1]
}

set NUMOFLRUENTRIES 0

annotation set pc kernel::lruentry__Q2_21SSACSimpleSharedArray10HashQueuesFRCi:START -tag functrace {
    set NUMOFLRUENTRIES [expr $NUMOFLRUENTRIES + 1]
}

set NUMOFDIRTYS 0

annotation set pc kernel::dirty__16CacheEntrySimpleFv:START -tag functrace {
    set NUMOFDIRTYS [expr $NUMOFDIRTYS + 1]
}

annotation set pc kernel::rollover__Q2_21SSACSimpleSharedArray10HashQueuesFv:START {
    log "**** ERROR:  SSACSimpleSharedArray::HashQueues::rollover called\n"
}

#set NUMOFFREEREMOVES 0

#annotation set pc kernel::free_remove__Q2_21SSACSimpleSharedArray10HashQueuesFP16CacheEntrySimple:START {
#    set NUMOFFREEREMOVES [expr $NUMOFFREEREMOVES + 1]
#}


#set NUMOFFREEADDATHEADS 0

#annotation set pc kernel::free_addathead__Q2_21SSACSimpleSharedArray10HashQueuesFP16CacheEntrySimple:START {
#    set NUMOFFREEADDATHEADS [expr $NUMOFFREEADDATHEADS + 1]
#}

#set NUMOFFREEADDATTAILS 0

#annotation set pc kernel::free_addattail__Q2_21SSACSimpleSharedArray10HashQueuesFP16CacheEntrySimple:START {
#    set NUMOFFREEADDATTAILS [expr $NUMOFFREEADDATTAILS + 1]
#}

set NUMOFGETS 0

annotation set pc kernel::get__21SSACSimpleSharedArrayFR13CacheObjectIdRP10CacheEntryRCQ2_4SSAC7gettype:START -tag functrace {
#    console "***** get a0=$a0 a1=$a1\n"
    set NUMOFGETS [expr $NUMOFGETS + 1]
}

set NUMOFPUTBACKS 0

annotation set pc kernel::putback__21SSACSimpleSharedArrayFRP10CacheEntryRCQ2_4SSAC7putflag:START -tag functrace {
    set NUMOFPUTBACKS [expr $NUMOFPUTBACKS + 1]
}

set NUMOFLOADS 0

annotation set pc kernel::load__19CacheObjectIdSimpleFv:START -tag functrace {
#    console "**** DEBUG: load a0=$a0\n"
    set NUMOFLOADS [expr $NUMOFLOADS + 1]
}

set NUMOFSAVES 0

annotation set pc kernel::save__19CacheObjectIdSimpleFPv:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFSAVES [expr $NUMOFSAVES + 1]
}

#annotation set pc kernel::handleMiss__8MHSharedFP10TransEntryP8MHOEntryi {
#    console "****** MHShared::handleMiss called\n";
#}

#annotation set tstEvent startworker {
#    set DOSOMETHINGCOUNT($ACTIVEPDA($CPU)) 0
#}

#annotation set tstEvent endworker {
#    log "**** STATS: Worker_${CPU}_$ACTIVEPDA($CPU): Total executed dosomethings()=$DOSOMETHINGCOUNT($ACTIVEPDA($CPU))\n"
#}

#annotation set pc kernel::dosomethingwithentry__FP16CacheEntrySimple {
#    set DOSOMETHINGCOUNT($ACTIVEPDA($CPU)) [expr $DOSOMETHINGCOUNT($ACTIVEPDA($CPU)) + 1]
#}


annotation set simos exit {
    log "**** STATS: Number of times executed CacheEntrySimple:: sleep()=$NUMOFSLEEPS\n"
    log "**** STATS: Number of times executed CacheEntrySimple:: wakeup()=$NUMOFWAKEUPS\n"
    log "**** STATS: Number of times executed CacheEntrySimple:: dirty()=$NUMOFDIRTYS\n"
    log "**** STATS: Number of times executed SSACSimpleSharedArray::HashQueues:: lruentry()=$NUMOFLRUENTRIES\n"
#    log "**** STATS: Number of times executed SSACSimpleSharedArray::HashQueues:: free_addathead()=$NUMOFFREEADDATHEADS\n"
#    log "**** STATS: Number of times executed SSACSimpleSharedArray::HashQueues:: free_addattail()=$NUMOFFREEADDATTAILS\n"
    log "**** STATS: Number of times executed SSACSimpleSharedArray:: get()=$NUMOFGETS\n"
    log "**** STATS: Number of times executed SSACSimpleSharedArray:: putback()=$NUMOFPUTBACKS\n"
    log "**** STATS: Number of times executed CacheObjectIdSimple:: load=$NUMOFLOADS\n"
    log "**** STATS: Number of times executed CacheObjectIdSimple:: save=$NUMOFSAVES\n"
}


#annotation set simos enter {
#    annotation disable stimer
#}

#annotation set osEvent endBoot {
#    annotation enable stimer
#}    

#annotation set store  0xa8000000400100f0 {
#    log "*** DEBUG: store to kernel::(($GLOCAL)->theTimer) at $pc value=$MEMORY(0xa8000000400100f0)$MEMORY(0xa8000000400100f4)\n"
#    console "*** DEBUG: store to kernel::(($GLOCAL)->theTimer) at $pc value=$MEMORY(0xa8000000400100f0)$MEMORY(0xa8000000400100f4)\n"
#}

#annotation set store  0xffffffff400100f0 {
#    log "*** DEBUG: *store to kernel::(($GLOCAL)->theTimer) at $pc value=$MEMORY(0xffffffff400100f0)$MEMORY(0xa400100f4)\n"
#    console "*** DEBUG: *store to kernel::(($GLOCAL)->theTimer) at $pc value=$MEMORY(0xaffffffff400100f0)$MEMORY(0xa8000000400100f4)\n"
#}

#annotation set pc kernel::now__5TimerFv:START {
#    log "**** DEBUG: at the start of Timer::now() this=a0=$a0 value=$MEMORY(0xa8000000400100f0)$MEMORY(0xa8000000400100f4)\n"
#    console "**** DEBUG: at the start of Timer::now() this=a0=$a0 value=$MEMORY(0xa8000000400100f0)$MEMORY(0xa8000000400100f4)\n"
#}

#source "debug.tcl"

#annotation set pc kernel::startworker__FPP4SSACP7requesti:START {
#     console "***** startworker invoked on CPU=$CPU\n"
#    console "**** DEBUG:startworker:START:CPU=$CPU: glp=$ACTIVEGLP($CPU) realPD=$ACTIVEREALPD($CPU) vp=$ACTIVEVP($CPU) portid=$ACTIVEPORTID($CPU) proc=$ACTIVEPROC($CPU)\n"
#    if { $CPU == 0 } {
#	console "**** DEBUG: BREAK: startworker__FPPSSACPii:START \tpc $pc\n"
#	debug
#    }
#}

#annotation set pc kernel::startworker__FPP4SSACP7requesti:START {
#    console "***** startworker Finished on CPU=$CPU\n"
#}

#annotation set tstEvent startworker {
#    set ICOUNTS($CPU) 0
#    console "  ***** Worker started on $CPU\n"
#}

#annotation set tstEvent endworker {
#    set ICOUNTS($CPU) 0
#    console "  ***** Worker finished on $CPU\n"
#}

#annotation set pc 0xffffffff80026f00 {
#    console "**** break pc=$pc in worker\n"
#    debug
#}

#annotation set pc 0xffffffff80026f64 {
#    log "**** DEBUGTRACE: CPU=$CPU i=$ICOUNTS($CPU)\n"
#    set ICOUNTS($CPU) [expr $ICOUNTS($CPU) + 1]
#}

#annotation set pc kernel::HandleGenInterrupt:START {
#   console "  **** HandleGenInterrupt: CPU=$CPU cause=$cause\n"
#}

#annotation set pc kernel::ReadSWInt:END {
#    if { $CPU != 4 }  {
#     console "     **** ReadSWInt: CPU=$CPU v0=$v0\n"
#    }
#}