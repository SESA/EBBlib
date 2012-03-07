if { $env(SIMOSPCACHETRACE) == "ON" } {
    proc pcachetraceon {dumpfile cpu} {
	global env
	cache create $env(TESTNAME).$env(SIMOSCPUS).${dumpfile}.cs 
	cache dump $env(TESTNAME).$env(SIMOSCPUS).${dumpfile}.cs $cpu
    }
    proc pcachetraceoff {dumpfile cpu} {
	global env
	cache dump $env(TESTNAME).$env(SIMOSCPUS).${dumpfile}.cs $cpu
	cache destroy $env(TESTNAME).$env(SIMOSCPUS).${dumpfile}.cs
    }
} else {
    proc pcachetraceon {dumpfile cpu} {}
    proc pcachetraceoff {dumpfile cpu} {}
}

