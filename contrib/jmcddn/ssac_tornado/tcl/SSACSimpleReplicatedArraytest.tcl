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

annotation set pc kernel::lruentry__Q2_25SSACSimpleReplicatedArray10HashQueuesFRCi:START -tag functrace {
    set NUMOFLRUENTRIES [expr $NUMOFLRUENTRIES + 1]
}

set NUMOFDIRTYS 0

annotation set pc kernel::dirty__16CacheEntrySimpleFv:START -tag functrace {
    set NUMOFDIRTYS [expr $NUMOFDIRTYS + 1]
}

annotation set pc kernel::rollover__Q2_25SSACSimpleReplicatedArray10HashQueuesFv:START {
    log "**** ERROR:  SSACSimpleReplicatedArray::HashQueues::rollover called\n"
}

#set NUMOFFREEREMOVES 0

#annotation set pc kernel::free_remove__Q2_21SSACSimpleReplicatedArray10HashQueuesFP16CacheEntrySimple:START {
#    set NUMOFFREEREMOVES [expr $NUMOFFREEREMOVES + 1]
#}


#set NUMOFFREEADDATHEADS 0

#annotation set pc kernel::free_addathead__Q2_21SSACSimpleReplicatedArray10HashQueuesFP16CacheEntrySimple:START {
#    set NUMOFFREEADDATHEADS [expr $NUMOFFREEADDATHEADS + 1]
#}

#set NUMOFFREEADDATTAILS 0

#annotation set pc kernel::free_addattail__Q2_21SSACSimpleReplicatedArray10HashQueuesFP16CacheEntrySimple:START {
#    set NUMOFFREEADDATTAILS [expr $NUMOFFREEADDATTAILS + 1]
#}

set NUMOFGETS 0

annotation set pc kernel::get__25SSACSimpleReplicatedArrayFR13CacheObjectIdRP10CacheEntryRCQ2_4SSAC7gettype:START -tag functrace {
#    console "***** get a0=$a0 a1=$a1\n"
    set NUMOFGETS [expr $NUMOFGETS + 1]
}

set NUMOFPUTBACKS 0

annotation set pc kernel::putback__25SSACSimpleReplicatedArrayFRP10CacheEntryRCQ2_4SSAC7putflag:START -tag functrace {
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

set NUMOFFINDDIRLINES 0

annotation set pc kernel::finddirline__25SSACSimpleReplicatedArrayFR19CacheObjectIdSimple:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFFINDDIRLINES [expr $NUMOFFINDDIRLINES + 1]
}

set NUMOFDIRLINELOOKUPIDS 0
#set CYCLESINLOOKUPID 0
#set INSTSINLOOKUPID 0
#set STARTCYCLESINLOOKUPID 0
#set STARTINSTSINLOOKUPID 0

annotation set pc kernel::lookupid__Q2_25SSACSimpleReplicatedArray7DirLineFR19CacheObjectIdSimpleRCi:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFDIRLINELOOKUPIDS [expr $NUMOFDIRLINELOOKUPIDS + 1]
#    set STARTCYCLESINLOOKUPID $CYCLES 
#    set STARTINSTSINLOOKUPID $INSTS
}

#annotation set pc kernel::lookupid__Q2_25SSACSimpleReplicatedArray7DirLineFR19CacheObjectIdSimpleRCi:END -tag functrace {
#    set CYCLESINLOOKUPID  [expr $CYCLESINLOOKUPID + ($CYCLES - $STARTCYCLESINLOOKUPID)]
#    set INSTSINLOOKUPID   [expr $INSTSINLOOKUPID + ($INSTS - $STARTINSTSINLOOKUPID)]
#    log "**** End of lookupid startcycles=$STARTCYCLESINLOOKUPID total cycles=$CYCLESINLOOKUPID ($CYCLES) startinsts=$STARTINSTSINLOOKUPID total instructions=$INSTSINLOOKUPID ($INSTS)\n"
#}

set NUMOFGETREMOTECOPIES 0
#set CYCLESINGETRC 0
#set INSTSINGETRC 0
#set STARTCYCLESINGETRC 0
#set STARTINSTSINGETRC 0

annotation set pc kernel::getremotecopyfromdir__25SSACSimpleReplicatedArrayFPQ2_25SSACSimpleReplicatedArray8DirEntry:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFGETREMOTECOPIES [expr $NUMOFGETREMOTECOPIES + 1]
#    set STARTCYCLESINGETRC $CYCLES
#    set STARTINSTSINGETRC $INSTS
}

#annotation set pc kernel::getremotecopyfromdir__25SSACSimpleReplicatedArrayFPQ2_25SSACSimpleReplicatedArray8DirEntry:END -tag functrace {
#    set CYCLESINGETRC [expr $CYCLESINGETRC + ($CYCLES - $STARTCYCLESINGETRC)]
#    set INSTSINGETRC  [expr $INSTSINGETRC + ($INSTS - $STARTINSTSINGETRC)]
#}

set NUMOFADDDIRENTRIES 0

annotation set pc kernel::adddirentry__Q2_25SSACSimpleReplicatedArray7DirLineFR19CacheObjectIdSimpleRCiT2:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFADDDIRENTRIES [expr $NUMOFADDDIRENTRIES + 1]
}

set NUMOFSETBUSYONALLS 0

annotation set pc kernel::setbusyonallcopiesindir__25SSACSimpleReplicatedArrayFPQ2_25SSACSimpleReplicatedArray8DirEntryP16CacheEntrySimple:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFSETBUSYONALLS [expr $NUMOFSETBUSYONALLS + 1]
}


set NUMOFUPDATEANDCLEARSBUSYONALLS 0

annotation set pc kernel::updateandclearbusyonallcopiesindir__25SSACSimpleReplicatedArrayFPQ2_25SSACSimpleReplicatedArray8DirEntryP16CacheEntrySimple:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFUPDATEANDCLEARSBUSYONALLS [expr $NUMOFUPDATEANDCLEARSBUSYONALLS + 1]
}

set NUMOFREMOVEFROMDIRS 0

annotation set pc kernel::removefromdir__25SSACSimpleReplicatedArrayFR19CacheObjectIdSimpleRCii:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFREMOVEFROMDIRS [expr $NUMOFREMOVEFROMDIRS + 1]
}

set NUMOFDIRENTRYSPINS 0

annotation set pc kernel::spin__Q2_25SSACSimpleReplicatedArray8DirEntryFv:START -tag functrace {
#    console "**** DEBUG: save a0=$a0\n"
    set NUMOFDIRENTRYSPINS [expr $NUMOFDIRENTRYSPINS + 1]
}


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
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray::HashQueues:: lruentry()=$NUMOFLRUENTRIES\n"
#    log "**** STATS: Number of times executed SSACSimpleReplicatedArray::HashQueues:: free_addathead()=$NUMOFFREEADDATHEADS\n"
#    log "**** STATS: Number of times executed SSACSimpleReplicatedArray::HashQueues:: free_addattail()=$NUMOFFREEADDATTAILS\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: get()=$NUMOFGETS\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: putback()=$NUMOFPUTBACKS\n"
    log "**** STATS: Number of times executed CacheObjectIdSimple:: load=$NUMOFLOADS\n"
    log "**** STATS: Number of times executed CacheObjectIdSimple:: save=$NUMOFSAVES\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: finddirline()=$NUMOFFINDDIRLINES\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: getremotecopyfromdir()=$NUMOFGETREMOTECOPIES\n"
#    log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: getremotecopyfromdirCycles=$CYCLESINGETRC\n"
#    log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: getremotecopyfromdirInstructions=$INSTSINGETRC\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: setbusyonallcopiesindir()=$NUMOFSETBUSYONALLS\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: updateandclearbusyonallcopiesindir()=$NUMOFUPDATEANDCLEARSBUSYONALLS\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray:: removefromdir()=$NUMOFREMOVEFROMDIRS\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray::DirLine:: lookupid()=$NUMOFDIRLINELOOKUPIDS\n"
#    log "**** STATS: Number of times executed SSACSimpleReplicatedArray::DirLine:: lookupidCycles=$CYCLESINLOOKUPID\n"
#    log "**** STATS: Number of times executed SSACSimpleReplicatedArray::DirLine:: lookupidInstructions=$INSTSINLOOKUPID\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray::DirLine:: adddirentry()=$NUMOFADDDIRENTRIES\n"
    log "**** STATS: Number of times executed SSACSimpleReplicatedArray::DirEntry:: spin()=$NUMOFDIRENTRYSPINS\n"
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

#annotation set pc kernel::startworker__FPP4SSACPii:START {    
#    console "**** DEBUG:startworker:START:CPU=$CPU: glp=$ACTIVEGLP($CPU) realPD=$ACTIVEREALPD($CPU) vp=$ACTIVEVP($CPU) portid=$ACTIVEPORTID($CPU) proc=$ACTIVEPROC($CPU)\n"
#    if { $CPU == 0 } {
#	console "**** DEBUG: BREAK: startworker__FPPSSACPii:START \tpc $pc\n"
#	debug
#    }
#}


#annotation set tstEvent startworker {
#    set ICOUNTS($CPU) 0
#    console "***** Worker started on $CPU\n"
#}

#annotation set pc kernel::HandleInternalTransMiss:START {
#    console "HandleInteralTransMiss on $CPU: a0=$a0 \n"
#}

#set dcount 0

#annotation set pc kernel::get__25SSACSimpleReplicatedArrayFR13CacheObjectIdRP10CacheEntryRCQ2_4SSAC7gettype:START {
#    if { $CPU == 0 } {
#        console "**** MYPDA=$ACTIVEPDA($CPU)\n"
#        set dcount [expr $dcount + 1]
#        if { $dcount == 2 } { 
#	    console "**** DEBUG: BREAK: get:START \tpc $pc\n"
#	    debug
#	}
#    } 
#}

#annotation set pc kernel::add__Q2_25SSACSimpleReplicatedArray7DirLineFR19CacheObjectIdSimpleRCiT2:START {
#    if { $CPU == 0 } {
#        console "**** MYPDA=$ACTIVEPDA($CPU)\n"
#        if { $dcount == 2 } { 
#	    console "**** DEBUG: BREAK: get:START \tpc $pc\n"
#	    debug
#	}
#    } 
#}

#annotation set pc kernel::startworker__FPP4SSACP7requesti:START {
#    if { $CPU == 0 } {
#	debug
#    }
#}

#annotation set pc 0xffffffff80026f00 {
#    console "**** break pc=$pc in worker\n"
#    debug
#}

#annotation set pc 0xffffffff80026f64 {
#    log "**** DEBUGTRACE: CPU=$CPU i=$ICOUNTS($CPU)\n"
#    set ICOUNTS($CPU) [expr $ICOUNTS($CPU) + 1]
#}