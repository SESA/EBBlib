source "pf.tcl"
source "obj.tcl"
source "lock.tcl"

#OBJtraceclass $KERNEL "LockDebugTemplate<SimpleSpinBlockLockNDebug>|ProgramDefault|RegionSimple|HATProgramDefault|HATRegionDefault" PFobjects

OBJtraceclass $KERNEL "ProgramDefault|RegionSimple|FCMSimple|FCMPartitionedSimple|HATProgramDefault|HATRegionDefault" PFobjects

#set foo [symbol read "kernel::&((struct ProgramDefault *)0x0)->_lock._lock"]
#log "*****  address of ProgramDefault::_lock._lock = $foo\n"

#set foo [symbol read "kernel::((struct ProgramDefault *)0x0)->_lock"]
#log "***** offset of ProgramDefault::_lock = $foo\n"

#set foo [symbol read "kernel::((struct HATProgramDefault *)0x0)->_lock"]
#log "***** offset of HATProgramDefault::_lock = $foo\n"


#set foo [symbol read "kernel::((struct HATRegionDefault *)0x0)->_lock"]
#log "***** offset of HATRegionDefault::_lock = $foo\n"

#set foo [symbol read "kernel::((struct RegionSimple *)0x0)->_lock"]
#log "***** offset of RegionSimple::_lock = $foo\n"

#set foo [symbol read "kernel::((struct FCMSimple *)0x0)->_lock"]
#log "***** offset of FCMSimple::_lock = $foo\n"

annotation set pc kernel::handleFault__14ProgramDefaultFUlT1Q2_3HAT11enum_PFType:START -tag PFobjects {
    set lockaddr [symbol read "kernel::((struct ProgramDefault *)$a0)->_lock"]
#    log "**** DEBUG: PFTST: &ProgramDefault::_lock = $lockaddr\n"
    set LockNames($lockaddr) "$a0 : ProgramDefault::_lock"
}

annotation set pc kernel::handleFault__12RegionSimpleFPP2PDUlT2Q2_3HAT11enum_PFType:START -tag PFobjects {
    set lockaddr [symbol read "kernel::((struct RegionSimple *)$a0)->_lock"]
#    log "**** DEBUG: PFTST: &RegionSimple::_lock = $lockaddr\n"
    set LockNames($lockaddr) "$a0 : RegionSimple::_lock"
}

annotation set pc kernel::setMapping__17HATProgramDefaultFUlN41:START -tag PFobjects {
    set lockaddr [symbol read "kernel::((struct HATProgramDefault *)$a0)->_lock"]
#    log "**** DEBUG: PFTST: &HATProgramDefault::_lock = $lockaddr\n"
    set LockNames($lockaddr) "$a0 : HATProgramDefault::_lock"
}

annotation set pc kernel::setMapping__16HATRegionDefaultFUlN41:START -tag PFobjects {
    set lockaddr [symbol read "kernel::((struct HATRegionDefault *)$a0)->_lock"]
#    log "**** DEBUG: PFTST: &HATRegionDefault::_lock = $lockaddr\n"
    set LockNames($lockaddr) "$a0 : HATRegionDefault::_lock"
}

annotation set pc kernel::find__9FCMSimpleFUlT1Q2_3FCM15enum_AccessTypePP6RegionPvT5RUlRUi:START -tag PFobjects {
    set lockaddr [symbol read "kernel::((struct FCMSimple *)$a0)->_lock"]
#    log "**** DEBUG: PFTST: &FCMSimple::_lock = $lockaddr\n"
    set LockNames($lockaddr) "$a0 : FCMSimple::_lock"
}

annotation set pc kernel::find__20FCMPartitionedSimpleFUlT1Q2_3FCM15enum_AccessTypePP6RegionPvT5RUlRUi:START -tag PFobjects {
    set lockaddr [symbol read "kernel::((struct FCMPartitionedSimple *)$a0)->_lock"]
    log "**** DEBUG: PFTST: &FCMPartitionedSimple::_lock = $lockaddr\n"
    set LockNames($lockaddr) "$a0 : FCMPartitionedSimple::_lock"
}

annotation set tstEvent starttest {
    set SCYCLESTEST $CYCLES
    OBJtraceobjectson PFobjects
    Locktraceon
    log "**** DEBUG: PFTST: Test Started on $CPU\n"
}
 
annotation set tstEvent endtest {
    log "**** DEBUG: PFTST: Test Ended on $CPU\n"
    OBJtraceobjectsoff PFobjects
    Locktraceoff
    set tstcycles [ expr $CYCLES - $SCYCLESTEST ]
    log "**** STATS: PFTST: Total cycles=$tstcycles\n" 
}

annotation set tstEvent startworker {
    set id "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    set SCYCLESWORKER($id) $CYCLES
    set SINSTSWORKER($id) $INSTS
    log "**** DEBUG: startworker CPU=$CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU)\n"    
    PFtraceon
}

annotation set tstEvent endworker {
    set id "$ACTIVEBADGE($CPU):$ACTIVEPDA($CPU)"
    set wcycles [ expr $CYCLES - $SCYCLESWORKER($id) ]
    set winsts [ expr $INSTS - $SINSTSWORKER($id) ]
    log "**** DEBUG: endworker CPU=$CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU)\n"
    log "**** STATS: PFWorker: CPU=$CPU id $id Total cycles=$wcycles\n" 
    log "**** STATS: PFWorker: CPU=$CPU id $id Total instructions=$winsts\n" 
    PFtraceoff
}

