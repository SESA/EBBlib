
timing create processes
#timing create total

source "insttrace.tcl"
source "pcachetrace.tcl"
source "scachetrace.tcl"
#source "lock.tcl"

log [timing fields]

set WORKERS {}

annotation type tstEvent enum {
    startworker  endworker
    startdriver  enddriver
    starttest    endtest
}

annotation set simos enter {
    annotation disable tst
    timing switch processes other_${CPU}
#    timing switch total $CPU all
}

annotation set simos exit {
    timing exit processes
    log [timing dump processes]
#    log [timing dump total]
}

annotation set osEvent startKernelProcess -tag tst {
#  log "**** startKernelProcess $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU)\n"
  set index [lsearch $WORKERS $ACTIVEPDA($CPU)]
  if { $index == -1 } {
      timing switch processes other_${CPU}
  } else {
      set CC($ACTIVEPDA($CPU))  $CYCLES
      set IC($ACTIVEPDA($CPU))  $INSTS
      timing switch processes $ACTIVEPDA($CPU)
      insttraceon "**** INSTTRACE: RESTART: for  cpu=$CPU pda=$ACTIVEPDA($CPU)"
      scachetraceon $ACTIVEPDA($CPU) "**** SCACHETRACE: RESTART: for  cpu=$CPU pda=$ACTIVEPDA($CPU)"
  }
}

annotation set osEvent endKernelProcess -tag tst {
#  log "**** startKernelProcess $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU)\n"
  set index [lsearch $WORKERS $ACTIVEPDA($CPU)]
  if { $index != -1 } {
      set CSUM($ACTIVEPDA($CPU)) [expr $CSUM($ACTIVEPDA($CPU)) + ($CYCLES - $CC($ACTIVEPDA($CPU)))]
      set ISUM($ACTIVEPDA($CPU)) [expr $ISUM($ACTIVEPDA($CPU)) + ($INSTS - $IC($ACTIVEPDA($CPU)))]
      insttraceoff "**** INSTTRACE: STOP: for cpu=$CPU pda=$ACTIVEPDA($CPU)" 
      scachetraceoff $ACTIVEPDA($CPU) "**** SCACHETRACE: STOP: for cpu=$CPU pda=$ACTIVEPDA($CPU)" 
  }
}

annotation set osEvent startIdleProcess -tag tst {
#    log "**** startIdleProcess $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU) $MODE($CPU)\n"
    timing switch processes other_${CPU}
}

annotation set osEvent startUserProcess -tag tst {
#     log "**** startUserProcess $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU) $MODE($CPU)\n"
    timing switch processes other_${CPU}
}

annotation set osEvent startExceptionLevel -tag tst {
#    log "**** startExecptionLevel $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU) $MODE($CPU)\n"
    timing switch processes other_${CPU}
}



annotation set tstEvent starttest {
#   log "**** starttest $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU) $MODE($CPU)\n"
   annotation enable tst
   pcachetraceon test_${CPU}_$ACTIVEPDA($CPU) $CPU
#   annotation enable lock
}

annotation set tstEvent endtest {
#   log "**** endtest $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU) $MODE($CPU)\n"
   annotation disable tst
#   annotation disable lock
   pcachetraceoff test_${CPU}_$ACTIVEPDA($CPU) $CPU
}

annotation set tstEvent startdriver {
#    log "**** startdriver $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU)\n"
    lappend WORKERS $ACTIVEPDA($CPU)
    set CSUM($ACTIVEPDA($CPU)) 0
    set ISUM($ACTIVEPDA($CPU)) 0
    set CC($ACTIVEPDA($CPU))  $CYCLES
    set IC($ACTIVEPDA($CPU))  $INSTS
    timing switch processes $ACTIVEPDA($CPU) driver_${CPU}_$ACTIVEPDA($CPU)
    insttraceon "**** INSTTRACE: START: for driver cpu=$CPU pda=$ACTIVEPDA($CPU)"
    pcachetraceon driver_${CPU}_$ACTIVEPDA($CPU) $CPU
    scachetraceon $ACTIVEPDA($CPU) "**** SCACHETRACE: START: for driver cpu=$CPU pda=$ACTIVEPDA($CPU)" 
}

annotation set tstEvent enddriver {
#    log "**** enddriver $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU)\n"
    set index [lsearch $WORKERS $ACTIVEPDA($CPU)]
    set WORKERS [lreplace $WORKERS $index $index]
    log "**** STATS: Driver_${CPU}_$ACTIVEPDA($CPU): Total Cycles=[expr $CSUM($ACTIVEPDA($CPU)) + ($CYCLES - $CC($ACTIVEPDA($CPU)))]\n"
    log "**** STATS: Driver_${CPU}_$ACTIVEPDA($CPU): Total Instructions=[expr $ISUM($ACTIVEPDA($CPU)) + ($INSTS - $IC($ACTIVEPDA($CPU)))]\n"
    timing end processes driver_${CPU}_$ACTIVEPDA($CPU)
    timing terminate processes $ACTIVEPDA($CPU)
    insttraceoff "**** INSTTRACE: END: for driver cpu=$CPU pda=$ACTIVEPDA($CPU)" 
    pcachetraceoff driver_${CPU}_$ACTIVEPDA($CPU) $CPU
    scachetraceoff $ACTIVEPDA($CPU) "**** SCACHETRACE: END: for driver cpu=$CPU pda=$ACTIVEPDA($CPU)" 
}

annotation set tstEvent startworker {
#    log "**** startworker $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU)\n"
    lappend WORKERS $ACTIVEPDA($CPU)
    set CSUM($ACTIVEPDA($CPU)) 0
    set ISUM($ACTIVEPDA($CPU)) 0
    set CC($ACTIVEPDA($CPU))  $CYCLES
    set IC($ACTIVEPDA($CPU))  $INSTS
    set START($ACTIVEPDA($CPU))  $CYCLES
    timing switch processes $ACTIVEPDA($CPU) worker_${CPU}_$ACTIVEPDA($CPU)
    insttraceon "**** INSTTRACE: START: for worker cpu=$CPU pda=$ACTIVEPDA($CPU)" 
    pcachetraceon worker_${CPU}_$ACTIVEPDA($CPU) $CPU
    scachetraceon $ACTIVEPDA($CPU) "**** SCACHETRACE: START: for worker cpu=$CPU pda=$ACTIVEPDA($CPU)" 
}

annotation set tstEvent endworker {
#    log "**** endworker $CPU $ACTIVEBADGE($CPU) $ACTIVEPDA($CPU)\n"
    set index [lsearch $WORKERS $ACTIVEPDA($CPU)]
    set WORKERS [lreplace $WORKERS $index $index]
    log "**** STATS: Worker_${CPU}_$ACTIVEPDA($CPU): Total Cycles=[expr $CSUM($ACTIVEPDA($CPU)) + ($CYCLES - $CC($ACTIVEPDA($CPU)))]\n"
    log "**** STATS: Worker_${CPU}_$ACTIVEPDA($CPU): Total Instructions=[expr $ISUM($ACTIVEPDA($CPU)) + ($INSTS - $IC($ACTIVEPDA($CPU)))]\n"
    log "**** STATS: Worker_${CPU}_$ACTIVEPDA($CPU): Total StarttoEnd=[expr $CYCLES - $START($ACTIVEPDA($CPU)) ]\n"    
    timing end processes worker_${CPU}_$ACTIVEPDA($CPU)
    timing terminate processes $ACTIVEPDA($CPU)
    insttraceoff "**** INSTTRACE: END: for worker cpu=$CPU pda=$ACTIVEPDA($CPU)" 
    pcachetraceoff worker_${CPU}_$ACTIVEPDA($CPU) $CPU
    scachetraceoff $ACTIVEPDA($CPU) "**** SCACHETRACE: END: for worker cpu=$CPU pda=$ACTIVEPDA($CPU)" 
}

