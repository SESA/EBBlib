annotation set pc kernel::worker__FP14integerCounterP7requesti:START {
    annotation exec tstEvent startworker
}

annotation set pc kernel::worker__FP14integerCounterP7requesti:END {
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

statistics create  updatestats 30 10 100
statistics create  getstats 30 10 100

annotation set pc kernel::increment__12CounterArrayFv:START {
    set updatestart($CPU) $CYCLES
}

annotation set pc kernel::increment__12CounterArrayFv:END {
    set ucycles [expr $CYCLES - $updatestart($CPU)]
    statistics entry updatestats $ucycles
#    log "**** STATS: inc: $ucycles\n"
}

annotation set pc kernel::decrement__12CounterArrayFv:START {
    set updatestart($CPU) $CYCLES
}

annotation set pc kernel::decrement__12CounterArrayFv:END {
    set ucycles [expr $CYCLES - $updatestart($CPU)]
    statistics entry updatestats $ucycles 
#    log "**** STATS: dec: $ucycles\n"
}

annotation set pc kernel::value__12CounterArrayFRi:START {
    set getstart($CPU) $CYCLES
}

annotation set pc kernel::value__12CounterArrayFRi:END {
    set gcycles [expr $CYCLES - $getstart($CPU)]
    statistics entry getstats $gcycles
#    log "**** STATS: val: $gcycles\n"
}

annotation set simos exit {
    log "**** STATS: updatestats: [statistics list updatestats]\n"
    log "**** STATS: getstats: [statistics list getstats]\n"
}

#annotation set simos enter {
#    annotation disable functrace
#}

#annotation set tstEvent startworker {
#    annotation enable functrace
#}

