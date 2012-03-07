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


#annotation set simos enter {
#    annotation disable functrace
#}

#annotation set tstEvent startworker {
#    annotation enable functrace
#}

