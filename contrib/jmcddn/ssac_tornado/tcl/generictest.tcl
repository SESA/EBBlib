annotation type tstEvent enum {
    startworker  endworker
    startdriver  enddriver
    starttest    endtest
}

annotation set pc prog::tstEvent_testinfo:START {
    set NumWorkers [expr $a0 + 0]
    set NumEventsPerWorker [expr $a1 + 0]
    set NumWorkersStarted 0
    set NumWorkersEnded   0
    log "**** DEBUG: tstEvent: testinfo: NumWorkers = $NumWorkers NumEventsPerWorker=$NumEventsPerWorker\n"
}

annotation set pc prog::tstEvent_startworker:END {
    set NumWorkersStarted [expr $NumWorkersStarted + 1]
    if { $NumWorkersStarted == 1 } {
	annotation exec tstEvent starttest
    }
    annotation exec tstEvent startworker
}

annotation set pc prog::tstEvent_endworker:START {
    set NumWorkersEnded [expr $NumWorkersEnded + 1]
    annotation exec tstEvent endworker
    if { $NumWorkersEnded == $NumWorkers } {
	annotation exec tstEvent endtest
    }
}

#annotation set pc prog::tstEvent_starttest:END {
#    annotation exec tstEvent starttest
#}

#annotation set pc prog::tstEvent_endtest:START {
#    annotation exec tstEvent endtest
#}

annotation set pc prog::tstEvent_startdriver:END {
    annotation exec tstEvent startdriver
}

annotation set pc prog::tstEvent_enddriver:START {
    annotation exec tstEvent enddriver
}
